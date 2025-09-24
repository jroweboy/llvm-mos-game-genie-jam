
#include "common.hpp"
#include "game.hpp"
#include "levels.hpp"

SPLIT_ARRAY_IMPL(all_levels, LEVEL_0);

const uint8_t LEVEL_0[] = {
    L_MANY(LevelObjType::SOLID_WALL, L_HORIZONTAL, 4, 0, 3),
    L_MANY(LevelObjType::SOLID_WALL, L_HORIZONTAL, 5, 5, 3),
    // L_MANY(LevelObjType::TIMED_WALL, L_VERTICAL, 4, 3, 5),
    L_ONE(LevelObjType::PICKUP, 5, 1),
    L_PLAYER(Facing::Up, 5, 5),
    L_CMD_MAIN(4),
    PACK(CMD_MOVE, CMD_MOVE), PACK(CMD_TURN_LEFT, CMD_MOVE),
    PACK(CMD_TURN_RIGHT, CMD_MOVE), PACK(CMD_MOVE, CMD_END),
    L_END,
};
