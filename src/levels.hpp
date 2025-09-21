#pragma once 

#include <stdint.h>
#include <neslib.h>

enum class LevelObjType : uint8_t {
    TERMINATOR,
    FLUSH_VRAM,
    SOLID_WALL,
    TIMED_WALL,
    PICKUP,
    ENEMY,
    PLAYER,
};

enum class Facing : uint8_t {
    Up,
    Right,
    Down,
    Left
};

union LevelObjId {
    uint8_t raw;
    struct {
        uint8_t id : 7;
        uint8_t multiple : 1;
    };
};

union LevelObjLenDir {
    uint8_t raw;
    struct {
        uint8_t len : 7;
        uint8_t dir : 1;
    };
};

void draw_hud(uint8_t level_num);
void load_level(uint8_t level_num);

constexpr uint8_t C_MULTIPLE = 1 << 7;
constexpr uint8_t C_HORIZONTAL = 0;
constexpr uint8_t C_VERTICAL = 1 << 7;
constexpr uint8_t C_FACING_UP = 0b00 << 6;
constexpr uint8_t C_FACING_RIGHT = 0b01 << 6;
constexpr uint8_t C_FACING_DOWN = 0b10 << 6;
constexpr uint8_t C_FACING_LEFT = 0b11 << 6;

#define C_POS(x, y) static_cast<uint8_t>(((x) & 0xf) << 4 | ((y) & 0xf))
#define C_FACE(id, facing) static_cast<uint8_t>(static_cast<uint8_t>((id)) | static_cast<uint8_t>(facing))

#define C_ONE(id, x, y)                              \
    static_cast<uint8_t>(static_cast<uint8_t>((id))),\
    C_POS(x, y)

#define C_MANY(id, direction, len, x, y)                               \
        static_cast<uint8_t>(static_cast<uint8_t>((id)) | C_MULTIPLE), \
        C_POS(x, y),                                                   \
        static_cast<uint8_t>((len) | static_cast<uint8_t>(direction))

#define C_SPAWN_ENEMY(facing, x, y) \
    C_FACE(LevelObjType::ENEMY, facing), \
    C_POS(x, y)

#define C_PLAYER(facing, x, y) \
    C_FACE(LevelObjType::PLAYER, facing), \
    C_POS(x, y)

#define C_FLUSH() static_cast<uint8_t>(LevelObjType::FLUSH_VRAM)

#define C_END() static_cast<uint8_t>(LevelObjType::TERMINATOR)

#define APPLY_1(macro, a) macro(a)
#define APPLY_2(macro, a, ...) macro(a), APPLY_1(macro, __VA_ARGS__)
#define APPLY_3(macro, a, ...) macro(a), APPLY_2(macro, __VA_ARGS__)
#define APPLY_4(macro, a, ...) macro(a), APPLY_3(macro, __VA_ARGS__)
#define APPLY_5(macro, a, ...) macro(a), APPLY_4(macro, __VA_ARGS__)

#define GET_APPLY_MACRO(_1, _2, _3, _4, _5, NAME, ...) NAME

#define FOR_EACH(macro, ...) \
    GET_APPLY_MACRO(__VA_ARGS__, APPLY_5, APPLY_4, APPLY_3, APPLY_2, APPLY_1)(macro, __VA_ARGS__)




#define NOP
#define M_TOP_HZ(tile) MTILE_TL(tile), MTILE_TR(tile)
#define M_BOT_HZ(tile) MTILE_BL(tile), MTILE_BR(tile)

#define MANY_HEADER(len, dir, x, y) \
    MSB((NAMETABLE_A) | (((y) << 5) | (x))) | (dir), \
    LSB((NAMETABLE_A) | (((y) << 5) | (x))) \

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
    FOR_EACH(NOP, __VAR_ARGS__)

// Draw a horizontal strip of tiles, repeating a single tile
#define T_HORZ_REPT(len, x, y, tile) \
    T_MANY_HEADER((len) | NT_UPD_REPT, NT_UPD_HORZ, x, y), \
    (tile)

// Draw a vertical strip of tiles, repeating a single tile
#define T_VERT_REPT(len, x, y, tile) \
    T_MANY_HEADER((len) | NT_UPD_REPT, NT_UPD_VERT, x, y), \
    (tile)


extern const uint8_t* all_levels[];

extern const uint8_t LEVEL_0[];
extern const uint8_t LEVEL_1[];
