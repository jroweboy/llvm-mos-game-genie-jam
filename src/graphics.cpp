
#include <neslib.h>

#include "graphics.hpp"

void pal_fade_to(char from, char to, char duration) {
    while (from != to) {
        from = (from < to) ? from + 1 : from - 1;
        pal_bright(from);
        delay(duration);
    }
    // Wait one additional frame to make sure the last fade happens before exiting
    ppu_wait_nmi();
}
