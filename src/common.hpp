#pragma once

#include <stdint.h>
#include <stddef.h>
#include <peekpoke.h>
#include <soa.h>

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
    SMALL_X,
    PORTAL,
    FOUR,
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
    CURSOR,
};

enum ObjSlot {
    SLOT_PLAYER = 0,
    SLOT_MAINCURSOR = 1,
    SLOT_CMDCURSOR = 2,
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

extern "C" void delay_a_27_clocks(uint8_t a);
extern "C" void delay_256a_x_33_clocks(uint8_t a, uint8_t x);


extern uint8_t VRAM_BUF[128];
extern uint8_t VRAM_INDEX;

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
    if ((int8_t)val < (int8_t)bound) {
        val = reset;
    }
}


// Custom MIN/MAX macros that do not double evaluate the inputs
#define MMAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define MMIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _b : _a; })


#define PACK(x, y) ((((x) & 0xf) << 4)|(((y) & 0xf)))
#define UNPACK(s) (Coord{.x=(uint8_t)(((s) & 0xf0) >> 4),.y=(uint8_t)(((s) & 0xf))})


#define APPLY_1(macro, a) macro(a)
#define APPLY_2(macro, a, ...) macro(a), APPLY_1(macro, __VA_ARGS__)
#define APPLY_3(macro, a, ...) macro(a), APPLY_2(macro, __VA_ARGS__)
#define APPLY_4(macro, a, ...) macro(a), APPLY_3(macro, __VA_ARGS__)
#define APPLY_5(macro, a, ...) macro(a), APPLY_4(macro, __VA_ARGS__)
#define APPLY_6(macro, a, ...) macro(a), APPLY_5(macro, __VA_ARGS__)
#define APPLY_7(macro, a, ...) macro(a), APPLY_6(macro, __VA_ARGS__)
#define APPLY_8(macro, a, ...) macro(a), APPLY_7(macro, __VA_ARGS__)
#define APPLY_9(macro, a, ...) macro(a), APPLY_8(macro, __VA_ARGS__)
#define APPLY_10(macro, a, ...) macro(a), APPLY_9(macro, __VA_ARGS__)
#define APPLY_11(macro, a, ...) macro(a), APPLY_10(macro, __VA_ARGS__)
#define APPLY_12(macro, a, ...) macro(a), APPLY_11(macro, __VA_ARGS__)
#define APPLY_13(macro, a, ...) macro(a), APPLY_12(macro, __VA_ARGS__)
#define APPLY_14(macro, a, ...) macro(a), APPLY_13(macro, __VA_ARGS__)
#define APPLY_15(macro, a, ...) macro(a), APPLY_14(macro, __VA_ARGS__)
#define APPLY_16(macro, a, ...) macro(a), APPLY_15(macro, __VA_ARGS__)

#define GET_APPLY_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, NAME, ...) NAME

#define FOR_EACH(macro, ...) \
    GET_APPLY_MACRO(__VA_ARGS__, APPLY_16, \
        APPLY_15, APPLY_14, APPLY_13, APPLY_12, APPLY_11, \
        APPLY_10, APPLY_9, APPLY_8, APPLY_7, APPLY_6, \
        APPLY_5, APPLY_4, APPLY_3, APPLY_2, APPLY_1)(macro, __VA_ARGS__)

#define COMBINE_1(macro, a) macro(a)
#define COMBINE_2(macro, a, ...) macro(a) COMBINE_1(macro, __VA_ARGS__)
#define COMBINE_3(macro, a, ...) macro(a) COMBINE_2(macro, __VA_ARGS__)
#define COMBINE_4(macro, a, ...) macro(a) COMBINE_3(macro, __VA_ARGS__)
#define COMBINE_5(macro, a, ...) macro(a) COMBINE_4(macro, __VA_ARGS__)
#define COMBINE_6(macro, a, ...) macro(a) COMBINE_5(macro, __VA_ARGS__)
#define COMBINE_7(macro, a, ...) macro(a) COMBINE_6(macro, __VA_ARGS__)
#define COMBINE_8(macro, a, ...) macro(a) COMBINE_7(macro, __VA_ARGS__)
#define COMBINE_9(macro, a, ...) macro(a) COMBINE_8(macro, __VA_ARGS__)
#define COMBINE_10(macro, a, ...) macro(a) COMBINE_9(macro, __VA_ARGS__)
#define COMBINE_11(macro, a, ...) macro(a) COMBINE_10(macro, __VA_ARGS__)
#define COMBINE_12(macro, a, ...) macro(a) COMBINE_11(macro, __VA_ARGS__)
#define COMBINE_13(macro, a, ...) macro(a) COMBINE_12(macro, __VA_ARGS__)
#define COMBINE_14(macro, a, ...) macro(a) COMBINE_13(macro, __VA_ARGS__)
#define COMBINE_15(macro, a, ...) macro(a) COMBINE_14(macro, __VA_ARGS__)
#define COMBINE_16(macro, a, ...) macro(a) COMBINE_15(macro, __VA_ARGS__)

#define GET_COMBINE_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, NAME, ...) NAME

#define COMBINE_FOR_EACH(macro, ...) \
    GET_COMBINE_MACRO(__VA_ARGS__, COMBINE_16, \
        COMBINE_15, COMBINE_14, COMBINE_13, COMBINE_12, COMBINE_11, \
        COMBINE_10, COMBINE_9, COMBINE_8, COMBINE_7, COMBINE_6, \
        COMBINE_5, COMBINE_4, COMBINE_3, COMBINE_2, COMBINE_1)(macro, __VA_ARGS__)

#define DEFINE_EXTERN(n) extern const uint8_t n[]; 
#define SPLIT_ARRAY_LO_BYTE(n) ".byte " #n "@mos16lo\n"
#define SPLIT_ARRAY_HI_BYTE(n) ".byte " #n "@mos16hi\n"

#define SPLIT_ARRAY_DEFINE(name, ...) \
    extern const unsigned char name##_lo_table[]; \
    extern const unsigned char name##_hi_table[];

#define SPLIT_ARRAY_IMPL(name, ...) \
    __asm__(".globl " #name "_lo_table\n  .globl " #name "_hi_table\n" \
        "" #name "_lo_table:\n" \
        COMBINE_FOR_EACH(SPLIT_ARRAY_LO_BYTE, __VA_ARGS__) \
        "" #name "_hi_table:\n" \
        COMBINE_FOR_EACH(SPLIT_ARRAY_HI_BYTE, __VA_ARGS__) \
    );

#define SPLIT_ARRAY(name, ...)\
    SPLIT_ARRAY_DEFINE(name, __VA_ARGS__)\
    SPLIT_ARRAY_IMPL(name, __VA_ARGS__)

#define SPLIT_ARRAY_POINTER(name, idx) \
    (void*)( name##_hi_table[idx] << 8 | name##_lo_table[idx])

#ifdef __cplusplus
}
#endif


struct Coord {
    uint8_t x;
    uint8_t y;
};
#define SOA_STRUCT Coord
#define SOA_MEMBERS MEMBER(x) MEMBER(y)
#include <soa-struct.inc>

union Point {
    uint8_t raw;
    struct {
        uint8_t y : 4;
        uint8_t x : 4;
    };
};


union Word {
    uint16_t raw;
    struct {
        uint8_t lo;
        uint8_t hi;
    };
};
