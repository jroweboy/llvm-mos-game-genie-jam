
#include "common.hpp"

#include "levels.hpp"

const uint8_t* all_levels[] = {
    LEVEL_0,
    LEVEL_1,
};

const uint8_t LEVEL_0[] = {
    C_MANY(LevelObjType::SOLID_WALL, C_VERTICAL, 4, 3, 0),
    C_MANY(LevelObjType::TIMED_WALL, C_VERTICAL, 4, 3, 5),
    C_FLUSH(),
    C_ONE(LevelObjType::PICKUP, 1, 6),
    C_PLAYER(Facing::Left, 5, 3),
    C_END(),
};
