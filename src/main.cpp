
// Used for standard int size defines
#include <cstdint>

// Common C NES libary that includes a simple NMI update routine
#include <cstdlib>
#include <neslib.h>
#include <soa.h>
#include <string.h>

// Add-ons to the neslib, bringing metatile support and more
#include <nesdoug.h>

// Include our own player update function for the movable sprite.
#include "common.hpp"
#include "graphics.hpp"
#include "game.hpp"
#include "text_render.hpp"
#include "levels.hpp"
#include "metatile.hpp"


// Trim the pal bright table back to just what we use
__asm__(R"ASM(
.section .rodata.bright_table,"a",@progbits
.globl __palBrightTableL
.globl __palBrightTableH
__palBrightTableL:

	.byte palBrightTable0@mos16lo,palBrightTable1@mos16lo,palBrightTable2@mos16lo
	.byte palBrightTable3@mos16lo,palBrightTable4@mos16lo

__palBrightTableH:

	.byte palBrightTable0@mos16hi,palBrightTable1@mos16hi,palBrightTable2@mos16hi
	.byte palBrightTable3@mos16hi,palBrightTable4@mos16hi

palBrightTable0:
	.byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f	;black
palBrightTable1:
	.byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
palBrightTable2:
	.byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
palBrightTable3:
	.byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
palBrightTable4:
	.byte $00,$01,$02,$03,$04,$05,$06,$07,$08,$09,$0a,$0b,$0c,$0f,$0f,$0f	;normal colors
palBrightTable5:
	.byte $10,$11,$12,$13,$14,$15,$16,$17,$18,$19,$1a,$1b,$1c,$00,$00,$00
palBrightTable6:
	.byte $10,$21,$22,$23,$24,$25,$26,$27,$28,$29,$2a,$2b,$2c,$10,$10,$10	;$10 because $20 is the same as $30
palBrightTable7:
	.byte $30,$31,$32,$33,$34,$35,$36,$37,$38,$39,$3a,$3b,$3c,$20,$20,$20
)ASM"
);

// On the Game Genie, only color 0 and 3 of each palette will be used
static const uint8_t default_palette[32] = {
// BG Palette
    0x0f, 0x0f, 0x0f, 0x37,
    0x0f, 0x0f, 0x0f, 0x11,
    0x0f, 0x0f, 0x0f, 0x16,
    0x0f, 0x0f, 0x0f, 0x29,
// Sprite Palette
    0x0f, 0x0f, 0x0f, 0x0f,
    0x0f, 0x0f, 0x0f, 0x22,
    0x0f, 0x0f, 0x0f, 0x12,
    0x0f, 0x0f, 0x0f, 0x02,
};

GameMode prev_mode;
GameMode game_mode;
uint8_t level;

static bool title_screen_draw_player;
static bool title_screen_draw_cursor;

void set_game_mode(GameMode mode) {
    prev_mode = game_mode;
    game_mode = mode;
}

extern volatile uint8_t currentSFX;
__attribute__((leaf)) extern "C"  void sabre_playSFX();
void playsfx(uint8_t sfx) {
    currentSFX = sfx;
    sabre_playSFX();
}

constexpr uint8_t DRAW_END = 0;
constexpr uint8_t DRAW_STRING = 1;
constexpr uint8_t DRAW_CURSOR = 2;
[[maybe_unused]] constexpr uint8_t DRAW_PLAYER_UP = 3;
[[maybe_unused]] constexpr uint8_t DRAW_PLAYER_RIGHT = 4;
[[maybe_unused]] constexpr uint8_t DRAW_PLAYER_DOWN = 5;
[[maybe_unused]] constexpr uint8_t DRAW_PLAYER_LEFT = 6;

