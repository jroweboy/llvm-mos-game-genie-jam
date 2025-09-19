
// Used for standard int size defines
#include <cstdint>

// Common C NES libary that includes a simple NMI update routine
#include <neslib.h>

// Add-ons to the neslib, bringing metatile support and more
#include <nesdoug.h>

// Include our own player update function for the movable sprite.
#include "player.hpp"
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
    0x0f, 0x0f, 0x0f, 0x0f,
    0x0f, 0x0f, 0x0f, 0x0f,
    0x0f, 0x0f, 0x0f, 0x0f,
};

// Start the scroll position at 0
static uint8_t scroll_y = 0;
static int8_t direction = 1;
static uint8_t scroll_frame_count = 0;
static bool show_left_nametable = true;

// Simple view with just some text rendered to it for demonstration.
void update_text_view() {
    set_scroll_x(0x00);
    set_scroll_y(0x00);
}

// Background showing how to change the scroll every frame or so
void update_scrolling_view() {
    // Update the scroll each frame to bounce the screen
    scroll_frame_count = (scroll_frame_count + 1) & 0x1f;

    // every 32 frames, switch which direction to move the words
    direction = scroll_frame_count != 0 ? direction : -direction;
    scroll_y += direction;
    // Be careful when scrolling in the Y direction!
    // Setting the y scroll between 240-255 will try to render the attribute table.
    // So make sure if you are setting the scroll that you skip over this.
    // The nesdoug libary has a add_scroll_y/sub_scroll_y helper method for skipping those.
    // But i'm not using them here.
    if (scroll_y >= 240) {
        // if we are going up into 240-255, then skip to 0
        // else we are going down into 255, so skip to 239
        scroll_y = direction > 0 ? 0 : 239;
    }
    set_scroll_x(0x100);
    set_scroll_y(scroll_y);
}

// Handles checking if select was just pressed this frame and switches game modes
void update_view() {
    // Using `pad_new` lets us check only which buttons are newly pressed this frame.
    auto input = get_pad_new(0);
    // If we just pushed select, then switch to the other "game mode"
    if (input & PAD_SELECT) {
        show_left_nametable = !show_left_nametable;
    }
    if (show_left_nametable) {
        update_text_view();
    } else {
        update_scrolling_view();
    }
}

// We defined this data in ca65 as an example of how to reference labels defined in asm in C
extern const uint8_t example_ca65_data[];
// This is a zeropage variable defined in ca65
extern uint8_t __zeropage var_defined_in_ca65;


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
    
    // And then clear out the other nametable as well.
    // vram_adr(NAMETABLE_B);
    // Write a RLE compressed nametable to the screen. You can generate NESLIB compatible RLE
    // compressed nametables with a tool like NEXXT
    // vram_unrle(nametable);

    // Example string rendering using the custom string conversion.
    // The `_l` is a user defined literal, which converts from ASCII to our custom character map at compile time
    // ie: in the generated code "THE QUICK" will be compiled as {Letter::T, Letter::H, ... Letter::NUL}
    // render_string(Nametable::A, 1, 1, "THE QUICK"_l);
    // render_string(Nametable::A, 1, 4, "BROWN FOX"_l);
    // render_string(Nametable::A, 1, 7, "JUMPS OVER"_l);
    // render_string(Nametable::A, 1, 10, "THE LAZY DOG"_l);
    // render_string(Nametable::A, 1, 14, "PUSH SELECT"_l);
    // render_string(Nametable::A, 1, 17, "TO SWITCH VIEW"_l);
    draw_level(0);

    // Set the scroll position on the screen to 0, 0
    scroll(0, 0);
    
    // Turn on the screen, showing both the background and sprites
    ppu_on_all();

    // Now time to start the main game loop
    while (true) {
        // At the start of the frame, poll the controller to get the latest state.
        pad_poll(0);
        // Once a frame, clear the sprites out so that we don't have leftover sprites.
        oam_clear();

        // Run the main code for processing our backgrounds.
        // NOTE: llvm-mos is very aggressive at inlining, so don't stress the small things
        // like function call overhead too much.
        // update_view();

        // update_player_position();
        
        // All done! Wait for the next frame before looping again
        ppu_wait_frame();
    }
    // Tell the compiler we are never stopping the game loop!
    __builtin_unreachable();
}
