

#include <nesdoug.h>
#include <neslib.h>
#include <stdint.h>
#include <peekpoke.h>

#include "common.hpp"
#include "game.hpp"
#include "metatile.hpp"
#include "levels.hpp"

extern volatile __zeropage uint8_t VRAM_INDEX;

static const uint8_t* __zp current_level;
// static __zp uint8_t level_offset;
static __zp uint8_t cmd;

constexpr uint8_t LEVEL_X_POS = 10;
constexpr uint8_t LEVEL_Y_POS = 2;


void timed_wall_change_color(uint8_t slot, uint8_t pal) {
    auto obj = objects[slot];
    auto lendir = obj.param1.get();

    auto len = (uint8_t)(lendir & ~(L_VERTICAL));
    if (lendir & L_VERTICAL)
        for (uint8_t i = 0; i < len; ++i) {
            update_attribute(obj->x.as_i(), obj->y.as_i() + i * 2, pal);
        }
    else
        for (uint8_t i = 0; i < len; ++i) {
            update_attribute(obj->x.as_i() + i * 2, obj->y.as_i(), pal);
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
    Point p = (Point)*current_level++; 
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
    if (cmd & L_MULTIPLE) {
        lendir = *current_level++;
        auto len = (uint8_t)(lendir & ~(L_VERTICAL));
        for (uint8_t i=0; i<len; i++) {
            draw_metatile_2_2(Nametable::A, x, y, metatile);
            if (lendir & L_VERTICAL) {
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

const uint8_t level_hud_2[] = {
    A_HORZ(2, 8, 20, 0xc0, 0x90),
    A_HORZ(2, 8, 24, 0x80, 0x7c),
    M_HORZ(3, 10, 22, TURN_LEFT, MOVE, TURN_RIGHT),
    M_HORZ(3, 10, 24, WAIT, BLANK, PICKUP),
    M_HORZ(3, 10, 26, SUB, ONE, TWO),
    NT_UPD_EOF
};

void draw_hud([[maybe_unused]] uint8_t level_num) {
    // TODO check the 
    vram_adr(NAMETABLE_A);
    vram_fill(0, 0x400);
    set_vram_update(level_hud);
    flush_vram_update2();
    set_vram_update(level_hud_2);
    flush_vram_update2();
    
    set_vram_buffer();
}

static inline uint8_t find_obj_slot() {
    for (int i=1; i<8; i++) {
        auto obj = objects[i];
        if (obj->type == NO_OBJECT) {
            return i;
        }
    }
    return 1;
}

static void add_command() {
    uint8_t len = *current_level++;
    while (len-- > 0) {
        uint8_t next_byte = *current_level++;
        auto [cmd0, cmd1] = UNPACK(next_byte);
        update_command_list(cmd0);
        if (cmd1 != CMD_END)
            update_command_list(cmd1);
        flush_vram_update2();
    }
}

void load_level(uint8_t level_num) {
    current_level = (uint8_t*)SPLIT_ARRAY_POINTER(all_levels, level_num);
    // current_level = all_levels[level_num];
    // level_offset = 0;
    while (true) {
        cmd = *current_level++;
        switch (static_cast<LevelObjType>(cmd & 0x0f)) {
        case LevelObjType::TERMINATOR:
            ppu_wait_nmi();
            return;
        case LevelObjType::TIMED_WALL: {
            auto slot = find_obj_slot();
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
        case LevelObjType::ENEMY: {
            auto [x, y] = read_pos();
            auto slot = find_obj_slot();
            auto enemy = objects[slot];
            enemy.type = ObjectType::PACE_ENEMY;
            enemy.x = x;
            
            break;
        }
        case LevelObjType::PLAYER:{
            auto [x, y] = read_pos();
            constexpr uint8_t slot = 0;
            auto player = objects[slot];
            player.type = ObjectType::PLAYER;
            player.x = x;
            player.y = y;
            // player.param1 = ;

            break;
        }
        case LevelObjType::FLUSH_VRAM:
            ppu_wait_nmi();
            break;
        case LevelObjType::CMD_MAIN:
            current_sub = 0;
            add_command();
            break;
        case LevelObjType::CMD_ONE:
            current_sub = 1;
            add_command();
            break;
        case LevelObjType::CMD_TWO:
            current_sub = 2;
            add_command();
            break;
        }
        flush_vram_update2();
    }
}