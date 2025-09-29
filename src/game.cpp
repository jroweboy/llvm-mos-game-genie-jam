
#include <string.h>
#include <cstdint>
#include "fixed_point2.hpp"
#include <nesdoug.h>
#include <soa.h>

#include <neslib.h>

#include "game.hpp"
#include "common.hpp"
#include "levels.hpp"
#include "metatile.hpp"
#include "text_render.hpp"
#include "graphics.hpp"

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

uint8_t commands[12 + 9 + 9];
uint8_t command_index[3];
uint8_t pickup_list[8];
uint8_t current_sub;
bool is_twox_speed;


constinit FIXED const uint8_t sub_attr_y_lut[] = { 2, 12, 20 };

// soa::Array<Cursor, 2> cursors;


constinit FIXED static const uint8_t command_lower_bound_lut[] = { 0, 12, 12 + 9 };
constinit FIXED static const uint8_t command_upper_bound_lut[] = { 12, 12 + 9, 12 + 9 + 9 };

constinit FIXED const uint8_t cursor_command_lut[9] = {
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

constinit FIXED const Metatile command_metatile_lut[9] = {
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
constinit FIXED const uint8_t command_attr_lut[9] = {
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

extern const Letter WORD_TURN_LEFT[] = { LETTERS_7( "TURN L" ) };
extern const Letter WORD_MOVE[] = { LETTERS_5( "MOVE" ) };
extern const Letter WORD_TURN_RIGHT[] = { LETTERS_7( "TURN R" ) };
extern const Letter WORD_WAIT[] = { LETTERS_5( "WAIT" ) };
extern const Letter WORD_PICKUP[] = { LETTERS_5( "GRAB" ) };
extern const Letter WORD_RETURN[] = { LETTERS_6( "SPEED" ) };
extern const Letter WORD_JMP_1[] = { LETTERS_7( "CALL 1" ) };
extern const Letter WORD_JMP_2[] = { LETTERS_7( "CALL 2" ) };

SPLIT_ARRAY(command_strings,
    WORD_TURN_LEFT,
    WORD_MOVE,
    WORD_TURN_RIGHT,
    WORD_WAIT,
    WORD_WAIT,
    WORD_PICKUP,
    WORD_RETURN,
    WORD_JMP_1,
    WORD_JMP_2
);

constinit FIXED const uint8_t command_to_string_lut[9] = {
    3, // WAIT
    1, // MOVE
    0, // TURN LEFT
    2, // TURN RIGHT
    5, // PICKUP
    6, // RETURN
    7, // JUMP1
    8, // JUMP2
    3, // UNUSED WAIT
};

constinit FIXED const uint8_t command_position_lut[12 + 9 + 9] = {
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

void update_sub_attribute(uint8_t new_val) {
    auto old_sub = current_sub;
    if (new_val == 3)
        wrapped_inc(current_sub, 3);
    else
        current_sub = new_val;
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
    if (new_command == Command::CMD_RETURN) {
        is_twox_speed = !is_twox_speed;
        update_speed_setting();
        return;
    }
    auto& idx = command_index[current_sub];
    uint8_t lower_bound = command_lower_bound_lut[current_sub];
    uint8_t upper_bound = command_upper_bound_lut[current_sub];
    commands[idx] = new_command;
    draw_command();
    wrapped_inc(idx, upper_bound, lower_bound);
}

constexpr int8_t minimum_velocity = 2;
constexpr int8_t maximum_velocity = 8;

void move_object(uint8_t slot) {
    auto object = objects[slot];
    if (object->x.as_i() < object->target_x) {
        object.x = object->x + object->x_vel;
        if (object->x.as_i() > object->target_x)
            object.x = object->target_x;
    } else if (object->x.as_i() > object->target_x) {
        object.x = object->x + object->x_vel;
        if (object->x < object->target_x)
            object.x = object->target_x;
    }
    if (object->y.as_i() < object->target_y) {
        object.y = object->y + object->y_vel;
        if (object->y.as_i() > object->target_y)
            object.y = object->target_y;
    } else if (object.y->as_i() > object->target_y) {
        object.y = object->y + object->y_vel;
        if (object.y->as_i() < object->target_y)
            object.y = object->target_y;
    }
    if (object->x.as_i() == object->target_x && object->y.as_i() == object->target_y) {
        object.is_moving = false;
    }

    if (object->type == CURSOR) {
        if (object->x_vel.as_i() < -minimum_velocity)
            object.x_vel = MMAX((object->target_x - object->x.as_i()) >> 2, -maximum_velocity);
        else if (object->x_vel > minimum_velocity)
            object.x_vel = MMIN((object->target_x - object->x.as_i()) >> 2, maximum_velocity);
        
        if (object->y_vel.as_i() < -minimum_velocity)
            object.y_vel = MMAX((object->target_y - object->y.as_i()) >> 2, -maximum_velocity);
        else if (object->y_vel.as_i() >= minimum_velocity)
            object.y_vel = MMIN((object->target_y - object->y.as_i()) >> 2, maximum_velocity);
    }
}

// void move_cursor(uint8_t slot) {
//     auto cursor = cursors[slot];
//     if (cursor->x < cursor->target_x) {
//         cursor.x = cursor->x + cursor->x_vel;
//         if (cursor->x > cursor->target_x)
//             cursor.x = cursor->target_x;
//     } else if (cursor->x > cursor->target_x) {
//         cursor.x = cursor->x + cursor->x_vel;
//         if (cursor->x < cursor->target_x)
//             cursor.x = cursor->target_x;
//     }
//     if (cursor->y < cursor->target_y) {
//         cursor.y = cursor->y + cursor->y_vel;
//         if (cursor->y > cursor->target_y)
//             cursor.y = cursor->target_y;
//     } else if (cursor->y > cursor->target_y) {
//         cursor.y = cursor->y + cursor->y_vel;
//         if (cursor->y < cursor->target_y)
//             cursor.y = cursor->target_y;
//     }
//     if (cursor->x == cursor->target_x && cursor->y == cursor->target_y) {
//         cursor.is_moving = false;
//     }
    
//     if (cursor->x_vel < -minimum_velocity)
//         cursor.x_vel = MMAX((cursor->target_x - cursor->x) >> 2, -maximum_velocity);
//     else if (cursor->x_vel > minimum_velocity)
//         cursor.x_vel = MMIN((cursor->target_x - cursor->x) >> 2, maximum_velocity);
    
//     if (cursor->y_vel < -minimum_velocity)
//         cursor.y_vel = MMAX((cursor->target_y - cursor->y) >> 2, -maximum_velocity);
//     else if (cursor->y_vel >= minimum_velocity)
//         cursor.y_vel = MMIN((cursor->target_y - cursor->y) >> 2, maximum_velocity);
// }

constexpr uint8_t X_LO_BOUND = (10 * 8);
constexpr uint8_t X_HI_BOUND = (14 * 8);
constexpr uint8_t X_AVOID    = (12 * 8);
constexpr uint8_t Y_AVOID    = (24 * 8);
constexpr uint8_t Y_LO_BOUND = (22 * 8);
constexpr uint8_t Y_HI_BOUND = (26 * 8);

static void clear_command_string(bool halfclear) {
    // Clear the text.. Cheat and only clear the last 3 letters to save time
    auto idx = VRAM_INDEX;
    int ppuaddr_row0 = (halfclear)
        ? 0x2000 | (((uint8_t)Nametable::A) << 8) | ((22) << 5) | (24)
        : 0x2000 | (((uint8_t)Nametable::A) << 8) | ((22) << 5) | (18);
    int ppuaddr_row1 = ppuaddr_row0 + (1 << 5);
    int ppuaddr_row2 = ppuaddr_row1 + (1 << 5);
    VRAM_BUF[idx+ 0] = MSB(ppuaddr_row0) | NT_UPD_HORZ;
    VRAM_BUF[idx+ 4] = MSB(ppuaddr_row1) | NT_UPD_HORZ;
    VRAM_BUF[idx+ 8] = MSB(ppuaddr_row2) | NT_UPD_HORZ;
    VRAM_BUF[idx+ 1] = LSB(ppuaddr_row0);
    VRAM_BUF[idx+ 5] = LSB(ppuaddr_row1);
    VRAM_BUF[idx+ 9] = LSB(ppuaddr_row2);
    uint8_t length = halfclear ? 6 | NT_UPD_REPT : 12 | NT_UPD_REPT;
    VRAM_BUF[idx+ 2]  = length;
    VRAM_BUF[idx+ 6]  = length;
    VRAM_BUF[idx+ 10] = length;
    VRAM_BUF[idx+ 3] = 0;
    VRAM_BUF[idx+ 7] = 0;
    VRAM_BUF[idx+ 11] = 0;
    VRAM_BUF[idx+ 12] = 0xff;
    VRAM_INDEX += 12;
}

static void draw_command_string(uint8_t id) {
    clear_command_string(true);
    const Letter* command = (const Letter*)SPLIT_ARRAY_POINTER(command_strings, id);
    render_string(Nametable::A, 18, 22, command);
}

static void draw_command_string_main_cursor() {
    auto cursor = objects[1];
    uint8_t x = (cursor->x.as_i() - X_LO_BOUND) / 16;
    uint8_t y = (cursor->y.as_i() - Y_LO_BOUND) / 16;
    uint8_t str = x + y * 3;
    draw_command_string(str);
}

void set_cursor_target(uint8_t idx, Coord target) {
    auto cursor = objects[idx];
    cursor.is_moving = true;
    cursor.target_x = target.x;
    cursor.target_y = target.y;
    cursor.x_vel = (cursor->target_x - cursor->x.as_i()) / 2;
    cursor.y_vel = (cursor->target_y - cursor->y.as_i()) / 2;
    if (cursor->x_vel.as_i() < 0)
        cursor.x_vel = MMIN(cursor->x_vel.as_i(), -maximum_velocity);
    else
        cursor.x_vel = MMAX(cursor->x_vel.as_i(), maximum_velocity);
    if (cursor->y_vel.as_i() < 0)
        cursor.y_vel = MMIN(cursor->y_vel.as_i(), -maximum_velocity);
    else
        cursor.y_vel = MMAX(cursor->y_vel.as_i(), maximum_velocity);
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
    
    auto cursor = objects[1];
    // cursor.is_moving = false;
    // cursor.x = X_LO_BOUND;
    // cursor.y = Y_LO_BOUND;
    // cursor.height = 16;
    // cursor.width = 16;
    // cursor.param1 = 4;

    auto cmdcursor = objects[2];
    // cmdcursor.is_moving = false;
    // cmdcursor.param1 = 1;
    // cmdcursor.height = 16;
    // cmdcursor.width = 16;
    auto [cmd_x, cmd_y] = get_pos_from_index();
    cmdcursor.x = cmd_x;
    cmdcursor.y = cmd_y;


    bool prev_is_moving = false;
    draw_command_string_main_cursor();

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
                set_cursor_target(SLOT_CMDCURSOR, target);
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
                    set_cursor_target(SLOT_CMDCURSOR, target);
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
                    set_cursor_target(SLOT_MAINCURSOR, target);
                }
            }

            if (input & PAD_B) {
                move_cmd_cursor(-1);
                auto target = get_pos_from_index();
                set_cursor_target(SLOT_CMDCURSOR, target);
            }

            if (input & PAD_A) {
                uint8_t x = (cursor->x.as_i() - X_LO_BOUND) / 16;
                uint8_t y = (cursor->y.as_i() - Y_LO_BOUND) / 16;
                uint8_t idx = x + y * 3;
                update_command_list(cursor_command_lut[idx]);
                auto target = get_pos_from_index();
                set_cursor_target(SLOT_CMDCURSOR, target);
            }

            if (input & PAD_START) {
                set_game_mode(GameMode::MODE_EXECUTE);
                return;
            }
        } else {
            if (prev_is_moving != cursor->is_moving) {
                prev_is_moving = cursor->is_moving;
                draw_command_string_main_cursor();
            }
            move_object(SLOT_MAINCURSOR);
        }

        if (cmdcursor->is_moving)
            move_object(SLOT_CMDCURSOR);

        draw_player();
        draw_cursor(SLOT_MAINCURSOR);
        draw_cursor(SLOT_CMDCURSOR);

        if (cursor.timer != 0) {
            cursor.timer--;
        }
    }
}

