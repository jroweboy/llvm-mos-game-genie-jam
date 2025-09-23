
#include <cstdint>
#include "fixed_point2.hpp"
#include <nesdoug.h>
#include <soa.h>

#include <neslib.h>
#include <stdlib.h>

#include "game.hpp"
#include "common.hpp"
#include "metatile.hpp"

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

uint8_t main_commands[12];
uint8_t one_commands[9];
uint8_t two_commands[9];
uint8_t main_command_index;
uint8_t one_command_index;
uint8_t two_command_index;
uint8_t current_sub;

struct Coord {
    uint8_t x;
    uint8_t y;
};

#define PACK(x, y) ((((x) & 0xf) << 4)|(((y) & 0xf)))
#define UNPACK(s) (Coord{.x=(uint8_t)(((s) & 0xf0) >> 4),.y=(uint8_t)(((s) & 0xf))})

constinit const uint8_t sub_attr_y_lut[] = { 2, 12, 20 };


struct Cursor {
    uint8_t target_x;
    uint8_t target_y;
    fu8_8 x;
    fu8_8 y;
    fs8_8 x_vel;
    fs8_8 y_vel;
    uint8_t timer;
    uint8_t param1;
    uint8_t height;
    uint8_t expand_timer;
    uint8_t expansion;
    uint8_t prev_expansion;
    bool expand_direction;
    bool is_moving;
};

static Cursor cursor;
// static Cursor command_cursor;

constinit static uint8_t cursor_expand_timer_lut[] = {
    12, 10, 8, 6, 6, 6, 8, 10
};


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

// constinit const Metatile command_metatile_lut[9] = {
//     Metatile::TURN_LEFT,
//     Metatile::MOVE,
//     Metatile::TURN_RIGHT,
//     Metatile::WAIT,
//     Metatile::WAIT, // unused
//     Metatile::PICKUP,
//     Metatile::SUB, // unused?? yield instead of return??
//     Metatile::ONE,
//     Metatile::TWO,
// };


