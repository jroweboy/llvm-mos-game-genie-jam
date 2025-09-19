#pragma once

#include <stdint.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

enum class Nametable : uint8_t {
    A = 0x00,
    B = 0x04,
    C = 0x08,
    D = 0x0c,
};

enum class Metatile : uint8_t {
    MOVE,
    TURN_LEFT,
    TURN_RIGHT,
    WAIT,
    PICKUP,
    SUB,
    ONE,
    TWO,
    WALL,
    ITEM,
    BORDER_TOP,
    BORDER_BOT,
    BORDER_LEFT,
    BORDER_RIGHT,
    BORDER_TL_CORNER,
    BORDER_TR_CORNER,
    BORDER_BL_CORNER,
    BORDER_BR_CORNER,
    COUNT,
};

/**
 * @brief List of possible letters in our custom font. If you would like to update this list,
 *        then add a new character, and update the `font.inc` to add your character at the same location
 *        in the list.
 */
enum Letter {
    _0,
    _1,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7,
    _8,
    _9,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    SPACE,
    COUNT,
};

enum class ObjectType : uint8_t {
    PLAYER,
    ENEMY,
    TIMED_WALL,
};



#ifdef __cplusplus
}
#endif
