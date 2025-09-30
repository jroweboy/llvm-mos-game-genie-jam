
#include <cstdint>

#include "common.hpp"
#include "game.hpp"
#include "levels.hpp"
#include "text_render.hpp"

const Letter password_alphabet[16] = {
    A, B, C, D,
    E, F, G, H,
    I, J, K, O,
    N, P, R, S
};

// constexpr const Letter PASSWORD_ALPHABET[] = {
//     A, B, C, D,
//     E, F, G, H,
//     I, J, K, O,
//     N, P, R, S
// };

consteval static uint16_t generate_password(uint8_t lev) {
    unsigned seed = (0xface + lev);
    
    unsigned x = seed;
    x ^= x << 7;
    x ^= x >> 9;
    x ^= x << 8;
    seed = x;
    // auto [nybble0, nybble1] = UNPACK(seed & 0xff);
    // auto [nybble2, nybble3] = UNPACK((seed >> 8) & 0xff);
    return seed;
}

extern const Letter LEVEL_TITLE_FREEBIE[] = { LETTERS_14("ITS A FREEBIE") };
extern const Letter LEVEL_TITLE_LESS_FREE[] = { LETTERS_12("ALMOST FREE") };
extern const Letter LEVEL_TITLE_ZIGZAG[] = { LETTERS_16("DODGING FOR FUN") };
extern const Letter LEVEL_TITLE_FINAL[] = { LETTERS_16("GAMEOVER WINNER") };

// extern const Letter LEVEL_PASSWORD_0[5] = PASSWORD_LIST(0);
// extern const Letter LEVEL_PASSWORD_1[5] = PASSWORD_LIST(1);
// extern const Letter LEVEL_PASSWORD_2[5] = PASSWORD_LIST(2);
// extern const Letter LEVEL_PASSWORD_FINAL[5] = { LETTERS_5("DONE") };

FIXED constinit const soa::Array<uint16_t, LEVEL_COUNT> level_passwords = {
    generate_password(0),
    generate_password(1),
    generate_password(2),
    generate_password(3),
};

// SPLIT_ARRAY(level_passwords,
//     LEVEL_PASSWORD_0,
//     LEVEL_PASSWORD_1,
//     LEVEL_PASSWORD_2,
//     LEVEL_PASSWORD_FINAL
// );

SPLIT_ARRAY_IMPL(level_titles,
    LEVEL_TITLE_FREEBIE,
    LEVEL_TITLE_LESS_FREE,
    LEVEL_TITLE_ZIGZAG,
    LEVEL_TITLE_FINAL
);

SPLIT_ARRAY_IMPL(all_levels,
    LEVEL_FREEBIE,
    LEVEL_SLIGHTLY_LESS_FREE,
    LEVEL_ZIGZAG,
    LEVEL_FINAL
);

extern const uint8_t LEVEL_FREEBIE[] = {
    L_ONE(LevelObjType::PICKUP, 5, 1),
    L_PLAYER(Facing::Up, 5, 5),
    L_CMD_MAIN(3),
    PACK(CMD_MOVE, CMD_MOVE), PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_PICKUP, CMD_END),
    L_END,
};

// extern const uint8_t LEVEL_FREEBIE[] = {
//     L_ONE(LevelObjType::PICKUP, 5, 1),
//     L_PLAYER(Facing::Up, 5, 5),
//     L_CMD_TWO(1),
//     PACK(CMD_PICKUP, CMD_END),
//     L_CMD_ONE(2),
//     PACK(CMD_MOVE, CMD_MOVE), PACK(CMD_END, CMD_END),
//     L_CMD_MAIN(2),
//     PACK(CMD_JMP_ONE, CMD_JMP_ONE), PACK(CMD_JMP_TWO, CMD_END),
//     L_END,
// };

extern const uint8_t LEVEL_SLIGHTLY_LESS_FREE[] = {
    L_ONE(LevelObjType::PICKUP, 5, 1),
    L_ONE(LevelObjType::PICKUP, 5, 2),
    L_PLAYER(Facing::Up, 5, 5),
    L_CMD_MAIN(3),
    PACK(CMD_MOVE, CMD_MOVE), PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_PICKUP, CMD_END), 
    L_END,
};

extern const uint8_t LEVEL_ZIGZAG[] = {
    L_MANY(LevelObjType::SOLID_WALL, L_HORIZONTAL, 4, 0, 3),
    L_MANY(LevelObjType::SOLID_WALL, L_HORIZONTAL, 5, 5, 3),
    L_ONE(LevelObjType::PICKUP, 5, 1),
    L_PLAYER(Facing::Up, 5, 5),
    L_CMD_MAIN(3),
    PACK(CMD_MOVE, CMD_MOVE), PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_PICKUP, CMD_END), 
    L_END,
};

// No pickups in the final level cause you already won
extern const uint8_t LEVEL_FINAL[] = {
    L_MANY(LevelObjType::SOLID_WALL, L_VERTICAL, 2, 1, 3),
    L_MANY(LevelObjType::SOLID_WALL, L_VERTICAL, 2, 7, 3),
    L_MANY(LevelObjType::SOLID_WALL, L_VERTICAL, 2, 2, 5),
    L_MANY(LevelObjType::SOLID_WALL, L_VERTICAL, 2, 6, 5),
    L_MANY(LevelObjType::SOLID_WALL, L_VERTICAL, 1, 3, 7),
    L_MANY(LevelObjType::SOLID_WALL, L_VERTICAL, 1, 5, 7),
    L_MANY(LevelObjType::SOLID_WALL, L_VERTICAL, 2, 4, 5),
    L_PLAYER(Facing::Up, 5, 5),
    L_END,
};

extern const uint8_t LEVEL_AMAZING[] = {
    // L_MANY(LevelObjType::SOLID_WALL, L_HORIZONTAL, 4, 0, 3),
    // L_MANY(LevelObjType::SOLID_WALL, L_HORIZONTAL, 5, 5, 3),
    L_ONE(LevelObjType::PICKUP, 5, 1),
    L_PLAYER(Facing::Up, 5, 5),
    L_CMD_MAIN(3),
    PACK(CMD_MOVE, CMD_MOVE), PACK(CMD_MOVE, CMD_MOVE),
    PACK(CMD_PICKUP, CMD_END), 
    L_END,
};