extern const uint8_t title_screen_0[] = {
    DRAW_STRING, 3, 6, LETTERS_13("STEP BY STEP"),
    DRAW_STRING, 3, 9, LETTERS_12("BY JROWEBOY"),
    // DRAW_STRING, 8, 12, LETTERS_7("HEYDON"),
    DRAW_STRING, 3, 17, LETTERS_11("PRESS DOWN"),
    DRAW_STRING, 3, 20, LETTERS_11("FOR MANUAL"),
    DRAW_PLAYER_DOWN, 13 * 8, 24 * 8,
    DRAW_END
};
extern const uint8_t title_screen_1[] = {
    DRAW_STRING, 2, 6, LETTERS_16("GUIDE THE ARROW"),
    DRAW_PLAYER_DOWN, 14 * 8, 10 * 8,
    DRAW_STRING, 2, 14, LETTERS_15("TO ALL PICKUPS"),
    M_ONE(14, 18, Metatile::ITEM),
    A_HORZ(1, 14, 18, A_BR(BG_PALETTE_GREEN)),
    DRAW_STRING, 2, 22, LETTERS_13("IN THE LEVEL"),
    DRAW_CURSOR, 14 * 8, 18 * 8,
    DRAW_END
};
extern const uint8_t title_screen_2[] = {
    DRAW_STRING, 2, 3, LETTERS_16("ADD COMMANDS TO"),
    DRAW_STRING, 2, 6, LETTERS_9("THE LIST"),
    M_HORZ(3, 10, 10, Metatile::SEPARATOR, Metatile::SUB, Metatile::SEPARATOR),
    A_HORZ(2, 10, 10, A_BR(BG_PALETTE_BLUE), A_BL(BG_PALETTE_BLUE) | A_BR(BG_PALETTE_BLUE)),
    DRAW_STRING, 2, 13, LETTERS_13("AND RUN THEM"),
    DRAW_STRING, 2, 16, LETTERS_11("WITH START"),
    A_HORZ(4, 10, 18, A_TR(BG_PALETTE_GREEN) | A_BR(BG_PALETTE_GREEN), 0xff, 0xff, 0xff), // BG_PALETTE_GREEN
    DRAW_PLAYER_DOWN, 13 * 8, 24 * 8,
    DRAW_END
};
extern const uint8_t title_screen_3[] = {
    DRAW_STRING, 7, 3, LETTERS_9("CONTROLS"),
    DRAW_STRING, 2, 7, LETTERS_17("A TO ADD COMMAND"),
    DRAW_CURSOR, 2 * 8 - 2, 7 * 8 + 2,
    DRAW_STRING, 2, 11, LETTERS_17("B TO MOVE CURSOR"),
    DRAW_STRING, 2, 15, LETTERS_15("SELECT TO MOVE"),
    DRAW_STRING, 2, 18, LETTERS_14("TO SUBROUTINE"),
    DRAW_PLAYER_DOWN, 13 * 8, 24 * 8,
    DRAW_END
};
extern const uint8_t title_screen_4[] = {
    DRAW_STRING, 5, 3, LETTERS_13("PRESS SELECT"),
    DRAW_STRING, 1, 6, LETTERS_17("FOR PASSWORD BOX"),
    DRAW_STRING, 11, 13, LETTERS_5("HINT"),
    DRAW_STRING, 5, 16, LETTERS_13("IF STUCK TRY"),
    DRAW_STRING, 1, 19, LETTERS_18("FAMOUS CHEATCODE"),
    DRAW_END
};

constexpr uint8_t TITLE_SCREEN_COUNT = 4;
SPLIT_ARRAY(title_screen_data,
    title_screen_0,
    title_screen_1,
    title_screen_2,
    title_screen_3,
    title_screen_4
);

