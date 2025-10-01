
#include <cstdint>

#include "common.hpp"
#include "game.hpp"
#include "levels.hpp"
#include "text_render.hpp"


// NEXT_LEVEL(
//     (LETTERS_11("CANDY LAND")),
//     Difficulty::MEDIUM,
//     L_PLAYER(Facing::LEFT, 2, 4),
//     L_ONE(LevelObjType::PICKUP, 8, 2),
//     L_ONE(LevelObjType::PICKUP, 8, 6),
//     L_MANY(LevelObjType::SOLID_WALL, L_VERTICAL, 3, 5, 3),
//     L_MANY(LevelObjType::TIMED_WALL, L_VERTICAL, 3, 5, 0),
//     L_MANY(LevelObjType::TIMED_WALL, L_VERTICAL, 3, 5, 6),
// #ifdef TEST_LEVEL_SOLUTION
//     L_CMD_ONE(4),
//     PACK(CMD_WAIT, CMD_MOVE),
//     PACK(CMD_MOVE, CMD_MOVE),
//     PACK(CMD_MOVE, CMD_PICKUP),
//     PACK(CMD_TURN_RIGHT, CMD_END),
//     L_CMD_MAIN(5),
//     PACK(CMD_MOVE, CMD_MOVE),
//     PACK(TURN_LEFT, CMD_MOVE),
//     PACK(CMD_MOVE, TURN_RIGHT),
//     PACK(CMD_JMP_ONE, CMD_JMP_ONE),
//     PACK(CMD_END, CMD_END),
// #else
// #endif
//     L_END,
// );

NEXT_LEVEL(
    (LETTERS_14("ITS A FREEBIE")),
    Difficulty::EASY,
    L_PLAYER(Facing::Up, 5, 5),
    L_ONE(LevelObjType::PICKUP, 5, 1),
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
    Difficulty::EASY,
    L_PLAYER(Facing::Up, 5, 5),
    L_ONE(LevelObjType::PICKUP, 5, 1),
    L_ONE(LevelObjType::PICKUP, 5, 2),
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
    Difficulty::EASY,
    L_PLAYER(Facing::Up, 5, 5),
    L_ONE(LevelObjType::PICKUP, 5, 1),
    L_MANY(LevelObjType::SOLID_WALL, L_HORIZONTAL, 4, 0, 3),
    L_MANY(LevelObjType::SOLID_WALL, L_HORIZONTAL, 5, 5, 3),
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
    (LETTERS_14("TIMING IS KEY")),
    Difficulty::EASY,
    L_PLAYER(Facing::Up, 5, 5),
    L_ONE(LevelObjType::PICKUP, 5, 1),
    L_MANY(LevelObjType::TIMED_WALL, L_HORIZONTAL, 10, 0, 2),
#ifdef TEST_LEVEL_SOLUTION
    L_CMD_MAIN(3),
    PACK(CMD_MOVE, CMD_MOVE), PACK(CMD_WAIT, CMD_MOVE),
    PACK(CMD_MOVE, CMD_PICKUP),
#else
    L_CMD_MAIN(3),
    PACK(CMD_MOVE, CMD_MOVE), PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_PICKUP, CMD_END),
#endif
    L_END,
);

