#pragma once

#include <cstdint>

__attribute__((leaf)) void pal_fade_to(char from, char to, char duration);

extern "C" void draw_cursor(uint8_t slot);
extern "C" void draw_player();

extern const uint8_t arrow_left_metasprite[];
extern const uint8_t arrow_up_metasprite[];
extern const uint8_t arrow_right_metasprite[];
extern const uint8_t arrow_down_metasprite[];