void draw_title_screen(uint8_t idx) {
    ppu_off();
    vram_adr(NAMETABLE_A);
    vram_fill(0, 0x400);
    uint8_t* screen = (uint8_t*) SPLIT_ARRAY_POINTER(title_screen_data, idx);
    title_screen_draw_player = false;
    title_screen_draw_cursor = false;

    uint8_t i = 0;
    uint8_t cmd;
    while ((cmd = screen[i++])) {
        uint8_t x = screen[i++];
        uint8_t y = screen[i++];
        if (cmd == DRAW_STRING) {
            uint8_t len = screen[i];
            render_string(Nametable::A, x, y, (const Letter*)&screen[i]);
            i += len;
        } else if (cmd == DRAW_CURSOR) {
            title_screen_draw_cursor = true;
            auto cursor = objects[SLOT_MAINCURSOR];
            cursor.long_timer = 4;
            cursor.x = x;
            cursor.y = y;
        } else if (cmd <= DRAW_PLAYER_LEFT) {
            title_screen_draw_player = true;
            uint8_t direction = cmd - DRAW_PLAYER_UP;
            auto obj = objects[0];
            obj.facing_dir = direction;
            obj.x = x;
            obj.y = y;
        } else {
            auto idx = VRAM_INDEX;
            VRAM_BUF[idx + 0] = cmd;
            VRAM_BUF[idx + 1] = x;
            VRAM_BUF[idx + 2] = y;
            uint8_t l = 0;
            while (l++ < y) {
                VRAM_BUF[idx + 2 + l] = screen[i++];
            }
            VRAM_INDEX += y + 3;
            VRAM_BUF[VRAM_INDEX] = 0xff;
        }
        flush_vram_update2();
    }
    ppu_wait_nmi();
    ppu_on_all();
    pal_fade(true);
}

void game_mode_title() {
    uint8_t current_screen = 0;
    uint8_t prev_screen = 0;
    if (prev_mode != MODE_TITLE) {
        prev_mode = MODE_TITLE;
        // Initialize title screen
        draw_title_screen(current_screen);
    }

    auto player = objects[SLOT_PLAYER];
    auto cursor = objects[SLOT_MAINCURSOR];
    player.long_timer = 4;
    cursor.long_timer = 4;

    while (true) {
        ppu_wait_nmi();
        pad_poll(0);
        oam_clear();

        auto input = get_pad_new(0);
        if (input & PAD_START) {
            pal_fade(false);
            level = 0;
            set_game_mode(MODE_LOAD_LEVEL);
            return;
        } else if (input & PAD_SELECT) {
            pal_fade(false);
            set_game_mode(MODE_PASSWORD);
            return;
        }

        if (input & PAD_DOWN) {
            current_screen = MMIN(current_screen+1, TITLE_SCREEN_COUNT);
        } else if (input & PAD_UP) {
            current_screen = MMAX(current_screen-1, 0);
        }

        if (current_screen != prev_screen) {
            prev_screen = current_screen;
            pal_fade(false);
            draw_title_screen(current_screen);
        }

        if (player.timer != 0) {
            player.timer--;
        }
        if (cursor.timer != 0) {
            cursor.timer--;
        }

        if (title_screen_draw_player) {
            draw_player();
        }
        if (title_screen_draw_cursor) {
            draw_cursor(SLOT_MAINCURSOR);
        }
    }
}

FIXED const uint8_t starfield_screen[] = {
    T_HORZ_REPT(0x20, 0, 7, 0x03),
    T_HORZ_REPT(0x20, 0, 20, 0x0c),
    T_ONE(31, 7, 0x0f),
    A_HORZ(0x3, 22, 14, A_BR(BG_PALETTE_GREEN), A_BL(BG_PALETTE_GREEN) | A_BR(BG_PALETTE_GREEN), A_BL(BG_PALETTE_GREEN)),
    A_HORZ(0x3, 22, 16, A_TR(BG_PALETTE_GREEN), A_TL(BG_PALETTE_GREEN) | A_TR(BG_PALETTE_GREEN), A_TL(BG_PALETTE_GREEN)),
    NT_UPD_EOF
};

