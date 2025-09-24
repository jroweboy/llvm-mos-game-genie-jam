
#include <cstdint>
#include "fixed_point2.hpp"
#include <nesdoug.h>
#include <soa.h>

#include <neslib.h>
#include <stdlib.h>

#include "game.hpp"
#include "common.hpp"
#include "metatile.hpp"
#include "text_render.hpp"

extern __zp unsigned char SPRID;
extern char OAM_BUF[256];

struct LevelCommand {
    uint8_t id;
    uint8_t x;
    uint8_t y;
};

#define SOA_STRUCT LevelCommand
#define SOA_MEMBERS MEMBER(x) MEMBER(y) MEMBER(id)
#include <soa-struct.inc>

#define OBJECT_COUNT 8
soa::Array<Object, OBJECT_COUNT> objects;

extern volatile uint8_t VRAM_BUF[128];
extern volatile __zeropage uint8_t VRAM_INDEX;

uint8_t commands[12 + 9 + 9];
uint8_t command_index[3];
uint8_t current_sub;

struct Coord {
    uint8_t x;
    uint8_t y;
};
#define SOA_STRUCT Coord
#define SOA_MEMBERS MEMBER(x) MEMBER(y)
#include <soa-struct.inc>

#define PACK(x, y) ((((x) & 0xf) << 4)|(((y) & 0xf)))
#define UNPACK(s) (Coord{.x=(uint8_t)(((s) & 0xf0) >> 4),.y=(uint8_t)(((s) & 0xf))})

constinit const uint8_t sub_attr_y_lut[] = { 2, 12, 20 };


struct Cursor {
    fu8_8 x;
    fu8_8 y;
    fs8_8 x_vel;
    fs8_8 y_vel;
    uint8_t target_x;
    uint8_t target_y;
    uint8_t timer;
    uint8_t param1;
    uint8_t width;
    uint8_t height;
    uint8_t expand_timer;
    uint8_t expansion;
    uint8_t prev_expansion;
    bool expand_direction;
    bool is_moving;
};

#define SOA_STRUCT Cursor
#define SOA_MEMBERS MEMBER(x) MEMBER(y) MEMBER(target_x) MEMBER(target_y) \
    MEMBER(x_vel) MEMBER(y_vel) MEMBER(timer) MEMBER(param1) MEMBER(width) MEMBER(height) \
    MEMBER(expand_timer) MEMBER(expansion) MEMBER(prev_expansion) \
    MEMBER(expand_direction) MEMBER(is_moving)
#include <soa-struct.inc>
soa::Array<Cursor, 2> cursors;


constinit static uint8_t cursor_expand_timer_lut[] = {
    12, 10, 8, 6, 6, 6, 8, 10
};

constinit static const uint8_t command_lower_bound_lut[] = { 0, 12, 12 + 9 };
constinit static const uint8_t command_upper_bound_lut[] = { 12, 12 + 9, 12 + 9 + 9 };

constinit const uint8_t cursor_command_lut[9] = {
    Command::CMD_TURN_LEFT,
    Command::CMD_MOVE,
    Command::CMD_TURN_RIGHT,
    Command::CMD_WAIT,
    Command::CMD_WAIT, // unused
    Command::CMD_PICKUP,
    Command::CMD_RETURN, // unused?? yield instead of return??
    Command::CMD_JMP_ONE,
    Command::CMD_JMP_TWO,
};

constinit const Metatile command_metatile_lut[9] = {
    Metatile::WAIT,
    Metatile::MOVE,
    Metatile::TURN_LEFT,
    Metatile::TURN_RIGHT,
    Metatile::PICKUP,
    Metatile::SUB, // unused?? yield instead of return??
    Metatile::ONE,
    Metatile::TWO,
    Metatile::WAIT, // unused
};
constinit const uint8_t command_attr_lut[9] = {
    BG_PALETTE_TAN, // Metatile::WAIT,
    BG_PALETTE_BLUE, // Metatile::MOVE,
    BG_PALETTE_GREEN, // Metatile::TURN_LEFT,
    BG_PALETTE_RED, // Metatile::TURN_RIGHT,
    BG_PALETTE_GREEN, // Metatile::PICKUP,
    BG_PALETTE_RED, // Metatile::SUB, // unused?? yield instead of return??
    BG_PALETTE_GREEN, // Metatile::ONE,
    BG_PALETTE_BLUE, // Metatile::TWO,
    BG_PALETTE_RED, // Metatile::WAIT, // unused
};

