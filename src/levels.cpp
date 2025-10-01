

#include <nesdoug.h>
#include <neslib.h>
#include <stdint.h>
#include <peekpoke.h>

#include "common.hpp"
#include "game.hpp"
#include "metatile.hpp"
#include "levels.hpp"
#include <string.h>


// static const uint8_t* __zp current_level;
static uint8_t level_offset;
static uint8_t cmd;

constexpr uint8_t LEVEL_X_POS = 10;
constexpr uint8_t LEVEL_Y_POS = 2;

uint8_t pickup_count;
uint8_t level_metatiles[(10 * 9) / 2];

static uint8_t find_obj_slot() {
    for (int i=3; i<10; i++) {
        auto obj = objects[i];
        if (obj->type == NO_OBJECT) {
            return i;
        }
    }
    return 3;
}

void timed_wall_change_color(uint8_t slot, uint8_t pal) {
    auto obj = objects[slot];
    auto lendir = obj.facing_dir.get();

    auto len = (uint8_t)(lendir & ~(L_VERTICAL));
    uint8_t x = obj->x;
    uint8_t y = obj->y;
    for (uint8_t i = 0; i < len; ++i) {
        update_attribute(x, y, pal);
        if (lendir & L_VERTICAL) {
            y += 2;
        } else {
            x += 2;
        }
        flush_vram_update2();
    }
}

static uint8_t create_timed_wall_obj(uint8_t lendir, uint8_t x, uint8_t y) {
    auto slot = find_obj_slot();
    auto obj = objects[slot];
    obj.type = ObjectType::TIMED_WALL;
    obj.x = x;
    obj.y = y;
    obj.facing_dir = lendir;
    timed_wall_change_color(slot, BG_PALETTE_GREEN);
    return slot;
}


Coord level_to_world(Point p) {
    return {
        (uint8_t)((p.x << 1) + LEVEL_X_POS),
        (uint8_t)((p.y << 1) + LEVEL_Y_POS),
    };
}

static Coord read_pos(uint8_t *current_level) {
    Point p = (Point)current_level[level_offset++];
    return level_to_world(p);
}

extern "C" __attribute__((noinline)) void update_level_buff(uint8_t tile_x, uint8_t tile_y, LevelObjType val) {
    uint8_t v = (uint8_t)val;
    uint8_t x = (tile_x - LEVEL_X_POS) >> 1;
    uint8_t y = (tile_y - LEVEL_Y_POS) >> 1;
    uint8_t idx = x + y * 10;
    if (idx & 1) {
        uint8_t old = level_metatiles[idx >> 1] & 0xf0;
        level_metatiles[idx >> 1] = old | v;
    } else {
        uint8_t old = level_metatiles[idx >> 1] & 0x0f;
        level_metatiles[idx >> 1] = old | (v << 4);
    }
}

extern "C" __attribute__((noinline)) LevelObjType load_metatile_at_coord(uint8_t px_x, uint8_t px_y) {
    // convert pixel position to offset in the world
    uint8_t x = (px_x >> 4) - LEVEL_X_POS/2;
    uint8_t y = (px_y >> 4) - LEVEL_Y_POS/2;
    uint8_t idx = x + y * 10;
    if (idx & 1) {
        return (LevelObjType) (level_metatiles[idx >> 1] & 0x0f);
    } else {
        return (LevelObjType) ((level_metatiles[idx >> 1] >> 4) & 0x0f);
    }
}

void level_draw_wall(uint8_t x, uint8_t y, uint8_t lendir, LevelObjType type) {
    auto metatile = Metatile::WALL;
    auto len = (uint8_t)(lendir & ~(L_VERTICAL));
    for (uint8_t i=0; i<len; i++) {
        update_level_buff(x, y, type);
        draw_metatile_2_2(Nametable::A, x, y, metatile);
        if (lendir & L_VERTICAL) {
            y += 2;
        } else {
            x += 2;
        }
    }
}

