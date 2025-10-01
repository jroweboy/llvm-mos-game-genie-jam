#pragma once 

#include <soa.h>
#include <stdint.h>
#include <neslib.h>
#include "common.hpp"

enum class LevelObjType : uint8_t {
    TERMINATOR = 0,
    EMPTY = 0,
    SOLID_WALL,
    TIMED_WALL,
    HURT_WALL,
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

enum class Difficulty : uint8_t {
    EASY,
    MEDIUM,
    HARD
};

void draw_hud(uint8_t level_num);
void load_level(uint8_t level_num);
void draw_pickup(Point p);

extern "C" LevelObjType load_metatile_at_coord(uint8_t x, uint8_t y);
extern "C" void update_level_buff(uint8_t tile_x, uint8_t tile_y, LevelObjType val);
extern "C" void update_speed_setting();

constexpr uint8_t LEVEL_COUNT = 12;

SPLIT_ARRAY_DEFINE(all_levels);
SPLIT_ARRAY_DEFINE(level_titles);
extern const soa::Array<uint16_t, LEVEL_COUNT> level_passwords;
extern const Difficulty level_difficulty[LEVEL_COUNT];

// SPLIT_ARRAY_DEFINE(level_passwords);

extern const Letter password_alphabet[16];

consteval static uint16_t generate_password(uint8_t lev) {
    unsigned seed = (0xface + lev);
    
    unsigned x = seed;
    x ^= x << 7;
    x ^= x >> 9;
    x ^= x << 8;
    seed = x;
    return seed;
}

constexpr uint8_t L_MULTIPLE = 1 << 7;
constexpr uint8_t L_HORIZONTAL = 0;
constexpr uint8_t L_VERTICAL = 1 << 7;
constexpr uint8_t L_FACING_UP = 0b00 << 6;
constexpr uint8_t L_FACING_RIGHT = 0b01 << 6;
constexpr uint8_t L_FACING_DOWN = 0b10 << 6;
constexpr uint8_t L_FACING_LEFT = 0b11 << 6;

#define L_POS(x, y) static_cast<uint8_t>(((x) & 0xf) << 4 | ((y) & 0xf))
#define L_FACE(id, facing) static_cast<uint8_t>(static_cast<uint8_t>((id)) | static_cast<uint8_t>(facing) << 6)

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

#define L_CMD_MAIN(len) (static_cast<uint8_t>(LevelObjType::CMD_MAIN)), (len)
#define L_CMD_ONE(len) (static_cast<uint8_t>(LevelObjType::CMD_ONE)), (len)
#define L_CMD_TWO(len) (static_cast<uint8_t>(LevelObjType::CMD_TWO)), (len)

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

#define A_TL(atr) ((atr) << 0)
#define A_TR(atr) ((atr) << 2)
#define A_BL(atr) ((atr) << 4)
#define A_BR(atr) ((atr) << 6)


#define UNPAREN(...) __VA_ARGS__
#define CONCAT_IMPL(x, y) x ## y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define LEVEL_IMPL(c, name, diff, ...) \
    CONCAT(extern const Letter LEVEL_TITLE_, c)[] = { UNPAREN name }; \
    CONCAT(constexpr Difficulty LEVEL_DIFFICULTY_, c) = (diff); \
    CONCAT(extern const uint8_t LEVEL_DATA_, c)[] = { __VA_ARGS__ }; \
    CONCAT(constexpr uint16_t LEVEL_PASSWORD_, c) = generate_password(c);

#define NEXT_LEVEL(name, diff, ...) \
    LEVEL_IMPL(__COUNTER__, name, diff, __VA_ARGS__)

// Generate a list of numbers of N-1
#define GENERATE_1() 0
#define GENERATE_2() GENERATE_1(), 1
#define GENERATE_3() GENERATE_2(), 2
#define GENERATE_4() GENERATE_3(), 3
#define GENERATE_5() GENERATE_4(), 4
#define GENERATE_6() GENERATE_5(), 5
#define GENERATE_7() GENERATE_6(), 6
#define GENERATE_8() GENERATE_7(), 7
#define GENERATE_9() GENERATE_8(), 8
#define GENERATE_10() GENERATE_9(), 9
#define GENERATE_11() GENERATE_10(), 10
#define GENERATE_12() GENERATE_11(), 11
#define GENERATE_13() GENERATE_12(), 12
#define GENERATE_14() GENERATE_13(), 13
#define GENERATE_15() GENERATE_14(), 14
#define GENERATE_16() GENERATE_15(), 15
#define GENERATE_17() GENERATE_16(), 16
#define GENERATE_18() GENERATE_17(), 17
#define GENERATE_19() GENERATE_18(), 18
#define GENERATE_20() GENERATE_19(), 19
#define GENERATE_21() GENERATE_20(), 20

#define GENERATE_N_1(n) CONCAT_IMPL(GENERATE_, n)()
