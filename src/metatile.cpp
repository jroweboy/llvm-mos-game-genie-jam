
#include <cstdint>

#include <nesdoug.h>
#include <neslib.h>

#include "text_render.hpp"
#include "graphics.hpp"
#include "metatile.hpp"



__attribute__((section(".prg_rom_fixed")))
static const constexpr soa::Array<Metatile_2_3, Letter::COUNT> all_letters = {
    #include "font.inc"
};

// Include the VRAM buffer and the VRAM_INDEX so we can write directly into the buffer ourselves.
extern volatile uint8_t VRAM_BUF[128];
extern volatile __zeropage uint8_t VRAM_INDEX;
extern volatile __zeropage uint8_t NAME_UPD_ENABLE;


extern "C" void draw_metatile_2_2(Nametable nmt, uint8_t x, uint8_t y, Metatile mtile_idx) {
    auto tile = metatiles[static_cast<uint8_t>(mtile_idx)];
    auto idx = VRAM_INDEX;
    int ppuaddr_left = 0x2000 | (((uint8_t)nmt) << 8) | (((y) << 5) | (x));
    int ppuaddr_right = ppuaddr_left + 1;
    VRAM_BUF[idx+ 0] = MSB(ppuaddr_left) | NT_UPD_VERT;
    VRAM_BUF[idx+ 1] = LSB(ppuaddr_left);
    VRAM_BUF[idx+ 5] = MSB(ppuaddr_right) | NT_UPD_VERT;
    VRAM_BUF[idx+ 6] = LSB(ppuaddr_right);
    VRAM_BUF[idx+ 2] = 2;
    VRAM_BUF[idx+ 7] = 2;
    VRAM_BUF[idx+ 3] = LEFT_TILE(tile->top);
    VRAM_BUF[idx+ 8] = RIGHT_TILE(tile->top);
    VRAM_BUF[idx+ 4] = LEFT_TILE(tile->bot);
    VRAM_BUF[idx+ 9] = RIGHT_TILE(tile->bot);
    VRAM_BUF[idx+10] = 0xff; // terminator bit
    VRAM_INDEX += 10;
}

extern "C" void draw_metatile_2_3(Nametable nmt, uint8_t x, uint8_t y, Letter mtile_idx) {
    auto tile = all_letters[mtile_idx];
    auto idx = VRAM_INDEX;
    int ppuaddr_left = 0x2000 | (((uint8_t)nmt) << 8) | (((y) << 5) | (x));
    int ppuaddr_right = ppuaddr_left + 1;
    VRAM_BUF[idx+ 0] = MSB(ppuaddr_left) | NT_UPD_VERT;
    VRAM_BUF[idx+ 1] = LSB(ppuaddr_left);
    VRAM_BUF[idx+ 6] = MSB(ppuaddr_right) | NT_UPD_VERT;
    VRAM_BUF[idx+ 7] = LSB(ppuaddr_right);
    VRAM_BUF[idx+ 2] = 3;
    VRAM_BUF[idx+ 8] = 3;
    VRAM_BUF[idx+ 3] = LEFT_TILE(tile->top_top);
    VRAM_BUF[idx+ 9] = RIGHT_TILE(tile->top_top);
    VRAM_BUF[idx+ 4] = LEFT_TILE(tile->top_bot);
    VRAM_BUF[idx+10] = RIGHT_TILE(tile->top_bot);
    VRAM_BUF[idx+ 5] = LEFT_TILE(tile->bot_top);
    VRAM_BUF[idx+11] = RIGHT_TILE(tile->bot_top);
    VRAM_BUF[idx+12] = 0xff; // terminator bit
    VRAM_INDEX += 12;
}

// extern "C" void draw_metatile_4_4(Nametable nmt, uint8_t x, uint8_t y, uint8_t idx) {
//     draw_metatile_2_2(nmt, x, y, &tile->topleft);
//     draw_metatile_2_2(nmt, x+2, y, &tile->topright);
//     draw_metatile_2_2(nmt, x, y+2, &tile->botleft);
//     draw_metatile_2_2(nmt, x+2, y+2, &tile->botright);
// }