FIXED constexpr const int8_t STAR_X_VELOCITY[] = {
    -1, -2, -3, -4
};
FIXED constexpr const int8_t STAR_Y_VELOCITY[] = {
    1, 2, 3, 4
};
FIXED constexpr const uint8_t STAR_TILE_LUT[] = {
    0x01,
    0x09,
    0x09,
    0x0f,
};
FIXED constexpr const uint8_t STAR_ATTR_LUT[] = {
    3,
    2,
    2,
    1,
};

extern volatile char PPUMASK_VAR;
extern __zp unsigned char SPRID;
extern char OAM_BUF[256];

// Putting these calls in a separate function keeps llvm-mos
// from putting the delay function parameters in a register
// and lets it use immediate loads instead, saving a bunch of time and space
__attribute__((noinline)) static void delay_and_hide_sprites() {
    while (!(PEEK(0x2002) & 0x40))
        ;
    // Fixed delay for waiting to disable sprites
    constexpr uint8_t delay_a1 = 0x01;
    constexpr uint8_t delay_x1 = 0x28;
    delay_256a_x_33_clocks(delay_a1, delay_x1);
    POKE(0x2001, PPUMASK_VAR & (~0b00010000));
    constexpr uint8_t delay_a2 = 0x03;
    constexpr uint8_t delay_x2 = 0xb8;
    delay_256a_x_33_clocks(delay_a2, delay_x2);
    POKE(0x2001, PPUMASK_VAR | (0b00010000));
    constexpr uint8_t delay_a3 = 0x25;
    constexpr uint8_t delay_x3 = 0x70;
    delay_256a_x_33_clocks(delay_a3, delay_x3);
    POKE(0x2001, PPUMASK_VAR & (~0b00010000));
    constexpr uint8_t delay_a4 = 0x03;
    constexpr uint8_t delay_x4 = 0x48;
    delay_256a_x_33_clocks(delay_a4, delay_x4);
    POKE(0x2001, PPUMASK_VAR | (0b00010000));
}

static bool victory_mode;


static uint8_t input_password[4] = {};
static uint8_t cursor_selected = 0;
static uint8_t input_position = 0;

static bool exit_starfield_effect;
__attribute__((noinline)) static void handle_password_input() {
    auto input = get_pad_new(0);
    auto cursor = objects[SLOT_MAINCURSOR];
    auto inputcursor = objects[SLOT_CMDCURSOR];
    // password input options
    if (input & PAD_SELECT) {
        set_game_mode(MODE_RESET);
        exit_starfield_effect = true;
        return;
    }
    if (input & PAD_START) {
        // check all passwords for a match
        for (uint8_t i = 0; i < LEVEL_COUNT; i++) {
            auto password = level_passwords[i];
            auto [xhi, xlo] = UNPACK((uint8_t)((password >> 8) & 0xff));
            auto [yhi, ylo] = UNPACK((password >> 0) & 0xff);
            if (xhi == input_password[0] && xlo == input_password[1]
                && yhi == input_password[2] && ylo == input_password[3]) {
                // success!
                level = i;
                exit_starfield_effect = true;
                set_game_mode(MODE_LOAD_LEVEL);
                return;
            }
        }
        
        // TODO: password bad sfx
        playsfx(sfx_error);
    }
    if (input & PAD_A) {
        input_password[input_position++] = cursor_selected;
        uint8_t x = inputcursor->x >> 3;
        uint8_t y = inputcursor->y >> 3;
        draw_metatile_2_3(Nametable::A, x, y, password_alphabet[cursor_selected]);
        x = inputcursor->x;
        wrapped_add(x, 16, 160, 176 + 16*3);
        set_cursor_target(SLOT_CMDCURSOR, Coord{ .x = x, .y= inputcursor->y});
    }
    if (input & PAD_B) {
        uint8_t x = inputcursor->x;
        wrapped_sub(x, 16, 176, 176 + 16*4);
        set_cursor_target(SLOT_CMDCURSOR, Coord{ .x = x, .y= inputcursor->y});
    }
    uint8_t cursor_x = cursor->x;
    uint8_t cursor_y = cursor->y;
    if (input & PAD_LEFT) {
        wrapped_sub(cursor_x, 16, 24, 152);
    } else if (input & PAD_RIGHT) {
        wrapped_add(cursor_x, 16, 8, 136);
    }
    if (input & (PAD_UP | PAD_DOWN)) {
        cursor_y = cursor_y == 104 ? 128 : 104;
    }
    set_cursor_target(SLOT_MAINCURSOR, Coord{ .x = cursor_x, .y= cursor_y});
    uint8_t tile_x_pos = ((cursor_x >> 4) - 1);
    cursor_selected = (cursor_y == 104) ? tile_x_pos : tile_x_pos + 8;
}