constexpr fs8_8 MOVE_SPEED = 0.75_s8_8;
constexpr fs8_8 TWOX_MOVE_SPEED = 1.5_s8_8;
constinit FIXED const fs8_8 x_movement_velocity[4] = {
    0,
    MOVE_SPEED,
    0,
    -MOVE_SPEED,
};
constinit FIXED const fs8_8 y_movement_velocity[4] = {
    -MOVE_SPEED,
    0,
    MOVE_SPEED,
    0,
};
constinit FIXED const fs8_8 twox_movement_velocity[4] = {
    0,
    TWOX_MOVE_SPEED,
    0,
    -TWOX_MOVE_SPEED,
};
constinit FIXED const fs8_8 twoy_movement_velocity[4] = {
    -TWOX_MOVE_SPEED,
    0,
    TWOX_MOVE_SPEED,
    0,
};

constinit FIXED const int8_t x_movement[4] = {
    0,
    16,
    0,
    -16,
};
constinit FIXED const int8_t y_movement[4] = {
    -16,
    0,
    16,
    0,
};


static bool execute_action(uint8_t slot) {
    // execute the current command
    auto cmd = commands[command_index[current_sub]];
    auto obj = objects[slot];
    auto cmdcursor = objects[2];
    // DEBUGGER(cmd);
    switch ((Command)cmd) {
    case CMD_MOVE: {
        Coord target;
        target.x = obj->x.as_i() + 8 + x_movement[obj->facing_dir];
        target.y = obj->y.as_i() + 8 + y_movement[obj->facing_dir];
        if (target.x < 80 || target.x > 240 || target.y < 16 || target.y > 160) {
            // stay in bounds!
            break;
        }
        LevelObjType ttype = load_metatile_at_coord(target.x, target.y);
        if (ttype == LevelObjType::SOLID_WALL) {
            break;
        }
        obj.x_vel = (is_twox_speed)
            ? twox_movement_velocity[obj->facing_dir]
            : x_movement_velocity[obj->facing_dir];
        obj.y_vel = (is_twox_speed)
            ? twoy_movement_velocity[obj->facing_dir]
            : y_movement_velocity[obj->facing_dir];
        obj.target_x = target.x - 8;
        obj.target_y = target.y - 8;
        break;
    }
    case CMD_TURN_LEFT: {
        uint8_t val = obj.facing_dir;
        wrapped_dec(val, 0, 3);
        obj.facing_dir = val;
        break;
    }
    case CMD_TURN_RIGHT: {
        uint8_t val = obj.facing_dir;
        wrapped_inc(val, 3, 0);
        obj.facing_dir = val;
        break;
    }
    case CMD_JMP_ONE: {
        move_cmd_cursor(1);
        current_sub = 1;
        command_index[1] = 12;
        auto [x, y] = get_pos_from_index();
        cmdcursor.x = x;
        cmdcursor.y = y;
        break;
    }
    case CMD_JMP_TWO: {
        move_cmd_cursor(1);
        current_sub = 2;
        command_index[2] = 12 + 9;
        auto [x, y] = get_pos_from_index();
        cmdcursor.x = x;
        cmdcursor.y = y;
        break;
    }
    case CMD_PICKUP: {
        uint8_t px = obj->x.as_i() + 4;
        uint8_t py = obj->y.as_i() + 4;
        LevelObjType ttype = load_metatile_at_coord(px, py);
        if (ttype == LevelObjType::PICKUP) {
            pickup_count--;
            update_level_buff(px >> 3, py >> 3, LevelObjType::EMPTY);
            draw_metatile_2_2(Nametable::A, px >> 3, py >> 3, Metatile::BLANK);
        }
        if (pickup_count == 0) {
            return true;
        }
        break;
    }
    case CMD_RETURN:
    case CMD_END:
    case CMD_WAIT:
        break;
    }
    return false;
}

