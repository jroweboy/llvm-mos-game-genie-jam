#pragma once 

#include <stdint.h>


enum class LevelObjType : uint8_t {
    TERMINATOR,
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

void draw_level(uint8_t level_num);

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

#define C_END() static_cast<uint8_t>(LevelObjType::TERMINATOR)

extern const uint8_t LEVEL_0[];
extern const uint8_t LEVEL_1[];