__attribute__((noinline)) int myrand() {
    return rand();
}

__attribute__((cold)) static void update_starfield(bool password_input) {
    // uint8_t star_x_lo[32];
    uint8_t star_x[32];
    // uint8_t star_y_lo[32];
    uint8_t star_y[32];
    uint8_t star_type[32];
    auto cursor = objects[SLOT_MAINCURSOR];
    GameMode orig_game_mode = game_mode;
    exit_starfield_effect = false;

    for (uint8_t i = 31; i < 128; i--) {
        star_x[i] = (myrand() & 0xff);
        star_y[i] = (myrand() & 0xff);
        uint8_t type = myrand() & 0x03;
        star_type[i] = type;
    }

    OAM_BUF[0] = 55;
    OAM_BUF[1] = 0xf;
    OAM_BUF[2] = 0x0;
    OAM_BUF[3] = 248;
    SPRID += 4;

    ppu_wait_nmi();
    ppu_on_all();
    pal_fade(true);
    while (true) {
        ppu_wait_nmi();
        SPRID = 4;
        // move the stars
        for (uint8_t i = 31; i < 128; i--) {
            uint8_t type = star_type[i];
            star_x[i] += STAR_X_VELOCITY[type];
            star_y[i] += STAR_Y_VELOCITY[type];
            auto real_y = star_y[i] > 56 && star_y[i] < 158 ? 255 : star_y[i];
            OAM_BUF[SPRID + 0] = real_y;
            OAM_BUF[SPRID + 3] = star_x[i];
            OAM_BUF[SPRID + 1] = STAR_TILE_LUT[type];
            OAM_BUF[SPRID + 2] = STAR_ATTR_LUT[type];
            SPRID += 4;
            if (star_y[i] > 250) {
                star_x[i] = (myrand() & 0xff);
                star_type[i] = myrand() & 0x03;
            }
        }
        // wait for sprite zero
        delay_and_hide_sprites();


        OAM_BUF[0] = 55;
        OAM_BUF[1] = 0xf;
        OAM_BUF[2] = 0x0;
        OAM_BUF[3] = 248;

        auto input = pad_trigger(0);

        if (!password_input) {
            if (input & (PAD_START | PAD_A | PAD_B | PAD_SELECT) && !victory_mode) {
                break;
            }
            continue;
        } else if (!cursor->is_moving) {
            handle_password_input();
        }

        move_object(SLOT_MAINCURSOR);
        move_object(SLOT_CMDCURSOR);
        
        draw_cursor(SLOT_MAINCURSOR);
        draw_cursor(SLOT_CMDCURSOR);
        if (cursor.timer != 0) {
            cursor.timer--;
        }
        if (game_mode != orig_game_mode)
            break;
    }
    pal_fade(false);
    ppu_off();
    vram_adr(NAMETABLE_A);
    vram_fill(0, 0x400);
}

