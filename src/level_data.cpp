
#include <cstdint>

#include "common.hpp"
#include "game.hpp"
#include "levels.hpp"


FIXED const Letter* LEVEL_TITLE_FREEBIE = "FREEBIE"_l;
FIXED const Letter* LEVEL_TITLE_ZIGZAG = "ZIG ZAG DRAG"_l;

SPLIT_ARRAY_IMPL(level_titles,
    LEVEL_TITLE_FREEBIE,
    LEVEL_TITLE_ZIGZAG
);

SPLIT_ARRAY_IMPL(all_levels,
    LEVEL_FREEBIE,
    LEVEL_ZIGZAG
);

const uint8_t LEVEL_FREEBIE[] = {
    L_ONE(LevelObjType::PICKUP, 5, 1),
    L_PLAYER(Facing::Up, 5, 5),
    L_CMD_MAIN(3),
    PACK(CMD_MOVE, CMD_MOVE), PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_PICKUP, CMD_END), 
    L_END,
};

const uint8_t LEVEL_ZIGZAG[] = {
    L_MANY(LevelObjType::SOLID_WALL, L_HORIZONTAL, 4, 0, 3),
    L_MANY(LevelObjType::SOLID_WALL, L_HORIZONTAL, 5, 5, 3),
    L_ONE(LevelObjType::PICKUP, 5, 1),
    L_PLAYER(Facing::Up, 5, 5),
    L_CMD_MAIN(3),
    PACK(CMD_MOVE, CMD_MOVE), PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_PICKUP, CMD_END), 
    L_END,
};