constinit const Letter* WORD_TURN_LEFT = "TURN L"_l;
constinit const Letter* WORD_MOVE = "MOVE"_l;
constinit const Letter* WORD_TURN_RIGHT = "TURN R"_l;
constinit const Letter* WORD_WAIT = "WAIT"_l;
constinit const Letter* WORD_PICKUP = "GRAB"_l;
constinit const Letter* WORD_RETURN = "YIELD"_l;
constinit const Letter* WORD_JMP_1 = "GOTO 1"_l;
constinit const Letter* WORD_JMP_2 = "GOTO 2"_l;
const Letter* command_strings[9] = {
    WORD_TURN_LEFT,
    WORD_MOVE,
    WORD_TURN_RIGHT,
    WORD_WAIT,
    WORD_WAIT,
    WORD_PICKUP,
    WORD_RETURN,
    WORD_JMP_1,
    WORD_JMP_2,
};

// uint8_t find_obj_slot(ObjectType t) {
//     return 1;
// }

constinit const uint8_t command_position_lut[12 + 9 + 9] {
    PACK(1, 2),PACK(2, 2),PACK(3, 2),
    PACK(1, 3),PACK(2, 3),PACK(3, 3),
    PACK(1, 4),PACK(2, 4),PACK(3, 4),
    PACK(1, 5),PACK(2, 5),PACK(3, 5),
    
    PACK(1, 7),PACK(2, 7),PACK(3, 7),
    PACK(1, 8),PACK(2, 8),PACK(3, 8),
    PACK(1, 9),PACK(2, 9),PACK(3, 9),
    
    PACK(1, 11),PACK(2, 11),PACK(3, 11),
    PACK(1, 12),PACK(2, 12),PACK(3, 12),
    PACK(1, 13),PACK(2, 13),PACK(3, 13),
};

Coord get_pos_from_index() {
    Coord t;
    auto idx = command_index[current_sub];
    auto [x, y] = UNPACK(command_position_lut[idx]);
    t.x = x << 4;
    t.y = y << 4;
    return t;
}

void update_sub_attribute() {
    auto old_sub = current_sub;
    wrapped_inc(current_sub, 3);
    update_attribute(2, sub_attr_y_lut[old_sub], BG_PALETTE_TAN);
    update_attribute(4, sub_attr_y_lut[old_sub], BG_PALETTE_TAN);
    update_attribute(6, sub_attr_y_lut[old_sub], BG_PALETTE_TAN);
    update_attribute(2, sub_attr_y_lut[current_sub], BG_PALETTE_BLUE);
    update_attribute(4, sub_attr_y_lut[current_sub], BG_PALETTE_BLUE);
    update_attribute(6, sub_attr_y_lut[current_sub], BG_PALETTE_BLUE);
}

static void draw_command() {
    auto idx = command_index[current_sub];
    auto cmd = commands[idx];

    auto mt = command_metatile_lut[cmd];
    auto attr = command_attr_lut[cmd];
    auto target = get_pos_from_index();
    draw_metatile_2_2(Nametable::A, target.x / 8, target.y / 8, mt);
    update_attribute(target.x / 8, target.y / 8, attr);
}