static void draw_starscreen() {
    ppu_off();
    vram_adr(NAMETABLE_A);
    vram_fill(0, 0x400);
    // vram_adr(0x2100);
    // vram_fill(0xf, 0x20 * 12);
    // replace the last color with black so we can use it as a background to hide sprites
    // pal_col(0x0f, 0x0f);

    set_vram_update(starfield_screen);
    flush_vram_update2();
    set_vram_buffer();
}

void game_mode_enter_password() {
    memset((void*)&objects, 0, sizeof(objects));
    auto cursor = objects[SLOT_MAINCURSOR];
    auto cmdcursor = objects[SLOT_CMDCURSOR];
    cursor.x = 24;
    cursor.y = 104;
    cursor.target_x = 24;
    cursor.target_y = 104;
    cursor.anim_state = 12;
    cursor.state = 20;
    cursor.timer = 0;
    cursor.long_timer = 4;
    cursor.x_vel = 8;
    cursor.y_vel = 8;
    cmdcursor.x = 176;
    cmdcursor.y = 112;
    cmdcursor.target_x = 176;
    cmdcursor.target_y = 112;
    cmdcursor.x_vel = 8;
    cmdcursor.y_vel = 8;
    cmdcursor.anim_state = 12;
    cmdcursor.state = 20;

    draw_starscreen();
    render_string(Nametable::A, 3, 9, "ENTER PASSWORD"_l);
    for (uint8_t j=0; j<2; j++) {
        uint8_t x = 3;
        uint8_t y = j == 0 ? 13 : 16;
        uint8_t offset = j == 0 ? 0 : 8;
        for (uint8_t i=0; i< 8; i++) {
            draw_metatile_2_3(Nametable::A, x, y, password_alphabet[i + offset]);
            flush_vram_update2();
            x += 2;
        }
    }

    update_starfield(true);
}

__attribute__((noinline)) static void reset_cursors() {

constexpr uint8_t X_LO_BOUND = (10 * 8);
constexpr uint8_t Y_LO_BOUND = (22 * 8);
    auto cursor = objects[SLOT_MAINCURSOR];
    cursor.type = CURSOR;
    cursor.is_moving = false;
    cursor.x = X_LO_BOUND;
    cursor.target_x = X_LO_BOUND;
    cursor.y = Y_LO_BOUND;
    cursor.target_y = Y_LO_BOUND;
    // cursor.x_vel = 0;
    // cursor.y_vel = 0;
    // cursor.frame = 0;
    cursor.state = 16;
    cursor.anim_state = 16;
    cursor.long_timer = 4;
    // cursor.timer = 0;
    // cursor.anim_timer = 0;
    // cursor.param2 = 0;
    // cursor.facing_dir = 0;

    auto cmdcursor = objects[SLOT_CMDCURSOR];
    cmdcursor.type = CURSOR;
    // cmdcursor.is_moving = false;
    // cmdcursor.x_vel = 0;
    // cmdcursor.y_vel = 0;
    // cmdcursor.frame = 0;
    cmdcursor.state = 16;
    cmdcursor.anim_state = 16;
    // Reset level variables
    // Force the sub index for the command slots
    command_index[0] = 0;
    command_index[1] = 12;
    command_index[2] = 12 + 9;
}

void game_mode_victory_screen() {
    draw_starscreen();
    
    render_string(Nametable::A, 1, 9, "CONGRATULATIONS"_l);
    render_string(Nametable::A, 7, 12, "HOPE YOU"_l);
    render_string(Nametable::A, 7, 15, "LIKED IT"_l);
    victory_mode = true;
    update_starfield(false);
}