NEXT_LEVEL(
    (LETTERS_15("CALL 1 FOR FUN")),
    Difficulty::MEDIUM,
    L_PLAYER(Facing::Down, 5, 4),
    L_ONE(LevelObjType::PICKUP, 3, 2),
    L_ONE(LevelObjType::PICKUP, 7, 2),
    L_ONE(LevelObjType::PICKUP, 3, 6),
    L_ONE(LevelObjType::PICKUP, 7, 6),
    L_MANY(LevelObjType::HURT_WALL, L_HORIZONTAL, 7, 2, 1),
    L_MANY(LevelObjType::HURT_WALL, L_HORIZONTAL, 7, 2, 7),
    L_MANY(LevelObjType::HURT_WALL, L_VERTICAL, 5, 2, 2),
    L_MANY(LevelObjType::HURT_WALL, L_VERTICAL, 5, 8, 2),
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
    (LETTERS_12("FORKED PATH")),
    Difficulty::MEDIUM,
    L_PLAYER(Facing::Right, 2, 4),
    L_ONE(LevelObjType::PICKUP, 8, 2),
    L_ONE(LevelObjType::PICKUP, 8, 6),
    L_MANY(LevelObjType::SOLID_WALL, L_VERTICAL, 3, 5, 3),
    L_MANY(LevelObjType::TIMED_WALL, L_VERTICAL, 3, 5, 0),
    L_MANY(LevelObjType::TIMED_WALL, L_VERTICAL, 3, 5, 6),
#ifdef TEST_LEVEL_SOLUTION
    L_CMD_ONE(4),
    PACK(CMD_WAIT, CMD_MOVE),
    PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_MOVE, CMD_PICKUP),
    PACK(CMD_TURN_RIGHT, CMD_END),
    L_CMD_MAIN(5),
    PACK(CMD_MOVE, CMD_MOVE),
    PACK(TURN_LEFT, CMD_MOVE),
    PACK(CMD_MOVE, TURN_RIGHT),
    PACK(CMD_JMP_ONE, CMD_JMP_ONE),
    PACK(CMD_END, CMD_END),
#else
#endif
    L_END,
);


NEXT_LEVEL(
    (LETTERS_15("NOT A SHORTCUT")),
    Difficulty::MEDIUM,
    L_PLAYER(Facing::Right, 8, 7),
    L_ONE(LevelObjType::PICKUP, 8, 1),
    L_MANY(LevelObjType::SOLID_WALL, L_HORIZONTAL, 8, 1, 3),
    L_MANY(LevelObjType::SOLID_WALL, L_HORIZONTAL, 8, 1, 4),
    L_MANY(LevelObjType::SOLID_WALL, L_HORIZONTAL, 8, 1, 5),
    L_MANY(LevelObjType::TIMED_WALL, L_VERTICAL, 9, 9, 0),
#ifdef TEST_LEVEL_SOLUTION
    L_CMD_ONE(4),
    PACK(CMD_TURN_RIGHT, CMD_MOVE),
    PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_MOVE, CMD_MOVE),
    L_CMD_MAIN(5),
    PACK(CMD_TURN_RIGHT, CMD_JMP_ONE),
    PACK(CMD_MOVE, CMD_JMP_ONE),
    PACK(CMD_JMP_ONE, CMD_MOVE),
    PACK(CMD_TURN_RIGHT, CMD_MOVE),
    PACK(CMD_PICKUP, CMD_END),
#else
#endif
    L_END,
);

NEXT_LEVEL(
    (LETTERS_14("GRATS YOU WON")),
    Difficulty::HARD,
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

#define CONCAT_LEVEL_DIFFICULTY(x) CONCAT(LEVEL_DIFFICULTY_, x)
#define CONCAT_LEVEL_TITLE(x) CONCAT(LEVEL_TITLE_, x)
#define CONCAT_LEVEL_DATA(x) CONCAT(LEVEL_DATA_, x)
#define CONCAT_LEVEL_PASSWORD(x) CONCAT(LEVEL_PASSWORD_, x)

#define LEVEL_POINTERS_IMPL(...) \
    SPLIT_ARRAY_IMPL(level_titles, FOR_EACH(CONCAT_LEVEL_TITLE, __VA_ARGS__) ); \
    SPLIT_ARRAY_IMPL(all_levels, FOR_EACH(CONCAT_LEVEL_DATA, __VA_ARGS__) ); \
    FIXED constinit const soa::Array<uint16_t, LEVEL_COUNT> level_passwords = { \
        FOR_EACH(CONCAT_LEVEL_PASSWORD, __VA_ARGS__) \
    }; \
    FIXED constinit const Difficulty level_difficulty[] = { \
        FOR_EACH(CONCAT_LEVEL_DIFFICULTY, __VA_ARGS__) \
    };

#define LEVEL_POINTERS_COUNT(n) \
    LEVEL_POINTERS_IMPL(GENERATE_N_1(n))

#define LEVEL_POINTERS() LEVEL_POINTERS_COUNT(__COUNTER__)

LEVEL_POINTERS();

const Letter password_alphabet[16] = {
    A, B, C, D,
    E, F, G, H,
    I, J, K, O,
    N, P, R, S
};
