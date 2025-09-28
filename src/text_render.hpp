#pragma once

#include "common.hpp"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief If true, then instead of rendering the space glyph, this will just skip over space tile, saving cycles.
 *        This does mean that you need to clear the text area before drawing text on the other hand.
 *        If false, the space character will be rendered
 */
constexpr bool SKIP_DRAWING_SPACE = true;
/**
 * @brief If true, drawing a space uses only 1 tile width instead of two (allowing for slightly more compact horizontal lines)
 *        If SKIP_DRAWING_SPACE = false, the full 2x3 space glyph is rendered, but the next character position starts in the middle of the space
 */
constexpr bool HALF_SIZE_SPACE = true;

/**
 * @brief Draw a single letter to the VRAM_BUFFER. This can be used with rendering ON since it buffers the writes.
 *        A letter is a 2x3 metatile representing a 4x6 pixel image, with the custom font defined in `font.inc`
 *
 * NOTICE: This function does NOT handle attributes! You will need to settle that outside of this.
 * NOTICE: This function will NOT check to see if it overflows the VRAM_BUFFER!
 * 
 * @param X - position from 0 to 31 to start drawing the string at
 * @param Y - position from 0 to 26 to start drawing the string at
 * @param str - Single letter to render in a 2x3 block of tiles
 */
void draw_letter(Nametable nmt, uint8_t x, uint8_t y, Letter letter);


/**
 * @brief Draw all letters from the string into the provided coordinate.
 *        If the next X position would be off the screen, this function will move to the next line.
 *        It will not break up words or such, so you should probably reflow text yourself if you need that.
 *
 * NOTICE: This function does NOT handle attributes! You will need to settle that outside of this.
 * NOTICE: This function will prevent itself from overloading the VRAM buffer, so it may take more
 *         than a frame to complete if the buffer fills up.
 * 
 * @param X - position from 0 to 31 to start drawing the string at
 * @param Y - position from 0 to 26 to start drawing the string at
 * @param str - List of letters to render starting at that position.
 */
void render_string(Nametable nmt, uint8_t x, uint8_t y, const Letter letter[]);

// Character access macro
#define CHAR_AT(str, n) (LetterArray(str).out[n])

// Recursive expansion macros for different string lengths
#define EXPAND_CHARS_1(str) CHAR_AT(str, 0)
#define EXPAND_CHARS_2(str) EXPAND_CHARS_1(str), CHAR_AT(str, 1)
#define EXPAND_CHARS_3(str) EXPAND_CHARS_2(str), CHAR_AT(str, 2)
#define EXPAND_CHARS_4(str) EXPAND_CHARS_3(str), CHAR_AT(str, 3)
#define EXPAND_CHARS_5(str) EXPAND_CHARS_4(str), CHAR_AT(str, 4)
#define EXPAND_CHARS_6(str) EXPAND_CHARS_5(str), CHAR_AT(str, 5)
#define EXPAND_CHARS_7(str) EXPAND_CHARS_6(str), CHAR_AT(str, 6)
#define EXPAND_CHARS_8(str) EXPAND_CHARS_7(str), CHAR_AT(str, 7)
#define EXPAND_CHARS_9(str) EXPAND_CHARS_8(str), CHAR_AT(str, 8)
#define EXPAND_CHARS_10(str) EXPAND_CHARS_9(str), CHAR_AT(str, 9)
#define EXPAND_CHARS_11(str) EXPAND_CHARS_10(str), CHAR_AT(str, 10)
#define EXPAND_CHARS_12(str) EXPAND_CHARS_11(str), CHAR_AT(str, 11)
#define EXPAND_CHARS_13(str) EXPAND_CHARS_12(str), CHAR_AT(str, 12)
#define EXPAND_CHARS_14(str) EXPAND_CHARS_13(str), CHAR_AT(str, 13)
#define EXPAND_CHARS_15(str) EXPAND_CHARS_14(str), CHAR_AT(str, 14)
#define EXPAND_CHARS_16(str) EXPAND_CHARS_15(str), CHAR_AT(str, 15)
#define EXPAND_CHARS_17(str) EXPAND_CHARS_16(str), CHAR_AT(str, 16)
#define EXPAND_CHARS_18(str) EXPAND_CHARS_17(str), CHAR_AT(str, 17)
#define EXPAND_CHARS_19(str) EXPAND_CHARS_18(str), CHAR_AT(str, 18)
#define EXPAND_CHARS_20(str) EXPAND_CHARS_19(str), CHAR_AT(str, 19)

// Convenience macros for common lengths
#define LETTERS_1(str) EXPAND_CHARS_1(str)
#define LETTERS_2(str) EXPAND_CHARS_2(str)
#define LETTERS_3(str) EXPAND_CHARS_3(str)
#define LETTERS_4(str) EXPAND_CHARS_4(str)
#define LETTERS_5(str) EXPAND_CHARS_5(str)
#define LETTERS_6(str) EXPAND_CHARS_6(str)
#define LETTERS_7(str) EXPAND_CHARS_7(str)
#define LETTERS_8(str) EXPAND_CHARS_8(str)
#define LETTERS_9(str) EXPAND_CHARS_9(str)
#define LETTERS_10(str) EXPAND_CHARS_10(str)
#define LETTERS_11(str) EXPAND_CHARS_11(str)
#define LETTERS_12(str) EXPAND_CHARS_12(str)
#define LETTERS_13(str) EXPAND_CHARS_13(str)
#define LETTERS_14(str) EXPAND_CHARS_14(str)
#define LETTERS_15(str) EXPAND_CHARS_15(str)
#define LETTERS_16(str) EXPAND_CHARS_16(str)
#define LETTERS_17(str) EXPAND_CHARS_17(str)
#define LETTERS_18(str) EXPAND_CHARS_18(str)


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
template<size_t N>
struct LetterArray
{
    Letter out[N]{};
 
    consteval LetterArray(char const(&text)[N])
    {
        out[0] = (Letter)((uint8_t)N);
        for (size_t i = 0; i < N; i++) {
            char c = text[i];
            if (c == '\0') break;
            Letter let = Letter::SPACE;
            if (c >= '0' && c <= '9') {
                let = (Letter) (c - '0' + Letter::_0);
            } else if (c >= 'A' && c <= 'Z') {
                let = (Letter) (c - 'A' + Letter::A);
            } else if (c >= 'a' && c <= 'z') {
                let = (Letter) (c - 'a' + Letter::A);
            } else if (c == ' ') {
                let = Letter::SPACE;
            }
            out[i+1] = let;
        }
    }
};

template<LetterArray A>
consteval auto operator""_l() {
    return A.out;
}


#endif