void update_command_list(uint8_t new_command) {
    auto& idx = command_index[current_sub];
    uint8_t lower_bound = command_lower_bound_lut[current_sub];
    uint8_t upper_bound = command_upper_bound_lut[current_sub];
    commands[idx] = new_command;
    draw_command();
    wrapped_inc(idx, upper_bound, lower_bound);
}
void draw_cursor(uint8_t slot) {
    auto cursor = cursors[slot];
    // param1 = longtimer
    // Run the cursor expando
    if (game_mode == GameMode::MODE_EDIT) {
        if (cursor.is_moving) {
            // if we are moving, contract in slightly
            // if (cursor_extra.expansion != (uint8_t)-4) {
            //     cursor_extra.prev_expansion = cursor_extra.expansion;
            // }
            // cursor.expansion = 0;
            // cursor.timer = 0;
            // cursor.param1 = 4;
            // cursor.expand_timer = 0;
            // cursor.expand_direction = false;
        } else if (cursor.timer == 0) {
            // if (cursor_extra.expansion == (uint8_t)-4) {
            //     cursor_extra.expansion = cursor_extra.prev_expansion;
            // }
            // other wise grow and shrink over time
            cursor.expand_timer++;
            cursor.expand_timer &= 7;
            cursor.timer = cursor_expand_timer_lut[cursor.expand_timer];
            cursor.expansion += cursor->expand_direction ? -1 : 1;
            cursor.param1 = cursor->param1 - 1;
            if (cursor->param1 == 0) {
                cursor.param1 = 4;
                cursor.expand_direction = !cursor->expand_direction;
            }
        }
    }
    // y, tile, attr, x
    OAM_BUF[SPRID+ 2] = 0x01;
    OAM_BUF[SPRID+ 6] = 0x01;
    OAM_BUF[SPRID+10] = 0x01;
    OAM_BUF[SPRID+14] = 0x01;

    OAM_BUF[SPRID+ 1] = 0x0d;
    OAM_BUF[SPRID+ 5] = 0x0e;
    OAM_BUF[SPRID+ 9] = 0x07;
    OAM_BUF[SPRID+13] = 0x0b;
    // OAM_BUF[SPRID+ 1] = 0x09;
    // OAM_BUF[SPRID+13] = 0x09;
    // OAM_BUF[SPRID+ 5] = 0x06;
    // OAM_BUF[SPRID+ 9] = 0x06;
    // top left
    OAM_BUF[SPRID+ 0] = cursor->y.as_i() - 4 - 1 - cursor->expansion;
    OAM_BUF[SPRID+ 3] = cursor->x.as_i() - 4 - cursor->expansion;
    // top right
    OAM_BUF[SPRID+ 4] = cursor->y.as_i() - 4 - 1 - cursor->expansion;
    OAM_BUF[SPRID+ 7] = cursor->x.as_i() - 4 + cursor->width + cursor->expansion;
    // bot left
    OAM_BUF[SPRID+ 8] = cursor->y.as_i() - 4 - 1 + cursor->height + cursor->expansion;
    OAM_BUF[SPRID+11] = cursor->x.as_i() - 4 - cursor->expansion;
    // bot right
    OAM_BUF[SPRID+12] = cursor->y.as_i() - 4 - 1 + cursor->height + cursor->expansion;
    OAM_BUF[SPRID+15] = cursor->x.as_i() - 4 + cursor->width + cursor->expansion;
    SPRID += 16;
}

constexpr fs8_8 minimum_velocity = 1.0_s8_8;

void move_cursor(uint8_t slot) {
    auto cursor = cursors[slot];
    if (cursor->x.as_i() < cursor->target_x) {
        cursor.x = cursor->x + cursor->x_vel;
        if (cursor->x.as_i() > cursor->target_x)
            cursor.x = cursor->target_x;
    } else if (cursor->x.as_i() > cursor->target_x) {
        cursor.x = cursor->x + cursor->x_vel;
        if (cursor->x < cursor->target_x)
            cursor.x = cursor->target_x;
    }
    if (cursor->y.as_i() < cursor->target_y) {
        cursor.y = cursor->y + cursor->y_vel;
        if (cursor->y.as_i() > cursor->target_y)
            cursor.y = cursor->target_y;
    } else if (cursor.y->as_i() > cursor->target_y) {
        cursor.y = cursor->y + cursor->y_vel;
        if (cursor.y->as_i() < cursor->target_y)
            cursor.y = cursor->target_y;
    }
    if (cursor->x.as_i() == cursor->target_x && cursor->y.as_i() == cursor->target_y) {
        cursor.is_moving = false;
    }
    
    if (cursor->x_vel < -minimum_velocity*2)
        cursor.x_vel = (cursor->target_x - cursor->x.as_i()) >> 2;
    else if (cursor->x_vel > minimum_velocity*2)
        cursor.x_vel = (cursor->target_x - cursor->x.as_i()) >> 2;
    
    if (cursor->y_vel < -minimum_velocity*2)
        cursor.y_vel = (cursor->target_y - cursor->y.as_i()) >> 2;
    else if (cursor->y_vel >= minimum_velocity*2)
        cursor.y_vel = (cursor->target_y - cursor->y.as_i()) >> 2;
}

