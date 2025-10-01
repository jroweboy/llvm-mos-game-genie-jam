
#include <cstdint>

#include "common.hpp"
#include "game.hpp"
#include "levels.hpp"
#include "text_render.hpp"


NEXT_LEVEL(
    (LETTERS_15("CALL 1 FOR FUN")),
    
    L_PLAYER(Facing::Down, 5, 4),
    L_MANY(LevelObjType::HURT_WALL, L_HORIZONTAL, 7, 2, 1),
    L_MANY(LevelObjType::HURT_WALL, L_HORIZONTAL, 7, 2, 7),
    L_MANY(LevelObjType::HURT_WALL, L_VERTICAL, 5, 2, 2),
    L_MANY(LevelObjType::HURT_WALL, L_VERTICAL, 5, 8, 2),
    L_ONE(LevelObjType::PICKUP, 3, 2),
    L_ONE(LevelObjType::PICKUP, 7, 2),
    L_ONE(LevelObjType::PICKUP, 3, 6),
    L_ONE(LevelObjType::PICKUP, 7, 6),
#ifdef TEST_LEVEL_SOLUTION
    L_CMD_ONE(3),
    PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_PICKUP, CMD_TURN_RIGHT),
    PACK(CMD_MOVE, CMD_MOVE),
    L_CMD_MAIN(4),
    PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_TURN_RIGHT, CMD_JMP_ONE),
    PACK(CMD_JMP_ONE, CMD_JMP_ONE),
    PACK(CMD_JMP_ONE, CMD_JMP_ONE),
#else
    L_CMD_ONE(3),
    PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_PICKUP, CMD_TURN_RIGHT),
    PACK(CMD_MOVE, CMD_MOVE),
    L_CMD_MAIN(1),
    PACK(CMD_END, CMD_END),
#endif
    L_END,
);


NEXT_LEVEL(
    (LETTERS_14("ITS A FREEBIE")),
    
    L_ONE(LevelObjType::PICKUP, 5, 1),
    L_PLAYER(Facing::Up, 5, 5),
#ifdef TEST_LEVEL_SOLUTION
    L_CMD_MAIN(3),
    PACK(CMD_MOVE, CMD_MOVE), PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_PICKUP, CMD_END),
#else
    L_CMD_MAIN(3),
    PACK(CMD_MOVE, CMD_MOVE), PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_PICKUP, CMD_END),
#endif
    L_END,
);

NEXT_LEVEL(
    (LETTERS_12("ALMOST FREE")),
    
    L_ONE(LevelObjType::PICKUP, 5, 1),
    L_ONE(LevelObjType::PICKUP, 5, 2),
    L_PLAYER(Facing::Up, 5, 5),
#ifdef TEST_LEVEL_SOLUTION
    L_CMD_MAIN(3),
    PACK(CMD_MOVE, CMD_MOVE), PACK(CMD_MOVE, CMD_PICKUP),
    PACK(CMD_MOVE, CMD_PICKUP),
#else
    L_CMD_MAIN(3),
    PACK(CMD_MOVE, CMD_MOVE), PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_PICKUP, CMD_END),
#endif
    L_END,
);

NEXT_LEVEL(
    (LETTERS_12("JUST SWERVE")),
    
    L_MANY(LevelObjType::SOLID_WALL, L_HORIZONTAL, 4, 0, 3),
    L_MANY(LevelObjType::SOLID_WALL, L_HORIZONTAL, 5, 5, 3),
    L_ONE(LevelObjType::PICKUP, 5, 1),
    L_PLAYER(Facing::Up, 5, 5),
#ifdef TEST_LEVEL_SOLUTION
    L_CMD_MAIN(5),
    PACK(CMD_TURN_LEFT, CMD_MOVE),
    PACK(CMD_TURN_RIGHT, CMD_MOVE),
    PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_MOVE, CMD_TURN_RIGHT),
    PACK(CMD_MOVE, CMD_PICKUP),
#else
    L_CMD_MAIN(3),
    PACK(CMD_MOVE, CMD_MOVE), PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_PICKUP, CMD_END),