uint8_t find_obj_slot(ObjectType t) {
    if (t == ObjectType::PLAYER) {
        return 0;
    } else {
        return 1;
    }
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



void update_command_list(uint8_t new_command) {
    switch (current_sub) {
    default:
    case 0:
        main_commands[main_command_index] = new_command;
        // command_metatile_lut[new_command];
        wrapped_inc(main_command_index, 12);
        break;
    case 1:
        one_commands[one_command_index] = new_command;
        wrapped_inc(one_command_index, 9);
        break;
    case 2:
        two_commands[two_command_index] = new_command;
        wrapped_inc(two_command_index, 9);
        break;
    }
}
void draw_cursor() {
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
            cursor.timer = cursor_expand_timer_lut[cursor.expand_timer++];
            cursor.expand_timer &= 7;
            cursor.expansion += cursor.expand_direction ? -1 : 1;
            cursor.param1 = cursor.param1 - 1;
            if (cursor.param1 == 0) {
                cursor.param1 = 4;
                cursor.expand_direction = !cursor.expand_direction;
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
    OAM_BUF[SPRID+ 0] = cursor.y.as_i() - 4 - cursor.expansion;
    OAM_BUF[SPRID+ 3] = cursor.x.as_i() - 4 - cursor.expansion;
    // top right
    OAM_BUF[SPRID+ 4] = cursor.y.as_i() - 4 - cursor.expansion;
    OAM_BUF[SPRID+ 7] = cursor.x.as_i() - 4 + 16 + cursor.expansion;
    // bot left
    OAM_BUF[SPRID+ 8] = cursor.y.as_i() - 4 + cursor.height + cursor.expansion;
    OAM_BUF[SPRID+11] = cursor.x.as_i() - 4 - cursor.expansion;
    // bot right
    OAM_BUF[SPRID+12] = cursor.y.as_i() - 4 + cursor.height + cursor.expansion;
    OAM_BUF[SPRID+15] = cursor.x.as_i() - 4 + 16 + cursor.expansion;
    SPRID += 16;
}

constexpr fs8_8 minimum_velocity = 1.0_s8_8;

void move_cursor() {
    if (cursor.x.as_i() < cursor.target_x) {
        cursor.x += cursor.x_vel;
        if (cursor.x.as_i() > cursor.target_x)
            cursor.x = cursor.target_x;
    } else if (cursor.x.as_i() > cursor.target_x) {
        cursor.x += cursor.x_vel;
        if (cursor.x < cursor.target_x)
            cursor.x = cursor.target_x;
    }
    if (cursor.y.as_i() < cursor.target_y) {
        cursor.y += cursor.y_vel;
        if (cursor.y.as_i() > cursor.target_y)
            cursor.y = cursor.target_y;
    } else if (cursor.y.as_i() > cursor.target_y) {
        cursor.y += cursor.y_vel;
        if (cursor.y.as_i() < cursor.target_y)
            cursor.y = cursor.target_y;
    }
    if (cursor.x.as_i() == cursor.target_x && cursor.y.as_i() == cursor.target_y) {
        cursor.is_moving = false;
    }
    
    if (cursor.x_vel < -minimum_velocity*2)
        cursor.x_vel = (cursor.target_x - cursor.x.as_i()) >> 2;
    else if (cursor.x_vel > minimum_velocity*2)
        cursor.x_vel = (cursor.target_x - cursor.x.as_i()) >> 2;
    
    if (cursor.y_vel < -minimum_velocity*2)
        cursor.y_vel = (cursor.target_y - cursor.y.as_i()) >> 2;
    else if (cursor.y_vel >= minimum_velocity*2)
        cursor.y_vel = (cursor.target_y - cursor.y.as_i()) >> 2;
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
void game_mode_edit_main() {
    uint8_t input_buffer[8] = { 0 };
    uint8_t read_ptr = 0;
    uint8_t write_ptr = 0;

    constexpr uint8_t X_LO_BOUND = (10 * 8);
    constexpr uint8_t X_HI_BOUND = (14 * 8);
    constexpr uint8_t X_AVOID    = (12 * 8);
    constexpr uint8_t Y_AVOID    = (24 * 8);
    constexpr uint8_t Y_LO_BOUND = (22 * 8);
    constexpr uint8_t Y_HI_BOUND = (26 * 8);
    
    cursor.is_moving = false;
    cursor.x = X_LO_BOUND;
    cursor.y = Y_LO_BOUND;
    cursor.height = 16;
    cursor.param1 = 4;

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
            }
            // Calculate new cursor position
            if (input & (PAD_UP | PAD_DOWN | PAD_LEFT | PAD_RIGHT)) {
                cursor.is_moving = true;

                uint8_t orig_x = cursor.x.as_i();
                uint8_t orig_y = cursor.y.as_i();

                Coord target = {.x = orig_x, .y = orig_y};

                // apply the movement for the current tiles
                if (input & PAD_UP) {
                    wrapped_sub(target.y, 16, Y_LO_BOUND-1, Y_HI_BOUND);
                } else if (input & PAD_DOWN) {
                    wrapped_add(target.y, 16, Y_HI_BOUND+1, Y_LO_BOUND);
                }
                if (input & PAD_LEFT) {
                    wrapped_sub(target.x, 16, X_LO_BOUND-1, X_HI_BOUND);
                } else if (input & PAD_RIGHT) {
                    wrapped_add(target.x, 16, X_HI_BOUND+1, X_LO_BOUND);
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
                cursor.target_x = target.x;
                cursor.target_y = target.y;
                cursor.x_vel = (cursor.target_x - cursor.x.as_i()) / 2;
                cursor.y_vel = (cursor.target_y - cursor.y.as_i()) / 2;
            }

            if (input & PAD_B) {
                switch (current_sub) {
                default:
                case 0:
                    wrapped_dec(main_command_index, 0, 12);
                    break;
                case 1:
                    wrapped_dec(one_command_index, 0, 9);
                    break;
                case 2:
                    wrapped_dec(two_command_index, 0, 9);
                    break;
                }
            }

            if (input & PAD_A) {
                uint8_t x = (cursor.x.as_i() - X_LO_BOUND) / 16;
                uint8_t y = (cursor.y.as_i() - Y_LO_BOUND) / 16;
                uint8_t idx = x + y * 3;
                update_command_list(cursor_command_lut[idx]);
            }

            if (input & PAD_START) {
                set_game_mode(GameMode::MODE_EXECUTE);
                return;
            }
        } else {
            move_cursor();
        }

        draw_sprites();
        draw_cursor();

        if (cursor.timer != 0) {
            cursor.timer--;
        }
    }
}