void game_mode_load_level() {
    memset((void*)&objects, 0, sizeof(objects));
    memset(level_metatiles, 0, sizeof(level_metatiles));
    auto player = objects[SLOT_PLAYER];
    player.type = PLAYER;
    pickup_count = 0;
    current_sub = 0;
    reset_cursors();
    memset(commands, 0, sizeof(commands));
    memset(pickup_list, 0xff, sizeof(pickup_list));

    // SKIP drawing the next level screen if we are testing levels
    // draw the next level screen
    draw_starscreen();
#ifndef TEST_LEVEL_SOLUTION

    const Letter* title = (const Letter*)SPLIT_ARRAY_POINTER(level_titles, level);

    // center the level title
    uint8_t len = title[0];
    uint8_t x = (16 - (uint8_t)(len / 2)) / 2;
    uint8_t y = 9;
    // generate_password(level);
    render_string(Nametable::A, x, y, title);
    render_string(Nametable::A, 3, 15, "PASSWORD"_l);
    auto password = level_passwords[level].get();
    auto [xhi, xlo] = UNPACK((uint8_t)((password >> 8) & 0xff));
    auto [yhi, ylo] = UNPACK((password >> 0) & 0xff);
    Letter pass[5];
    pass[0] = (Letter)5;
    pass[1] = password_alphabet[xhi];
    pass[2] = password_alphabet[xlo];
    pass[3] = password_alphabet[yhi];
    pass[4] = password_alphabet[ylo];
    render_string(Nametable::A, 22, 15, pass);

    update_starfield(false);
#endif
    // if (prev_mode != MODE_LOAD_LEVEL && prev_mode != MODE_EDIT && prev_mode != MODE_EXECUTE) {
    //     prev_mode = MODE_LOAD_LEVEL;
    draw_hud(level);
    // }

    load_level(level);
    ppu_wait_nmi();

    set_game_mode(MODE_EDIT);
}

void game_mode_edit() {
    if (prev_mode != MODE_EDIT && prev_mode != MODE_EXECUTE) {
        prev_mode = MODE_EDIT;
        ppu_on_all();
        pal_fade(true);
    }
    game_mode_edit_main();
}

void game_mode_execute() {

    // i dare you to inline this llvm-mos. bring it on.
    game_mode_execute_main();
}

int main() {
    srand(0x4321);
    // Tell NMI to update graphics using the VRAM_BUFFER provided by nesdoug library
    set_vram_buffer();
    
    // Start off by disabling the PPU rendering, allowing us to upload data safely to the nametable (background)
    ppu_off();

    // Set to use 8x8 sprite mode. I doubt 8x16 sprite mode will help much with the game genie CHR :)
    oam_size(0);

    // Upload a basic palette we can use later.
    pal_all(default_palette);

    pal_bright(0);
    flush_vram_update2();
    
    // Set the scroll position on the screen to 0, 0
    scroll(0, 0);
    // Turn on the screen, showing both the background and sprites
    // ppu_on_all();
    
#ifdef TEST_LEVEL_SOLUTION
    speed_setting = 0;
#endif

    // Now time to start the main game loop
    while (true) {
        // Wait for the next frame before looping again
        ppu_wait_nmi();
        // At the start of the frame, poll the controller to get the latest state.
        pad_poll(0);
        // Once a frame, clear the sprites out so that we don't have leftover sprites.
        oam_clear();

        switch (game_mode) {
        case MODE_RESET:
#ifdef TEST_LEVEL_SOLUTION
            game_mode = MODE_LOAD_LEVEL;
            break;
#else
            reset_cursors();
#endif
            // fallthrough
        case MODE_TITLE:
            game_mode_title();
            break;
        case MODE_LOAD_LEVEL:
            if (level == LEVEL_COUNT) {
                game_mode_victory_screen();
            }
            game_mode_load_level();
            break;
        case MODE_EDIT:
            game_mode_edit();
            break;
        case MODE_EXECUTE:
            game_mode_execute();
            break;
        case MODE_PASSWORD:
        // force the compiler to stop inlining load level
            // uint8_t random = rand();
            // level = random;
            game_mode_enter_password();
            break;
        }
    }
    // Tell the compiler we are never stopping the game loop!
    __builtin_unreachable();
}
