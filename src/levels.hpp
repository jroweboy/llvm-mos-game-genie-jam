#pragma once 

#include <stdint.h>
#include <neslib.h>
#include "common.hpp"

enum class LevelObjType : uint8_t {
    TERMINATOR,
    FLUSH_VRAM,
    SOLID_WALL,
    TIMED_WALL,
    PICKUP,
    ENEMY,
    PLAYER,
    CMD_MAIN,
    CMD_ONE,
    CMD_TWO,
};

enum class Facing : uint8_t {
    Up,
    Right,
    Down,
    Left
};

// union LevelObjId {
//     uint8_t raw;
//     struct {
//         uint8_t id : 6;
//         uint8_t multiple : 4;
//     };
// };

// union LevelObjLenDir {
//     uint8_t raw;
//     struct {
//         uint8_t len : 6;
//         uint8_t dir : 2;
//     };
// };

void draw_hud(uint8_t level_num);
void load_level(uint8_t level_num);

constexpr uint8_t L_MULTIPLE = 1 << 7;
constexpr uint8_t L_HORIZONTAL = 0;
constexpr uint8_t L_VERTICAL = 1 << 7;
constexpr uint8_t L_FACING_UP = 0b00 << 6;
constexpr uint8_t L_FACING_RIGHT = 0b01 << 6;
constexpr uint8_t L_FACING_DOWN = 0b10 << 6;
constexpr uint8_t L_FACING_LEFT = 0b11 << 6;

#define L_POS(x, y) static_cast<uint8_t>(((x) & 0xf) << 4 | ((y) & 0xf))
#define L_FACE(id, facing) static_cast<uint8_t>(static_cast<uint8_t>((id)) | static_cast<uint8_t>(facing))

#define L_ONE(id, x, y)                              \
    static_cast<uint8_t>(static_cast<uint8_t>((id))),\
    L_POS(x, y)

#define L_MANY(id, direction, len, x, y)                               \
        static_cast<uint8_t>(static_cast<uint8_t>((id)) | L_MULTIPLE), \
        L_POS(x, y),                                                   \
        static_cast<uint8_t>((len) | static_cast<uint8_t>(direction))

#define L_SPAWN_ENEMY(facing, x, y) \
    L_FACE(LevelObjType::ENEMY, facing), \
    L_POS(x, y)

#define L_PLAYER(facing, x, y) \
    L_FACE(LevelObjType::PLAYER, facing), \
    L_POS(x, y)

#define L_FLUSH() static_cast<uint8_t>(LevelObjType::FLUSH_VRAM)

#define L_CMD_MAIN(len) (static_cast<uint8_t>(LevelObjType::CMD_MAIN) + 0), (len)
#define L_CMD_ONE(len) (static_cast<uint8_t>(LevelObjType::CMD_ONE) + 12), (len)
#define L_CMD_TWO(len) (static_cast<uint8_t>(LevelObjType::CMD_TWO) + 12 + 9), (len)

#define L_END static_cast<uint8_t>(LevelObjType::TERMINATOR)

#define NOP(...) __VA_ARGS__
#define M_TOP_HZ(tile) MTILE_TL(tile), MTILE_TR(tile)
#define M_BOT_HZ(tile) MTILE_BL(tile), MTILE_BR(tile)

#define MANY_HEADER(len, dir, x, y) \
    MSB((NAMETABLE_A) | (((y) << 5) | (x))) | (dir), \
    LSB((NAMETABLE_A) | (((y) << 5) | (x))) \

#define A_MANY_HEADER(len, dir, x, y) \
    MSB((NAMETABLE_A | 0x3c0) + (((y / 4) * 8) | (x / 4))) | (dir), \
    LSB((NAMETABLE_A | 0x3c0) + (((y / 4) * 8) | (x / 4))), \
    (len)

#define T_MANY_HEADER(len, dir, x, y) \
    MANY_HEADER(len, dir, x, y), \
    (len)

#define M_MANY_HEADER(len, dir, x, y) \
    MANY_HEADER(len, dir, x, y), \
    ((len)*2)


// Draw a horizontal strip of metatiles
#define M_HORZ(len, x, y, ...) \
    M_MANY_HEADER(len, NT_UPD_HORZ, x, y), \
    FOR_EACH(M_TOP_HZ, __VA_ARGS__), \
    M_MANY_HEADER(len, NT_UPD_HORZ, x, ((y)+1)), \
    FOR_EACH(M_BOT_HZ, __VA_ARGS__) \

// Draw a single metatile
#define M_ONE(x, y, tile) \
    M_HORZ(1, x, y, tile)

// Draw a single tile
#define T_ONE(x, y, tile) \
    MSB((NAMETABLE_A) | (((y) << 5) | (x))), \
    LSB((NAMETABLE_A) | (((y) << 5) | (x))), \
    (tile)

// Draw a horizontal strip of tiles
#define T_HORZ(len, x, y, ...) \
    T_MANY_HEADER(len, NT_UPD_HORZ, x, y) \
    FOR_EACH(NOP, __VA_ARGS__)

// Draw a horizontal strip of tiles, repeating a single tile
#define T_HORZ_REPT(len, x, y, tile) \
    T_MANY_HEADER((len) | NT_UPD_REPT, NT_UPD_HORZ, x, y), \
    (tile)

// Draw a vertical strip of tiles, repeating a single tile
#define T_VERT_REPT(len, x, y, tile) \
    T_MANY_HEADER((len) | NT_UPD_REPT, NT_UPD_VERT, x, y), \
    (tile)

#define A_HORZ(len, x, y, ...) \
    A_MANY_HEADER((len), NT_UPD_HORZ, x, y), \
    FOR_EACH(NOP, __VA_ARGS__)

SPLIT_ARRAY_DEFINE(all_levels);

extern const uint8_t LEVEL_0[];
extern const uint8_t LEVEL_1[];
