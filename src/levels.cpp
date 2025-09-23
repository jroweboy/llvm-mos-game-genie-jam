
#include <cstddef>
#include <nesdoug.h>
#include <neslib.h>
#include <stdint.h>
#include <peekpoke.h>

#include "common.hpp"
#include "game.hpp"
#include "graphics.hpp"
#include "metatile.hpp"
#include "levels.hpp"
#include "rle.hpp"

extern volatile __zeropage uint8_t VRAM_INDEX;

static const uint8_t* __zp current_level;
static __zp uint8_t level_offset;
static __zp uint8_t cmd;

constexpr uint8_t LEVEL_X_POS = 10;
constexpr uint8_t LEVEL_Y_POS = 2;


void timed_wall_change_color(uint8_t slot, uint8_t pal) {
    auto obj = objects[slot];
    auto lendir = obj.param1.get();

    auto len = (uint8_t)(lendir & ~(C_VERTICAL));
    if (lendir & C_VERTICAL)
        for (uint8_t i = 0; i < len; ++i) {
            update_attribute(obj->x, obj->y + i * 2, pal);
        }
    else
        for (uint8_t i = 0; i < len; ++i) {
            update_attribute(obj->x + i * 2, obj->y, pal);
        }
}

static void create_timed_wall_obj(uint8_t slot, uint8_t lendir, uint8_t x, uint8_t y) {
    auto obj = objects[slot];
    obj.type = ObjectType::TIMED_WALL;
    obj.x = x;
    obj.y = y;
    obj.param1 = lendir;
    timed_wall_change_color(slot, BG_PALETTE_GREEN);
}

struct WorldSpacePoint {
    uint8_t x;
    uint8_t y;
};

static inline WorldSpacePoint read_pos() {
    Point p = (Point)current_level[level_offset++]; 
    return {
        (uint8_t)((p.x << 1) + LEVEL_X_POS),
        (uint8_t)((p.y << 1) + LEVEL_Y_POS),
    };
}


static void create_wall(uint8_t slot) {
    auto metatile = Metatile::WALL;
    auto [x, y] = read_pos();
    auto orig_x = x;
    auto orig_y = y;
    auto lendir = 1;
    if (cmd & C_MULTIPLE) {
        lendir = current_level[level_offset++];
        auto len = (uint8_t)(lendir & ~(C_VERTICAL));
        for (uint8_t i=0; i<len; i++) {
            draw_metatile_2_2(Nametable::A, x, y, metatile);
            if (lendir & C_VERTICAL) {
                y += 2;
            } else {
                x += 2;
            }
        }
    } else {
        draw_metatile_2_2(Nametable::A, x, y, metatile);
    }
    if (slot)
        create_timed_wall_obj(slot, lendir, orig_x, orig_y);
}

const uint8_t level_hud[] = {
    M_HORZ(3, 2, 2, SEPARATOR, SUB, SEPARATOR),
    M_HORZ(3, 2, 12, SEPARATOR, ONE, SEPARATOR),
    M_HORZ(3, 2, 20, SEPARATOR, TWO, SEPARATOR),
    M_HORZ(3, 10, 22, TURN_LEFT, MOVE, TURN_RIGHT),
    M_HORZ(3, 10, 24, WAIT, BLANK, PICKUP),
    M_HORZ(3, 10, 26, SUB, ONE, TWO),
    T_HORZ_REPT(20, 10, 1, MTILE_BL(BORDER_TOP)),
    T_HORZ_REPT(20, 10, 20, MTILE_TL(BORDER_BOT)),
    T_VERT_REPT(18, 9, 2, MTILE_BR(BORDER_LEFT)),
    T_VERT_REPT(18, 30, 2, MTILE_BL(BORDER_RIGHT)),
    T_ONE(9, 1, MTILE_BR(BORDER_TL_CORNER)),
    T_ONE(30, 1, MTILE_BL(BORDER_TR_CORNER)),
    T_ONE(9, 20, MTILE_TR(BORDER_BL_CORNER)),
    T_ONE(30, 20, MTILE_TL(BORDER_BR_CORNER)),
    NT_UPD_EOF
};

FIXED constinit auto hud_attr_rle = RLE(
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x50,0x00,0x00,0x00,0x00,
0x00,0x00,0x08,0x0c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
);

void draw_hud([[maybe_unused]] uint8_t level_num) {
    // TODO check the 
    vram_adr(NAMETABLE_A);
    vram_fill(0, 0x400);
    set_vram_update(level_hud);
    flush_vram_update2();
    set_vram_buffer();
    vram_adr(NAMETABLE_A | 0x3c0);
    vram_unrle(hud_attr_rle.data);
}

void load_level(uint8_t level_num) {
    current_level = all_levels[level_num];
    level_offset = 0;
    // objects[1] = {
    //     .type = CURSOR,
    //     .x = 10 * 8,
    //     .y = 22 * 8,
    //     .timer = 0,
    //     .frame = 0,
    //     .param1 = 4,
    // };
    while (true) {
        cmd = current_level[level_offset++];
        switch (static_cast<LevelObjType>(static_cast<LevelObjId>(cmd).id)) {
        case LevelObjType::TERMINATOR:
            ppu_wait_nmi();
            return;
        case LevelObjType::TIMED_WALL: {
            auto slot = find_obj_slot(ObjectType::TIMED_WALL);
            create_wall(slot);
            break;
        }
        case LevelObjType::SOLID_WALL: {
            create_wall(0);
            break;
        }
        case LevelObjType::PICKUP: {
            auto [x, y] = read_pos();
            draw_metatile_2_2(Nametable::A, x, y, Metatile::ITEM);
            update_attribute(x, y, BG_PALETTE_GREEN);
            break;
        }
        case LevelObjType::ENEMY:
        case LevelObjType::PLAYER:{

            break;
        }
        case LevelObjType::FLUSH_VRAM:
            ppu_wait_nmi();
            break;
        }
    }
}