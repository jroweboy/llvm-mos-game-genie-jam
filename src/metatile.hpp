#pragma once

#include <soa.h>

#include "common.hpp"

#ifdef __cplusplus
extern "C" {
#endif

// Bitpacks a 2x1 strip of tiles into a single byte
#define LEFT_TILE(x) (((x) >> 4) & 0xf)
#define RIGHT_TILE(x) ((x) & 0xf)

#define MTILE_TL(x) ((uint8_t)LEFT_TILE(metatiles[(x)]->top))
#define MTILE_TR(x) ((uint8_t)RIGHT_TILE(metatiles[(x)]->top))
#define MTILE_BL(x) ((uint8_t)LEFT_TILE(metatiles[(x)]->bot))
#define MTILE_BR(x) ((uint8_t)RIGHT_TILE(metatiles[(x)]->bot))

union Attribute {
    uint8_t raw;
    struct {
        uint8_t tl : 2;
        uint8_t tr : 2;
        uint8_t bl : 2;
        uint8_t br : 2;
    };
};

struct Metatile_2_2 {
    uint8_t top;
    uint8_t bot;
};

struct Metatile_2_3 {
    uint8_t top_top;
    uint8_t top_bot;
    uint8_t bot_top;
};

struct Metatile_4_4 {
    Metatile_2_2 topleft;
    Metatile_2_2 botleft;
    Metatile_2_2 topright;
    Metatile_2_2 botright;
};


extern const soa::Array<Metatile_2_2, Metatile::METATILE_COUNT> metatiles;
extern uint8_t attribute_buffer[0x40];
void write_attribute(uint8_t tile_x, uint8_t tile_y, uint8_t attr);
void update_attribute(uint8_t tile_x, uint8_t tile_y, uint8_t attr);

/**
 * @brief Metatile draw routines - buffers a draw to the VRAM_BUF for metatiles.
 *
 * @param nmt - Which Nametable to draw the metatile in
 * @param x   - X coord between 0 - 31
 * @param y   - Y coord between 0 - 29
 * @param tile - Pointer to the metatile to draw
 */
void draw_metatile_2_2(Nametable nmt, uint8_t x, uint8_t y, Metatile idx);
void draw_metatile_2_3(Nametable nmt, uint8_t x, uint8_t y, Letter idx);
void draw_metatile_4_4(Nametable nmt, uint8_t x, uint8_t y, uint8_t idx);

consteval uint8_t get_tile_for_bits(uint8_t bits) {
    const uint8_t bits_to_tile[] = {
        // order for the bits is tl tr bl br
        0x0, // 0b0000
        0x2, // 0b0001
        0x1, // 0b0010
        0x3, // 0b0011
        0x8, // 0b0100
        0xa, // 0b0101
        0x9, // 0b0110
        0xb, // 0b0111
        0x4, // 0b1000
        0x6, // 0b1001
        0x5, // 0b1010
        0x7, // 0b1011
        0xc, // 0b1100
        0xe, // 0b1101
        0xd, // 0b1110
        0xf, // 0b1111
    };
    return bits_to_tile[bits];
}

consteval uint8_t combine_bits_to_tile(uint64_t bits, uint8_t offset) {
    constexpr uint8_t stride = 8;
    return (uint8_t)((get_tile_for_bits((((bits >> (offset + 2 + stride)) & 0x3) << 2) | ((bits >> (offset + 2)) & 0x3)) << 4)
                    | get_tile_for_bits((((bits >> (offset + stride)) & 0x3) << 2) | ((bits >> (offset)) & 0x3)));
}

consteval Metatile_4_4 parse_string_mt_4_4(const char* text, size_t len) {
    // loop through the letters of the provided string looking for any special characters.
    // Replace a space character with a since bit 0 and any other non-ignored character with a bit 1
    // I'm sure I could've done this in a better way but this works well enough for now.
    uint64_t bits = 0;
    int i = 0;
    // Require all tiles passed into mt to have at least 4px per row and 8 px per column
    char c = text[i];
    for (int row=0; row < 8; row++) {
        // skip anything else until the next pipe
        while (c != '|' && i < len) c = text[i++];
        // skip over the left |
        if (c == '|') c = text[i++];
        // then read 8 px (or until the next pipe or newline)
        for (int col=0; col < 8; col++) {
            // Any non-"blank" character will use color 3, and any "blank" or missing characters will use 
            uint8_t value = (c == ' ' || c == '|' || c == '\n') ? 0 : 1;
            bits <<= 1;
            bits |= value;
            if (c != '|' && c != '\n')
                c = text[i++];
        }
        // skip to the next line
        while (c != '\n' && i < len) c = text[i++];
    }
    // Screaming internally.
    Metatile_2_2 topleft {
        .top = combine_bits_to_tile(bits, 48 + 4),
        .bot = combine_bits_to_tile(bits, 32 + 4),
    };
    Metatile_2_2 topright {
        .top = combine_bits_to_tile(bits, 48),
        .bot = combine_bits_to_tile(bits, 32),
    };
    Metatile_2_2 botleft {
        .top = combine_bits_to_tile(bits, 16 + 4),
        .bot = combine_bits_to_tile(bits, 0 + 4),
    };
    Metatile_2_2 botright {
        .top = combine_bits_to_tile(bits, 16),
        .bot = combine_bits_to_tile(bits, 0),
    };

    return {
        .topleft = topleft,
        .topright = topright,
        .botleft = botleft,
        .botright = botright,
    };
}
consteval Metatile_2_2 parse_string_mt_2_2(const char* text, size_t len) {
    Metatile_4_4 mt = parse_string_mt_4_4(text, len);
    return {
        .top = mt.topleft.top,
        .bot = mt.topleft.bot,
    };
}

consteval Metatile_2_3 parse_string_mt_2_3(const char* text, size_t len) {
    Metatile_4_4 mt = parse_string_mt_4_4(text, len);
    return {
        .top_top = mt.topleft.top,
        .top_bot = mt.topleft.bot,
        .bot_top = mt.botleft.top,
    };
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include <soa.h>

#define SOA_STRUCT Metatile_2_2
#define SOA_MEMBERS MEMBER(top) MEMBER(bot)
#include <soa-struct.inc>

#define SOA_STRUCT Metatile_2_3
#define SOA_MEMBERS MEMBER(top_top) MEMBER(top_bot) MEMBER(bot_top)
#include <soa-struct.inc>

#define SOA_STRUCT Metatile_4_4
#define SOA_MEMBERS MEMBER(topleft) MEMBER(botleft) MEMBER(topright) MEMBER(botright)
#include <soa-struct.inc>

#include <cstdint>
#include <cstddef>

consteval Metatile_2_2 operator""_mt_2_2(const char* text, size_t len) {
    return parse_string_mt_2_2(text, len);
}
consteval Metatile_2_3 operator""_mt_2_3(const char* text, size_t len) {
    return parse_string_mt_2_3(text, len);
}
consteval Metatile_4_4 operator""_mt_4_4(const char* text, size_t len) {
    return parse_string_mt_4_4(text, len);
}

#endif