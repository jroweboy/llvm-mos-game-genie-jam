
#include "graphics.hpp"

__attribute__((section(".prg_rom_fixed")))
const soa::Array<Metatile_2_2, static_cast<uint8_t>(Metatile::COUNT)> metatiles = {
    #include "metatile_graphics.inc"
};