#endif
    L_END,
);


NEXT_LEVEL(
    (LETTERS_14("GRATS YOU WON")),
    L_PLAYER(Facing::Up, 5, 4),
    L_ONE(LevelObjType::PICKUP, 1, 2),
    L_ONE(LevelObjType::PICKUP, 7, 2),
    L_ONE(LevelObjType::PICKUP, 3, 8),
    L_ONE(LevelObjType::PICKUP, 5, 8),
    L_ONE(LevelObjType::PICKUP, 4, 4),
    L_MANY(LevelObjType::SOLID_WALL, L_VERTICAL, 2, 1, 3),
    L_MANY(LevelObjType::SOLID_WALL, L_VERTICAL, 2, 7, 3),
    L_MANY(LevelObjType::SOLID_WALL, L_VERTICAL, 2, 2, 5),
    L_MANY(LevelObjType::SOLID_WALL, L_VERTICAL, 2, 6, 5),
    L_MANY(LevelObjType::SOLID_WALL, L_VERTICAL, 1, 3, 7),
    L_MANY(LevelObjType::SOLID_WALL, L_VERTICAL, 1, 5, 7),
    L_MANY(LevelObjType::SOLID_WALL, L_VERTICAL, 2, 4, 5),
#ifdef TEST_LEVEL_SOLUTION
    L_CMD_ONE(5),
    PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_MOVE, CMD_PICKUP),
    PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_PICKUP, CMD_MOVE),
    PACK(CMD_TURN_RIGHT, CMD_END),
    L_CMD_TWO(5),
    PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_TURN_RIGHT, CMD_MOVE),
    PACK(CMD_MOVE, CMD_PICKUP),
    PACK(CMD_MOVE, CMD_TURN_RIGHT),
    PACK(CMD_MOVE, CMD_END),
    L_CMD_MAIN(6),
    PACK(CMD_JMP_TWO, CMD_JMP_ONE),
    PACK(CMD_JMP_ONE, CMD_TURN_LEFT),
    PACK(CMD_JMP_ONE, CMD_JMP_ONE),
    PACK(CMD_MOVE, CMD_PICKUP),
    PACK(CMD_MOVE, CMD_JMP_TWO),
    PACK(CMD_END, CMD_PICKUP),
#else
    // Maybe include CMD_TWO
#endif
    L_END,
);

// extern const uint8_t LEVEL_AMAZING[] = {
//     // L_MANY(LevelObjType::SOLID_WALL, L_HORIZONTAL, 4, 0, 3),
//     // L_MANY(LevelObjType::SOLID_WALL, L_HORIZONTAL, 5, 5, 3),
//     L_ONE(LevelObjType::PICKUP, 5, 1),
//     L_PLAYER(Facing::Up, 5, 5),
//     L_CMD_MAIN(3),
//     PACK(CMD_MOVE, CMD_MOVE), PACK(CMD_MOVE, CMD_MOVE),
//     PACK(CMD_PICKUP, CMD_END), 
//     L_END,
// };

SPLIT_ARRAY_IMPL(level_titles,
    LEVEL_TITLE_0,
    LEVEL_TITLE_1,
    LEVEL_TITLE_2,
    LEVEL_TITLE_3
);

SPLIT_ARRAY_IMPL(all_levels,
    LEVEL_DATA_0,
    LEVEL_DATA_1,
    LEVEL_DATA_2,
    LEVEL_DATA_3
);

const Letter password_alphabet[16] = {
    A, B, C, D,
    E, F, G, H,
    I, J, K, O,
    N, P, R, S
};
consteval static uint16_t generate_password(uint8_t lev) {
    unsigned seed = (0xface + lev);
    
    unsigned x = seed;
    x ^= x << 7;
    x ^= x >> 9;
    x ^= x << 8;
    seed = x;
    return seed;
}

FIXED constinit const soa::Array<uint16_t, LEVEL_COUNT> level_passwords = {
    generate_password(0),
    generate_password(1),
    generate_password(2),
    generate_password(3),
};
