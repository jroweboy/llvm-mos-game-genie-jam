
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
    0x0f, 0x0f, 0x0f, 0x30,
    0x0f, 0x0f, 0x0f, 0x22,
    0x0f, 0x0f, 0x0f, 0x0f,
    0x0f, 0x0f, 0x0f, 0x0f,
};

GameMode prev_mode;
GameMode game_mode;
uint8_t level;

void set_game_mode(GameMode mode) {
    prev_mode = game_mode;
    game_mode = mode;
}

// Start the scroll position at 0
// static uint8_t scroll_y = 0;
// static int8_t direction = 1;
// static uint8_t scroll_frame_count = 0;
// static bool show_left_nametable = true;

void game_mode_title() {
    if (prev_mode != MODE_TITLE) {
        prev_mode = MODE_TITLE;
        // Initialize title screen
        render_string(Nametable::A, 3, 6, "NES CODER"_l);
        render_string(Nametable::A, 3, 9, "BY JROWEBOY"_l);
        render_string(Nametable::A, 3, 14, "PRESS DOWN"_l);
        render_string(Nametable::A, 3, 17, "FOR MANUAL"_l);
        flush_vram_update2();
        ppu_wait_nmi();
        ppu_on_all();
        pal_fade_to(0, 4, 4);
        return;
    }

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
    
}

void game_mode_load_level() {
    ppu_off();
    if (prev_mode != MODE_LOAD_LEVEL && prev_mode != MODE_EDIT && prev_mode != MODE_EXECUTE) {
        prev_mode = MODE_LOAD_LEVEL;
        draw_hud(level);
    }
    load_level(level);
    flush_vram_update2();

    // start with sub 0 by incrementing it from sub 2
    current_sub = SELECT_TWO;
    update_sub_attribute();

    ppu_wait_nmi();

    set_game_mode(MODE_EDIT);
}

void game_mode_edit() {
    if (prev_mode != MODE_EDIT) {
        prev_mode = MODE_EDIT;
        ppu_on_all();
        pal_fade_to(0, 4, 4);
    }
    game_mode_edit_main();
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

    pal_bright(4);
    flush_vram_update2();
    
    // Set the scroll position on the screen to 0, 0
    scroll(0, 0);
    
    // Turn on the screen, showing both the background and sprites
    // ppu_on_all();

    // draw_level(0);

    // Now time to start the main game loop
    while (true) {
        // At the start of the frame, poll the controller to get the latest state.
        pad_poll(0);
        // Once a frame, clear the sprites out so that we don't have leftover sprites.
        oam_clear();

        switch (game_mode) {
        case MODE_RESET:
            game_mode = MODE_LOAD_LEVEL;
            break;
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
            break;
        case MODE_PASSWORD:
            break;
        }
        
        // All done! Wait for the next frame before looping again
        ppu_wait_nmi();
    }
    // Tell the compiler we are never stopping the game loop!
    __builtin_unreachable();
}
