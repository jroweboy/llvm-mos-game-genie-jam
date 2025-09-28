
// Used for standard int size defines
#include <cstdint>

// Common C NES libary that includes a simple NMI update routine
#include <neslib.h>

// Add-ons to the neslib, bringing metatile support and more
#include <nesdoug.h>

// Include our own player update function for the movable sprite.
#include "common.hpp"
#include "graphics.hpp"
#include "game.hpp"
#include "text_render.hpp"
#include "levels.hpp"
#include "metatile.hpp"

// Include a basic nametable thats RLE compressed for the demo
// const unsigned char nametable[] = {
//     #embed "../default-nametable-rle.nam"
// };

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
    0x0f, 0x0f, 0x0f, 0x0f,
    0x0f, 0x0f, 0x0f, 0x0f,
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
    A_HORZ(1, 14, 18, A_BL(BG_PALETTE_GREEN)),
    DRAW_STRING, 2, 22, LETTERS_13("IN THE LEVEL"),
    DRAW_CURSOR, 14 * 8, 18 * 8,
    DRAW_END
};
extern const uint8_t title_screen_2[] = {
    DRAW_STRING, 2, 3, LETTERS_16("ADD COMMANDS TO"),
    DRAW_STRING, 2, 6, LETTERS_9("THE LIST"),
    M_HORZ(3, 10, 10, Metatile::SEPARATOR, Metatile::SUB, Metatile::SEPARATOR),
    A_HORZ(2, 10, 10, A_BL(BG_PALETTE_BLUE), A_BL(BG_PALETTE_BLUE) | A_BR(BG_PALETTE_BLUE)),
    DRAW_STRING, 2, 13, LETTERS_13("AND RUN THEM"),
    DRAW_STRING, 2, 16, LETTERS_11("WITH START"),
    A_HORZ(2, 14, 18, 0xff, 0xff), // BG_PALETTE_GREEN
    DRAW_PLAYER_DOWN, 13 * 8, 24 * 8,
    DRAW_END
};

constexpr uint8_t TITLE_SCREEN_COUNT = 2;
SPLIT_ARRAY(title_screen_data,
    title_screen_0,
    title_screen_1,
    title_screen_2
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
        if (cmd == DRAW_STRING) {
            uint8_t x = screen[i++];
            uint8_t y = screen[i++];
            uint8_t len = screen[i];
            render_string(Nametable::A, x, y, (const Letter*)&screen[i]);
            i += len;
        } else if (cmd == DRAW_CURSOR) {
            title_screen_draw_cursor = true;
            uint8_t x = screen[i++];
            uint8_t y = screen[i++];
            auto cursor = cursors[0];
            cursor.param1 = 4;
            cursor.x = x;
            cursor.y = y;
        } else if (cmd <= DRAW_PLAYER_LEFT) {
            title_screen_draw_player = true;
            uint8_t x = screen[i++];
            uint8_t y = screen[i++];
            uint8_t direction = cmd - DRAW_PLAYER_UP;
            auto obj = objects[0];
            obj.facing_dir = direction;
            obj.x = x;
            obj.y = y;
        } else {
            auto idx = VRAM_INDEX;
            VRAM_BUF[idx + 0] = cmd;
            VRAM_BUF[idx + 1] = screen[i++];
            uint8_t len = screen[i++];
            VRAM_BUF[idx + 2] = len;
            uint8_t l = 0;
            while (l++ < len) {
                VRAM_BUF[idx + 2 + l] = screen[i++];
            }
            VRAM_INDEX += len + 3;
            VRAM_BUF[VRAM_INDEX] = 0xff;
        }
        flush_vram_update2();
    }
    ppu_wait_nmi();
    ppu_on_all();
    pal_fade_to(0, 4, 2);
}

void game_mode_title() {
    uint8_t current_screen = 0;
    uint8_t prev_screen = 0;
    if (prev_mode != MODE_TITLE) {
        prev_mode = MODE_TITLE;
        // Initialize title screen
        draw_title_screen(current_screen);
    }


    auto player = objects[0];
    auto cursor = cursors[0];
    player.long_timer = 4;
    cursor.param1 = 4;

    while (true) {
        ppu_wait_nmi();
        pad_poll(0);
        oam_clear();

        auto input = get_pad_new(0);
        if (input & PAD_START) {
            pal_fade_to(4, 0, 4);
            level = 0;
            set_game_mode(MODE_LOAD_LEVEL);
            return;
        } else if (input & PAD_SELECT) {
            pal_fade_to(4, 0, 4);
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
            pal_fade_to(4, 0, 2);
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
            draw_cursor(0);
        }
    }
}

