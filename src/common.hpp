#pragma once

#include <stdint.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

#define NT_UPD_REPT 0x80

#define FIXED __attribute__((section(".prg_rom_fixed")))

enum class Nametable : uint8_t {
    A = 0x00,
    B = 0x04,
    C = 0x08,
    D = 0x0c,
};

enum Metatile {
    BLANK,
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
    SEPARATOR,
    METATILE_COUNT,
};

enum BGColors {
    BG_PALETTE_TAN,
    BG_PALETTE_BLUE,
    BG_PALETTE_RED,
    BG_PALETTE_GREEN,
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


enum ObjectType {
    NO_OBJECT,
    PLAYER,
    TIMED_WALL,
    PACE_ENEMY,
};

enum GameMode {
    MODE_RESET = 0,
    MODE_TITLE,
    MODE_LOAD_LEVEL,
    MODE_PASSWORD,
    MODE_EDIT,
    MODE_EXECUTE
};

extern uint8_t level;
extern GameMode game_mode;

void set_game_mode(GameMode game_mode);

#define _DEBUGGER_0() { POKE(0x4018, 0); }
#define _DEBUGGER_1(a) { POKE(0x4018, (uint8_t)a); }
#define _DEBUGGER_X(x,A,FUNC,...)  FUNC
#define DEBUGGER(...) _DEBUGGER_X(,##__VA_ARGS__,\
  _DEBUGGER_1(__VA_ARGS__),\
  _DEBUGGER_0(__VA_ARGS__))

// constexpr inline void wrapped_add(uint8_t& val, uint8_t v, uint8_t bound, uint8_t reset = 0) {
//     val += v;
//     if ((int8_t)val >= (int8_t)bound) {
//         val = reset + (val - bound);
//     }
// }

// constexpr inline void wrapped_sub(uint8_t& val, uint8_t v, uint8_t bound, uint8_t reset = 0) {
//     val -= v;
//     if ((int8_t)val < (int8_t)bound-1) {
//         val = reset + (val + bound);
//     }
// }
constexpr inline void wrapped_add(uint8_t& value, uint8_t add_val, uint8_t lower_bound, uint8_t upper_bound) {
    value += add_val;
    
    // Check if we exceeded upper bound
    if (value > upper_bound) {
        // Wrap to lower bound + overflow amount
        value = lower_bound + (value - upper_bound);
    }
}

constexpr inline void wrapped_sub(uint8_t& value, uint8_t sub_val, uint8_t lower_bound, uint8_t upper_bound) {
    value -= sub_val;
    
    // Check if we went below lower bound  
    if ((int8_t)value < (int8_t)lower_bound) {
        // Wrap to upper bound - underflow amount
        value = upper_bound + (value - lower_bound);
    }
}

constexpr inline void wrapped_inc(uint8_t& val, uint8_t bound, uint8_t reset = 0) {
    val++;
    if ((int8_t)val >= (int8_t)bound) {
        val = reset;
    }
}

constexpr inline void wrapped_dec(uint8_t& val, uint8_t bound, uint8_t reset = 0) {
    val--;
    if ((int8_t)val < (int8_t)bound-1) {
        val = reset;
    }
}

#ifdef __cplusplus
}
#endif
