
#include <neslib.h>
#include <stdint.h>
#include <peekpoke.h>

#include "common.hpp"
#include "game.hpp"
#include "graphics.hpp"
#include "metatile.hpp"
#include "levels.hpp"


extern volatile __zeropage uint8_t VRAM_INDEX;

const uint8_t* all_levels[] = {
    LEVEL_0,
    LEVEL_1,
};

const uint8_t LEVEL_0[] = {
    C_MANY(LevelObjType::SOLID_WALL, C_VERTICAL, 4, 3, 0),
    C_MANY(LevelObjType::SOLID_WALL, C_VERTICAL, 4, 3, 5),
    // C_ONE(LevelObjType::PICKUP, 1, 6),
    // C_PLAYER(Facing::Left, 5, 3),
    C_END(),
};

static const uint8_t* __zp current_level;
static __zp uint8_t level_offset;
static __zp uint8_t cmd;

constexpr uint8_t LEVEL_X_POS = 10;
constexpr uint8_t LEVEL_Y_POS = 2;

static void create_timed_wall_obj([[maybe_unused]]uint8_t lendir, [[maybe_unused]]uint8_t x, [[maybe_unused]]uint8_t y) {

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
    if (cmd & C_MULTIPLE) {
        auto lendir = current_level[level_offset++];
        auto len = (uint8_t)(lendir & ~(C_VERTICAL));
        for (uint8_t i=0; i<len; i++) {
            draw_metatile_2_2(Nametable::A, x, y, metatile);
            if (lendir & C_VERTICAL) {
                y += 2;
            } else {
                x += 2;
            }
        }
        if (slot)
            create_timed_wall_obj(lendir, orig_x, orig_y);
    } else {
        draw_metatile_2_2(Nametable::A, x, y, metatile);
        if (slot)
            create_timed_wall_obj(1, orig_x, orig_y);
    }
}

void draw_level(uint8_t level_num) {
    current_level = all_levels[level_num];
    level_offset = 0;
    while (true) {
        cmd = current_level[level_offset++];
        switch (static_cast<LevelObjType>(static_cast<LevelObjId>(cmd).id)) {
        case LevelObjType::TERMINATOR:
            return;
        case LevelObjType::TIMED_WALL: {
            auto slot = load_object(ObjectType::TIMED_WALL);
            create_wall(slot);
            break;
        }
        case LevelObjType::SOLID_WALL: {
            create_wall(0);
            break;
        }
        case LevelObjType::PICKUP: {
            auto [x, y] = read_pos();
            draw_metatile_2_2(Nametable::A, x, y, Metatile::PICKUP);
            break;
        }
        case LevelObjType::ENEMY:
        case LevelObjType::PLAYER:{

            break;
        }
        }
        
        if (VRAM_INDEX >= 100) {
            ppu_wait_nmi();
        }
    }
}