void draw_sprites() {
    for (uint8_t i=0; i<OBJECT_COUNT; ++i) {
        auto obj = objects[i];
        if (obj->timer != 0) {
            obj.timer--;
        }
        switch (obj.type) {
        case PLAYER:
            break;
        case TIMED_WALL:
        case PACE_ENEMY:
            break;
        case NO_OBJECT:
            break;
        }
    }
}


constexpr uint8_t X_LO_BOUND = (10 * 8);
constexpr uint8_t X_HI_BOUND = (14 * 8);
constexpr uint8_t X_AVOID    = (12 * 8);
constexpr uint8_t Y_AVOID    = (24 * 8);
constexpr uint8_t Y_LO_BOUND = (22 * 8);
constexpr uint8_t Y_HI_BOUND = (26 * 8);

static void draw_command_string() {
    auto cursor = cursors[0];    
    uint8_t x = (cursor->x.as_i() - X_LO_BOUND) / 16;
    uint8_t y = (cursor->y.as_i() - Y_LO_BOUND) / 16;
    uint8_t str = x + y * 3;
    
    // Clear the text.. Cheat and only clear the last 3 letters to save time
    auto idx = VRAM_INDEX;
    int ppuaddr_row0 = 0x2000 | (((uint8_t)Nametable::A) << 8) | ((24) << 5) | (24);
    int ppuaddr_row1 = ppuaddr_row0 + (1 << 5);
    int ppuaddr_row2 = ppuaddr_row1 + (1 << 5);
    VRAM_BUF[idx+ 0] = MSB(ppuaddr_row0) | NT_UPD_HORZ;
    VRAM_BUF[idx+ 4] = MSB(ppuaddr_row1) | NT_UPD_HORZ;
    VRAM_BUF[idx+ 8] = MSB(ppuaddr_row2) | NT_UPD_HORZ;
    VRAM_BUF[idx+ 1] = LSB(ppuaddr_row0);
    VRAM_BUF[idx+ 5] = LSB(ppuaddr_row1);
    VRAM_BUF[idx+ 9] = LSB(ppuaddr_row2);
    VRAM_BUF[idx+ 2]  = 6 | NT_UPD_REPT;
    VRAM_BUF[idx+ 6]  = 6 | NT_UPD_REPT;
    VRAM_BUF[idx+ 10] = 6 | NT_UPD_REPT;
    VRAM_BUF[idx+ 3] = 0;
    VRAM_BUF[idx+ 7] = 0;
    VRAM_BUF[idx+ 11] = 0;
    VRAM_BUF[idx+ 12] = 0xff;

    VRAM_INDEX += 12;
    render_string(Nametable::A, 18, 24, command_strings[str]);
}

static void set_cursor_target(uint8_t idx, Coord target) {
    auto cursor = cursors[idx];
    cursor.is_moving = true;
    cursor.target_x = target.x;
    cursor.target_y = target.y;
    cursor.x_vel = (cursor->target_x - cursor->x.as_i()) / 2;
    cursor.y_vel = (cursor->target_y - cursor->y.as_i()) / 2;
}

static void move_cmd_cursor(uint8_t diff) {
    uint8_t& offset = command_index[current_sub];
    uint8_t lower_bound = command_lower_bound_lut[current_sub];
    uint8_t upper_bound = command_upper_bound_lut[current_sub];
    if (diff & 0x80) {
        wrapped_sub(offset, ~diff+1, lower_bound, upper_bound);
    } else {
        wrapped_add(offset, diff, lower_bound-1, upper_bound-1);
    }
}