void game_mode_load_level() {
    ppu_off();

    if (prev_mode != MODE_LOAD_LEVEL && prev_mode != MODE_EDIT && prev_mode != MODE_EXECUTE) {
        prev_mode = MODE_LOAD_LEVEL;
        draw_hud(level);
    }

    // Draw all of the WAIT symbols
    for (uint8_t i=0; i<12; i++) {
        update_command_list(Command::CMD_WAIT);
        flush_vram_update2();
    }
    update_sub_attribute();
    for (uint8_t i=0; i<9; i++) {
        update_command_list(Command::CMD_WAIT);
        flush_vram_update2();
    }
    update_sub_attribute();
    for (uint8_t i=0; i<9; i++) {
        update_command_list(Command::CMD_WAIT);
        flush_vram_update2();
    }
    update_sub_attribute();
    flush_vram_update2();

    load_level(level);
    ppu_wait_nmi();

    set_game_mode(MODE_EDIT);
}

void game_mode_edit() {
    if (prev_mode != MODE_EDIT && prev_mode != MODE_EXECUTE) {
        prev_mode = MODE_EDIT;
        ppu_on_all();
        pal_fade_to(0, 4, 4);
    }
    game_mode_edit_main();
}

void game_mode_execute() {
    // if (prev_mode != MODE_EDIT) {
    //     prev_mode = MODE_EDIT;
    //     ppu_on_all();
    //     pal_fade_to(0, 4, 4);
    // }

    // i dare you to inline this llvm-mos. bring it on.
    game_mode_execute_main();
}

int main() {
    
    // Tell NMI to update graphics using the VRAM_BUFFER provided by nesdoug library
    set_vram_buffer();
    
    // Start off by disabling the PPU rendering, allowing us to upload data safely to the nametable (background)
    ppu_off();

    // Clear all sprites off screen. RAM state is random on boot so there is a bunch of garbled sprites on screen
    // by default.
    oam_clear();

    // Set to use 8x8 sprite mode. I doubt 8x16 sprite mode will help much with the game genie CHR :)
    oam_size(0);

    // Upload a basic palette we can use later.
    pal_all(default_palette);

    pal_bright(0);
    flush_vram_update2();
    
    // Set the scroll position on the screen to 0, 0
    scroll(0, 0);

    // Force the sub index for the command slots
    command_index[1] = 12;
    command_index[2] = 12 + 9;
    
    // Turn on the screen, showing both the background and sprites
    // ppu_on_all();
    
constexpr uint8_t X_LO_BOUND = (10 * 8);
constexpr uint8_t Y_LO_BOUND = (22 * 8);
    auto cursor = cursors[0];
    cursor.is_moving = false;
    cursor.x = X_LO_BOUND;
    cursor.y = Y_LO_BOUND;
    cursor.height = 16;
    cursor.width = 16;
    cursor.param1 = 4;

    auto cmdcursor = cursors[1];
    cmdcursor.is_moving = false;
    cmdcursor.param1 = 1;
    cmdcursor.height = 16;
    cmdcursor.width = 16;

    // Now time to start the main game loop
    while (true) {
        // At the start of the frame, poll the controller to get the latest state.
        pad_poll(0);
        // Once a frame, clear the sprites out so that we don't have leftover sprites.
        oam_clear();

        switch (game_mode) {
        case MODE_RESET:
            // game_mode = MODE_LOAD_LEVEL;
            // break;
            // fallthrough
        case MODE_TITLE:
            game_mode_title();
            break;
        case MODE_LOAD_LEVEL:
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
            game_mode_load_level();
            break;
        }
        
        // All done! Wait for the next frame before looping again
        ppu_wait_nmi();
    }
    // Tell the compiler we are never stopping the game loop!
    __builtin_unreachable();
}
