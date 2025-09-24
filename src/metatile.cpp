
#include <cstdint>

#include <nesdoug.h>
#include <neslib.h>

#include "metatile.hpp"

__attribute__((section(".prg_rom_fixed")))
static const constexpr soa::Array<Metatile_2_3, Letter::COUNT> all_letters = {
    #include "font.inc"
};


__attribute__((section(".prg_rom_fixed")))
constexpr soa::Array<Metatile_2_2, METATILE_COUNT> metatiles = {
    #include "metatile_graphics.inc"
};

// Include the VRAM buffer and the VRAM_INDEX so we can write directly into the buffer ourselves.
extern volatile uint8_t VRAM_BUF[128];
extern volatile __zeropage uint8_t VRAM_INDEX;
extern volatile __zeropage uint8_t NAME_UPD_ENABLE;


uint8_t attribute_buffer[0x40];

constexpr uint8_t get_attr_idx(uint8_t tile_x, uint8_t tile_y) {
    int attribute_block_x = tile_x / 4;
    int attribute_block_y = tile_y / 4;
    return (attribute_block_y * 8) + attribute_block_x;
}

void buffer_attribute_update(uint8_t index) {
    int ppuaddr = (0x2300) | (uint8_t)((0xc0) + index);
    auto idx = VRAM_INDEX;
    VRAM_BUF[idx+ 0] = MSB(ppuaddr);
    VRAM_BUF[idx+ 1] = LSB(ppuaddr);
    VRAM_BUF[idx+ 2] = attribute_buffer[index];
    VRAM_BUF[idx+ 3] = 0xff;
    VRAM_INDEX += 3;
}

constinit uint8_t attr_shift_lut[16] = {
    // tl
    0b00 << 0, 0b01 << 0, 0b10 << 0, 0b11 << 0,
    // tr
    0b00 << 2, 0b01 << 2, 0b10 << 2, 0b11 << 2,
    // bl
    0b00 << 4, 0b01 << 4, 0b10 << 4, 0b11 << 4,
    // br
    0b00 << 6, 0b01 << 6, 0b10 << 6, 0b11 << 6
};
constinit uint8_t attr_bit_lut[4] = {
    (uint8_t)(0b11 << 0),
    (uint8_t)(0b11 << 2),
    (uint8_t)(0b11 << 4),
    (uint8_t)(0b11 << 6),
};
constinit uint8_t attr_mask_lut[4] = {
    (uint8_t)~(0b11 << 0),
    (uint8_t)~(0b11 << 2),
    (uint8_t)~(0b11 << 4),
    (uint8_t)~(0b11 << 6),
};

void update_attribute(uint8_t tile_x, uint8_t tile_y, uint8_t attr) {
    auto idx = get_attr_idx(tile_x, tile_y);
    auto old = attribute_buffer[idx];
    uint8_t quadrant = (uint8_t)((tile_y & 2)) | ((tile_x & 2) >> 1);
    uint8_t shifted_value = attr_shift_lut[quadrant << 2 | attr];
    uint8_t orig_shifted_value = old & attr_bit_lut[quadrant];
    bool update_buffer = false;
    if (orig_shifted_value != shifted_value) {
        attribute_buffer[idx] = (attribute_buffer[idx] & attr_mask_lut[quadrant]) | shifted_value;
        update_buffer = true;
    }
    if (update_buffer) {
        buffer_attribute_update(idx);
    }
}

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