void game_mode_edit_main() {
    uint8_t input_buffer[8] = { 0 };
    uint8_t read_ptr = 0;
    uint8_t write_ptr = 0;
    
    auto cursor = cursors[0];
    cursor.is_moving = false;
    cursor.x = X_LO_BOUND;
    cursor.y = Y_LO_BOUND;
    cursor.height = 16;
    cursor.width = 16;
    cursor.param1 = 4;

    auto cmdcursor = cursors[1];
    cmdcursor.is_moving = false;
    cmdcursor.param1 = 1;
    cmdcursor.height = 16;
    cmdcursor.width = 16;
    auto [cmd_x, cmd_y] =get_pos_from_index();
    cmdcursor.x = cmd_x;
    cmdcursor.y = cmd_y;


    bool prev_is_moving = false;
    draw_command_string();

    while (true) {
        ppu_wait_nmi();
        pad_poll(0);
        oam_clear();

        // Update cursor
        auto latest_input = get_pad_new(0);

        // If the cursor is moving and the player wants to do something, then buffer it
        // till the movement stops.
        if (latest_input) {
            input_buffer[write_ptr++] = latest_input;
            write_ptr &= sizeof(input_buffer) - 1;
        }

        // If we have input and we aren't currently moving the cursor
        if (!cursor.is_moving && read_ptr != write_ptr) {
            auto input = input_buffer[read_ptr++];
            read_ptr &= sizeof(input_buffer) - 1;

            // controls: b to "go back" in the current sub
            // select to change which sub you are editing
            // directions to move the cursor.
            // start to start the game play

            if (input & PAD_SELECT) {
                update_sub_attribute();
                auto target = get_pos_from_index();
                set_cursor_target(1, target);
            }
            // Calculate new cursor position
            if (input & (PAD_UP | PAD_DOWN | PAD_LEFT | PAD_RIGHT)) {
                // Bit jank, but while they are holding B if you push a direction,
                // it should move the cursor instead.
                auto current = pad_state(0);
                if (current & PAD_B) {
                    if (input & PAD_UP) {
                        move_cmd_cursor(-3);
                    } else if (input & PAD_DOWN) {
                        move_cmd_cursor(3);
                    }
                    if (input & PAD_LEFT) {
                        move_cmd_cursor(-1);
                    } else if (input & PAD_RIGHT) {
                        move_cmd_cursor(1);
                    }
                    auto target = get_pos_from_index();
                    set_cursor_target(1, target);
                } else {
                    // Move the cursor for the selections
                    prev_is_moving = false;

                    uint8_t orig_x = cursor->x.as_i();
                    uint8_t orig_y = cursor->y.as_i();

                    Coord target = {.x = orig_x, .y = orig_y};

                    // apply the movement for the current tiles
                    if (input & PAD_UP) {
                        wrapped_sub(target.y, 16, Y_LO_BOUND, Y_HI_BOUND+16);
                    } else if (input & PAD_DOWN) {
                        wrapped_add(target.y, 16, Y_LO_BOUND-16, Y_HI_BOUND);
                    }
                    if (input & PAD_LEFT) {
                        wrapped_sub(target.x, 16, X_LO_BOUND, X_HI_BOUND+16);
                    } else if (input & PAD_RIGHT) {
                        wrapped_add(target.x, 16, X_LO_BOUND-16, X_HI_BOUND);
                    }

                    if (target.x == X_AVOID && target.y == Y_AVOID) {
                        if (input & PAD_UP) {
                            target.y -= 16;
                        } else if (input & PAD_DOWN) {
                            target.y += 16;
                        }
                        if (input & PAD_LEFT) {
                            target.x -= 16;
                        } else if (input & PAD_RIGHT) {
                            target.x += 16;
                        }
                    }
                    set_cursor_target(0, target);
                }
            }

            if (input & PAD_B) {
                move_cmd_cursor(-1);
                auto target = get_pos_from_index();
                set_cursor_target(1, target);
            }

            if (input & PAD_A) {
                uint8_t x = (cursor->x.as_i() - X_LO_BOUND) / 16;
                uint8_t y = (cursor->y.as_i() - Y_LO_BOUND) / 16;
                uint8_t idx = x + y * 3;
                update_command_list(cursor_command_lut[idx]);
                auto target = get_pos_from_index();
                set_cursor_target(1, target);
            }

            if (input & PAD_START) {
                set_game_mode(GameMode::MODE_EXECUTE);
                return;
            }
        } else {
            if (prev_is_moving != cursor->is_moving) {
                prev_is_moving = cursor->is_moving;
                draw_command_string();
            }
            move_cursor(0);
        }

        if (cmdcursor->is_moving)
            move_cursor(1);
        draw_sprites();
        draw_cursor(0);
        draw_cursor(1);

        if (cursor.timer != 0) {
            cursor.timer--;
        }
    }
}