void game_mode_execute_main() {
    auto cmdcursor = objects[2];
    cmdcursor.is_moving = true;

    uint8_t original_objs[sizeof(Object) * 8];
    memcpy(original_objs, (void*)&objects, sizeof(Object) * 8);

    uint8_t original_sub = current_sub;
    uint8_t original_index[3] = { command_index[0], command_index[1], command_index[2] };

    current_sub = 0;
    command_index[0] = 0;
    command_index[1] = 0;
    command_index[2] = 0;
    
    // Move the cursor to the start
    auto target = get_pos_from_index();
    set_cursor_target(SLOT_CMDCURSOR, target);
    while (cmdcursor.is_moving) {
        ppu_wait_nmi();
        oam_clear();
        
        move_object(SLOT_CMDCURSOR);
        draw_cursor(SLOT_CMDCURSOR);
        draw_player();
    }

    clear_command_string(false);

    uint8_t base_frame_length = 30;
    if (is_twox_speed)
        base_frame_length = 15;
    delay(base_frame_length);

    // wait a bit before starting
    auto id = commands[command_index[current_sub]];
    draw_command_string(command_to_string_lut[id]);

    uint8_t frame_length = 1;

    while (true) {
        ppu_wait_nmi();
        pad_poll(0);
        oam_clear();

        auto input = get_pad_new(0);

        if (input & PAD_START) {
            break;
        }

        if (frame_length == 0) {
            uint8_t original_sub;
            do {
                original_sub = current_sub;
                if (execute_action(0)) {
                    // Level complete!
                    level++;
                    set_game_mode(MODE_LOAD_LEVEL);
                    return;
                }
            } while (original_sub != current_sub);
            frame_length = base_frame_length;
            auto id = commands[command_index[current_sub]];
            draw_command_string(command_to_string_lut[id]);

            // advance to the next command
            move_cmd_cursor(1);
            
            // check if its time to end the run.
            if (current_sub == 0 && command_index[0] == 0) {
                // end conditions
                break;
            } else if (command_index[current_sub] == command_lower_bound_lut[current_sub]) {
                // return to main? should be return to prev sub...
                // DEBUGGER();
                current_sub = 0;
            }
            
            auto target = get_pos_from_index();
            set_cursor_target(SLOT_CMDCURSOR, target);
        } else {
            if (cmdcursor->is_moving) {
                move_object(SLOT_CMDCURSOR);
            }
            move_object(0);
            frame_length--;
        }

        draw_cursor(SLOT_CMDCURSOR);
        draw_player();
    }


    current_sub = original_sub;
    command_index[0] = original_index[0];
    command_index[1] = original_index[1];
    command_index[2] = original_index[2];

    for (pickup_count=0; pickup_count<sizeof(pickup_list); ++pickup_count) {
        uint8_t p = pickup_list[pickup_count];
        if (p == 0xff) break;
        draw_pickup((Point)p);
        ppu_wait_nmi();
    }

    memcpy((void*)&objects, original_objs, sizeof(Object) * 8);
    set_game_mode(MODE_EDIT);
}