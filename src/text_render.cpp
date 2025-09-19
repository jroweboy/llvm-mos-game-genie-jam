

#include <cstdint>
#include <nesdoug.h>
#include <neslib.h>

// We want to keep the bytes for each of the 
#include <soa.h>

#include "metatile.hpp"
#include "text_render.hpp"


/**
    Font definitions
*/

[[maybe_unused]]
static const Letter* test_string = "01001"_l;

// Include the VRAM buffer and the VRAM_INDEX so we can write directly into the buffer ourselves.
extern volatile uint8_t VRAM_BUF[128];
extern volatile __zeropage uint8_t VRAM_INDEX;
extern volatile __zeropage uint8_t NAME_UPD_ENABLE;

extern "C" void render_string(Nametable nmt, uint8_t x, uint8_t y, const Letter str[]) {
    uint8_t len = (uint8_t)str[0];
    for (uint8_t i = 1; i < len; i++) {
        auto letter = str[i];
        if (!SKIP_DRAWING_SPACE || letter != Letter::SPACE) {
            draw_metatile_2_3(nmt, x, y, letter);
        }
        if (HALF_SIZE_SPACE && letter == Letter::SPACE)
            x += 1;
        else
            x += 2;
        if (x >= 31) {
            x = 0;
            y += 3;
        }
        if (VRAM_INDEX > (128 - 14)) {
            flush_vram_update2();
        }
    }
    NAME_UPD_ENABLE = true;
}