static void create_wall(uint8_t *current_level, LevelObjType type) {
    // auto type = slot ? LevelObjType::TIMED_WALL : LevelObjType::SOLID_WALL;
    auto [x, y] = read_pos(current_level);
    auto orig_x = x;
    auto orig_y = y;
    auto lendir = 1;
    if (cmd & L_MULTIPLE) {
        lendir = current_level[level_offset++];
        level_draw_wall(x, y, lendir, type);
    } else {
        level_draw_wall(x, y, 1, type);
        // update_level_buff(x, y, type);
        // draw_metatile_2_2(Nametable::A, x, y, metatile);
    }
    if (type != LevelObjType::SOLID_WALL) {
        uint8_t slot = create_timed_wall_obj(lendir, orig_x, orig_y);
        if (type == LevelObjType::HURT_WALL) {
            // Change the wall color and then delete the object
            timed_wall_change_color(slot, BG_PALETTE_RED);
            auto obj = objects[slot];
            obj.type = ObjectType::NO_OBJECT;
        }
    }
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
    M_HORZ(1, 26, 26, SMALL_X),
    NT_UPD_EOF
};

constinit const Metatile speed_tile_lut[3] = {
    ONE, TWO, FOUR,
};
constinit const uint8_t speed_attr_lut[3] = {
    BG_PALETTE_BLUE, BG_PALETTE_GREEN, BG_PALETTE_RED,
};

void update_speed_setting() {
    Metatile mtile = speed_tile_lut[speed_setting];
    uint8_t pal = speed_attr_lut[speed_setting];
    draw_metatile_2_2(Nametable::A, 28, 26, mtile);
    update_attribute(26, 26, pal);
    update_attribute(28, 26, pal);
}

void draw_hud([[maybe_unused]] uint8_t level_num) {
    // TODO check the level num to see what is enabled
    memset(attribute_buffer, 0, sizeof(attribute_buffer));
    vram_adr(NAMETABLE_A);
    vram_fill(0, 0x400);
    set_vram_update(level_hud);
    flush_vram_update2();
    set_vram_update(level_hud_2);
    attribute_buffer[0xea-0xc0] = 0xc0;
    attribute_buffer[0xeb-0xc0] = 0x90;
    attribute_buffer[0xf2-0xc0] = 0x80;
    attribute_buffer[0xf3-0xc0] = 0x7c;
    flush_vram_update2();
    set_vram_buffer();
    update_speed_setting();
    flush_vram_update2();
}

static void add_command(uint8_t* current_level) {
    uint8_t len = current_level[level_offset++];
    while (len-- > 0) {
        uint8_t next_byte = current_level[level_offset++];
        auto [cmd0, cmd1] = UNPACK(next_byte);
        if (cmd0 != CMD_END) {
            update_command_list(cmd0);
            if (cmd1 != CMD_END) {
                update_command_list(cmd1);
            }
        }
        flush_vram_update2();
    }
}

void draw_pickup(Point p) {
    auto [x, y] = level_to_world(p);
    update_level_buff(x, y, LevelObjType::PICKUP);
    draw_metatile_2_2(Nametable::A, x, y, Metatile::ITEM);
    update_attribute(x, y, BG_PALETTE_GREEN);
}

void load_level(uint8_t level_num) {
    level_offset = 0;
    auto player = objects[SLOT_PLAYER];
    uint8_t *current_level = (uint8_t*)SPLIT_ARRAY_POINTER(all_levels, level_num);
    while (true) {
        cmd = current_level[level_offset++];
        LevelObjType objtype = static_cast<LevelObjType>(cmd & 0x0f);
        switch (objtype) {
        case LevelObjType::TERMINATOR:
            return;
        case LevelObjType::TIMED_WALL:
        case LevelObjType::SOLID_WALL:
        case LevelObjType::HURT_WALL:
            create_wall(current_level, objtype);
            break;
        case LevelObjType::PICKUP: {
            uint8_t xy = current_level[level_offset++];
            draw_pickup((Point)xy);
            pickup_list[pickup_count++] = xy;
            break;
        }
        case LevelObjType::ENEMY: {
            auto [x, y] = read_pos(current_level);
            auto slot = find_obj_slot();
            auto enemy = objects[slot];
            enemy.type = ObjectType::PACE_ENEMY;
            enemy.x = x << 3;
            enemy.y = y << 3;
            
            break;
        }
        case LevelObjType::PLAYER: {
            auto [x, y] = read_pos(current_level);
            player.type = ObjectType::PLAYER;
            player.x = x << 3;
            player.y = y << 3;
            player.facing_dir = cmd >> 6;

            break;
        }
        case LevelObjType::CMD_MAIN:
            current_sub = 0;
            add_command(current_level);
            break;
        case LevelObjType::CMD_ONE:
            current_sub = 1;
            add_command(current_level);
            break;
        case LevelObjType::CMD_TWO:
            current_sub = 2;
            add_command(current_level);
            break;
        }
        flush_vram_update2();
    }
}