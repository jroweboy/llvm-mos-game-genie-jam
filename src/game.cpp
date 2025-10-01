
#include <string.h>
#include <cstdint>
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
uint8_t pickup_list[50];
uint8_t current_sub;
uint8_t speed_setting;
static bool finished_level;


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
extern const Letter WORD_ERROR[] = { LETTERS_6( "ERROR" ) };
extern const Letter WORD_YIELD[] = { LETTERS_6( "YIELD" ) };

SPLIT_ARRAY(command_strings,
    WORD_TURN_LEFT,
    WORD_MOVE,
    WORD_TURN_RIGHT,
    WORD_WAIT,
    WORD_WAIT,
    WORD_PICKUP,
    WORD_RETURN,
    WORD_JMP_1,
    WORD_JMP_2,
    WORD_ERROR,
    WORD_YIELD
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

__attribute__((noinline)) static void draw_sprites() {
    draw_player();
    draw_cursor(SLOT_MAINCURSOR);
    draw_cursor(SLOT_CMDCURSOR);
}

__attribute__((noinline)) static void wait_for_flush() {
    draw_sprites();
    ppu_wait_nmi();
    oam_clear();
}

static void level_completed() {
    // Level complete!
    level++;
    finished_level = true;
    pal_fade(false);
    set_game_mode(MODE_LOAD_LEVEL);
}

void update_sub_attribute(uint8_t new_val) {
    auto old_sub = current_sub;
    if (new_val == 3)
        wrapped_inc(current_sub, 3);
    else
        current_sub = new_val;
    // If we are full, just lag a frame to prevent overflowing the buffer
    if (VRAM_INDEX > 0x30) {
        wait_for_flush();
    }
    for (uint8_t x = 2; x < 8; x += 2) {
        update_attribute(x, sub_attr_y_lut[old_sub], BG_PALETTE_TAN);
        update_attribute(x, sub_attr_y_lut[current_sub], BG_PALETTE_BLUE);
    }
    // update_attribute(4, sub_attr_y_lut[old_sub], BG_PALETTE_TAN);
    // update_attribute(6, sub_attr_y_lut[old_sub], BG_PALETTE_TAN);
    // update_attribute(4, sub_attr_y_lut[current_sub], BG_PALETTE_BLUE);
    // update_attribute(6, sub_attr_y_lut[current_sub], BG_PALETTE_BLUE);
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
        wrapped_inc(speed_setting, 3);
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
    if (object->x < object->target_x) {
        object.x = object->x + object->x_vel;
        if (object->x > object->target_x)
            object.x = object->target_x;
    } else if (object->x > object->target_x) {
        object.x = object->x + object->x_vel;
        if (object->x < object->target_x)
            object.x = object->target_x;
    }
    if (object->y < object->target_y) {
        object.y = object->y + object->y_vel;
        if (object->y > object->target_y)
            object.y = object->target_y;
    } else if (object->y > object->target_y) {
        object.y = object->y + object->y_vel;
        if (object->y < object->target_y)
            object.y = object->target_y;
    }
    if (object->x == object->target_x && object->y == object->target_y) {
        object.is_moving = false;
    }

    if (object->type == CURSOR) {
        if (object->x_vel < -minimum_velocity)
            object.x_vel = MMAX((object->target_x - object->x) >> 2, -maximum_velocity);
        else if (object->x_vel > minimum_velocity)
            object.x_vel = MMIN((object->target_x - object->x) >> 2, maximum_velocity);
        
        if (object->y_vel < -minimum_velocity)
            object.y_vel = MMAX((object->target_y - object->y) >> 2, -maximum_velocity);
        else if (object->y_vel >= minimum_velocity)
            object.y_vel = MMIN((object->target_y - object->y) >> 2, maximum_velocity);
    }
}

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
    // check if we are trying to jump to the current sub
    // then its a yield statement
    if ((current_sub == 1 && id == CMD_JMP_ONE+1)
        || (current_sub == 2 && id == CMD_JMP_TWO+1)) {
        id = CMD_YIELD+1;
    } else if (current_sub == 2 && id == CMD_JMP_ONE+1) {
        id = CMD_ERROR+1;
    }
        // uint8_t subcmd = current_sub + CMD_JMP_ONE+1;
        // if (subcmd == id) {
        //     id = CMD_YIELD+1;
        // } else if (subcmd == id - 1) {
        //     // if we try to jump from 2 to 1 then show error
        //     id = CMD_ERROR+1;
        // }
    const Letter* command = (const Letter*)SPLIT_ARRAY_POINTER(command_strings, id);
    render_string(Nametable::A, 18, 22, command);
}

static void draw_command_string_main_cursor() {
    auto cursor = objects[1];
    uint8_t x = (cursor->x - X_LO_BOUND) / 16;
    uint8_t y = (cursor->y - Y_LO_BOUND) / 16;
    uint8_t str = x + y * 3;
    draw_command_string(str);
}

void set_cursor_target(uint8_t idx, Coord target) {
    auto cursor = objects[idx];
    cursor.is_moving = true;
    cursor.target_x = target.x;
    cursor.target_y = target.y;
    cursor.x_vel = (cursor->target_x - cursor->x) / 2;
    cursor.y_vel = (cursor->target_y - cursor->y) / 2;
    if (cursor->x_vel < 0)
        cursor.x_vel = MMIN(cursor->x_vel, -maximum_velocity);
    else
        cursor.x_vel = MMAX(cursor->x_vel, maximum_velocity);
    if (cursor->y_vel < 0)
        cursor.y_vel = MMIN(cursor->y_vel, -maximum_velocity);
    else
        cursor.y_vel = MMAX(cursor->y_vel, maximum_velocity);
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


constinit static const uint8_t timed_wall_attrs[4] = {
    BG_PALETTE_GREEN, BG_PALETTE_GREEN,
    BG_PALETTE_BLUE,
    BG_PALETTE_RED
};
constinit static const LevelObjType timed_wall_type[4] = {
    LevelObjType::TIMED_WALL, LevelObjType::TIMED_WALL, LevelObjType::TIMED_WALL,
    LevelObjType::HURT_WALL
};

static void run_object_continue_frame([[maybe_unused]] uint8_t slot) {
    // DEBUGGER(slot);
}

static void update_wall_attr(uint8_t slot) {
    auto object = objects[slot];
    uint8_t lendir = object->facing_dir;
    uint8_t len = lendir & 0x7f;
    uint8_t x = object->x;
    uint8_t y = object->y;
    uint8_t timer = object->param2;
    for (uint8_t i=0; i<len; i++) {
        update_level_buff(x, y, timed_wall_type[timer]);
        update_attribute(x, y, timed_wall_attrs[timer], false);
        
        if (lendir & L_VERTICAL) {
            y += 2;
        } else {
            x += 2;
        }
    }
    // lag a frame cuz why not
    write_all_attributes();
    wait_for_flush();
}

static void reset_object(uint8_t slot) {
    auto object = objects[slot];
    switch (object->type) {
    case TIMED_WALL: {
        object.param2 = 0;
        update_wall_attr(slot);
        // uint8_t lendir = object->facing_dir;
        // uint8_t len = lendir & 0x7f;
        // uint8_t x = object->x;
        // uint8_t y = object->y;
        // for (uint8_t i=0; i<len; i++) {
        //     update_level_buff(x, y, timed_wall_type[0]);
        //     update_attribute(x, y, timed_wall_attrs[0], false);
            
        //     if (lendir & L_VERTICAL) {
        //         y += 2;
        //     } else {
        //         x += 2;
        //     }
        // }
        // lag a frame cuz why not
        write_all_attributes();
        wait_for_flush();
        break;
    }
    case PACE_ENEMY:
        break;
    case NO_OBJECT:
    case PLAYER:
    case CURSOR:
        break;
    }
}

constinit const static uint8_t KONAMI_CODE[10] = {
    PAD_UP, PAD_UP, PAD_DOWN, PAD_DOWN,
    PAD_LEFT, PAD_RIGHT, PAD_LEFT, PAD_RIGHT,
    PAD_B, PAD_A
};

static bool prev_is_moving;
static uint8_t input_buffer[8];
static uint8_t latest_inputs[11];
static uint8_t cheat_write_ptr;
static uint8_t read_ptr;
static uint8_t write_ptr;

__attribute__((noinline)) static void handle_edit_main_mode() {
    auto cursor = objects[1];
    
    // Update cursor
    auto latest_input = get_pad_new(0);

    // If the cursor is moving and the player wants to do something, then buffer it
    // till the movement stops.
    if (latest_input) {
        latest_inputs[cheat_write_ptr] = latest_input;
        input_buffer[write_ptr++] = latest_input;
        write_ptr &= sizeof(input_buffer) - 1;
        wrapped_inc(cheat_write_ptr, 11);
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
            wait_for_flush();
            draw_command_string_main_cursor();
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

                uint8_t orig_x = cursor->x;
                uint8_t orig_y = cursor->y;

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
            uint8_t x = (cursor->x - X_LO_BOUND) / 16;
            uint8_t y = (cursor->y - Y_LO_BOUND) / 16;
            uint8_t idx = x + y * 3;
            // don't let sub 2 jump to sub 1!
            if (!(idx == CMD_JMP_ONE+1 && current_sub == 2)) {
                update_command_list(cursor_command_lut[idx]);
                auto target = get_pos_from_index();
                set_cursor_target(SLOT_CMDCURSOR, target);
            }
        }

        if (input & PAD_START) {
            uint8_t read = cheat_write_ptr;
            uint8_t i = 0;
            while (latest_inputs[read] == KONAMI_CODE[i++] && i <= 10)
                wrapped_inc(read, 11);
            if (i >= 10) {
                // konami code activated! skip this level
                level_completed();
                return;
            }
            draw_sprites();
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
}

void game_mode_edit_main() {
    prev_is_moving = 0;
    cheat_write_ptr = 0;
    read_ptr = 0;
    write_ptr = 0;
    memset(input_buffer, 0, sizeof(input_buffer));
    memset(latest_inputs, 0, sizeof(latest_inputs));

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

    for (uint8_t i=3; i<10; i++) {
        reset_object(i);
    }

    draw_command_string_main_cursor();

    while (true) {
        ppu_wait_nmi();
        if (game_mode != MODE_EDIT) {
            return;
        }
        pad_poll(0);
        oam_clear();

        handle_edit_main_mode();

        if (cmdcursor->is_moving)
            move_object(SLOT_CMDCURSOR);

        draw_sprites();

        if (cursor.timer != 0) {
            cursor.timer--;
        }
    }
}

constexpr int8_t MOVE_SPEED = 1;
constinit FIXED const int8_t x_movement_velocity[12] = {
    0, MOVE_SPEED, 0, -MOVE_SPEED,
    0, 2 * MOVE_SPEED, 0, 2 * -MOVE_SPEED,
    0, 4 * MOVE_SPEED, 0, 4 * -MOVE_SPEED,
};
constinit FIXED const int8_t y_movement_velocity[12] = {
    -MOVE_SPEED, 0, MOVE_SPEED, 0,
    2 * -MOVE_SPEED, 0, 2 * MOVE_SPEED, 0,
    4 * -MOVE_SPEED, 0, 4 * MOVE_SPEED, 0,
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

static bool player_died;
static bool yielded_one;
static bool yielded_two;
static uint8_t previous_sub;

static bool execute_action(uint8_t slot) {
    // execute the current command
    auto cmd = commands[command_index[current_sub]];
    auto obj = objects[slot];
    auto cmdcursor = objects[2];
    switch ((Command)cmd) {
    case CMD_MOVE: {
        Coord target;
        target.x = obj->x + 8 + x_movement[obj->facing_dir];
        target.y = obj->y + 8 + y_movement[obj->facing_dir];
        if (target.x < 80 || target.x > 240 || target.y < 16 || target.y > 160) {
            // stay in bounds!
            break;
        }
        LevelObjType ttype = load_metatile_at_coord(target.x, target.y);
        if (ttype == LevelObjType::SOLID_WALL) {
            break;
        }
        if (ttype == LevelObjType::HURT_WALL) {
            // player died to the wall
            player_died = true;
            break;
        }
        uint8_t offset = obj->facing_dir + 4 * speed_setting;
        obj.x_vel = x_movement_velocity[offset];
        obj.y_vel = y_movement_velocity[offset];
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
        wrapped_inc(val, 4, 0);
        obj.facing_dir = val;
        break;
    }
    case CMD_JMP_ONE: {
        move_cmd_cursor(1);
        uint8_t next_sub;
        if (current_sub == 1) {
            next_sub = 0;
            yielded_one = true;
        } else {
            next_sub = 1;
            if (!yielded_one) {
                command_index[1] = 12;
            } else {
                yielded_one = false;
            }
        }
        update_sub_attribute(next_sub);
        current_sub = next_sub;
        auto [x, y] = get_pos_from_index();
        cmdcursor.x = x;
        cmdcursor.y = y;
        wait_for_flush();
        break;
    }
    case CMD_JMP_TWO: {
        move_cmd_cursor(1);
        uint8_t next_sub;
        if (current_sub == 2) {
            next_sub = previous_sub;
            yielded_two = true;
        } else {
            previous_sub = current_sub;
            next_sub = 2;
            if (!yielded_two) {
                command_index[2] = 12 + 9;
            } else {
                yielded_two = false;
            }
        }
        update_sub_attribute(next_sub);
        current_sub = next_sub;
        auto [x, y] = get_pos_from_index();
        cmdcursor.x = x;
        cmdcursor.y = y;
        wait_for_flush();
        break;
    }
    case CMD_PICKUP: {
        uint8_t px = obj->x + 4;
        uint8_t py = obj->y + 4;
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
    case CMD_ERROR:
    case CMD_YIELD:
        break;
    }
    return false;
}


static void run_object_new_frame(uint8_t slot) {
    auto object = objects[slot];
    switch (object->type) {
    case TIMED_WALL: {
        uint8_t timer = object.param2;
        wrapped_inc(timer, 4);
        object.param2 = timer;
        update_wall_attr(slot);
        // uint8_t lendir = object->facing_dir;
        // uint8_t len = lendir & 0x7f;
        // uint8_t x = object->x;
        // uint8_t y = object->y;
        break;
    }
    case PACE_ENEMY:
        break;
    case NO_OBJECT:
    case PLAYER:
    case CURSOR:
        break;
    }
}

static bool finished_execute;
static uint8_t frame_length;

__attribute__((noinline)) static void handle_execute_main_mode(uint8_t base_frame_length) {
    auto cmdcursor = objects[2];
    auto input = get_pad_new(0);

    if (input & PAD_START) {
        finished_execute = true;
        return;
    }

    if (frame_length == 0) {
        if (player_died) {
            finished_execute = true;
            return;
        }
        // Run other objects before player
        for (uint8_t i=3; i<10; i++) {
            run_object_new_frame(i);
        }

        uint8_t original_sub;
        do {
            original_sub = current_sub;
            if (execute_action(0)) {
                // Level complete!
                level_completed();
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
            return;
        } else if (command_index[current_sub] == command_lower_bound_lut[current_sub]) {
            // work around an issue when returning from sub 2 -> 1 and the jmp command
            // is the last instruction of sub 1
            if (current_sub == 2 && command_index[1] == 12) {
                previous_sub = 0;
            }
            update_sub_attribute(previous_sub);
            previous_sub = 0;
            wait_for_flush();
        }
        
        auto target = get_pos_from_index();
        set_cursor_target(SLOT_CMDCURSOR, target);
    } else {
        for (uint8_t i=3; i<10; i++) {
            run_object_continue_frame(i);
        }
        if (cmdcursor->is_moving) {
            move_object(SLOT_CMDCURSOR);
        }
        move_object(SLOT_PLAYER);
        frame_length--;
    }

};

constinit const uint8_t base_frame_pacing_lut[3] = {
    30, 15, 7
};
void game_mode_execute_main() {
    yielded_one = false;
    yielded_two = false;
    finished_execute = false;
    finished_level = false;
    previous_sub = 0;
    auto cursor = objects[SLOT_MAINCURSOR];
    cursor.frame = 1;
    auto cmdcursor = objects[SLOT_CMDCURSOR];
    cmdcursor.is_moving = true;

    uint8_t original_objs[sizeof(Object) * 8];
    memcpy(original_objs, (void*)&objects, sizeof(Object) * 8);

    uint8_t original_sub = current_sub;
    uint8_t original_index[3] = { command_index[0], command_index[1], command_index[2] };

    update_sub_attribute(0);
    command_index[0] = 0;
    command_index[1] = 0;
    command_index[2] = 0;
    
    // Move the cursor to the start
    auto target = get_pos_from_index();
    set_cursor_target(SLOT_CMDCURSOR, target);
    while (cmdcursor.is_moving) {
        oam_clear();
        move_object(SLOT_CMDCURSOR);
        wait_for_flush();
        
        // draw_cursor(SLOT_CMDCURSOR);
        // draw_player();
    }

    clear_command_string(false);


    // wait a bit before starting
    uint8_t base_frame_length = base_frame_pacing_lut[speed_setting];
    uint8_t delay_frames = base_frame_length;
    while (--delay_frames) 
        wait_for_flush();

    frame_length = 1;

    auto id = commands[command_index[current_sub]];
    draw_command_string(command_to_string_lut[id]);

    player_died = false;

    while (true) {
        ppu_wait_nmi();
        pad_poll(0);
        oam_clear();

        handle_execute_main_mode(base_frame_length);

        draw_sprites();

        if (finished_execute)
            break;
        if (finished_level)
            return;
    }

    current_sub = original_sub;
    command_index[0] = original_index[0];
    command_index[1] = original_index[1];
    command_index[2] = original_index[2];

    wait_for_flush();
    for (pickup_count=0; pickup_count<sizeof(pickup_list); ++pickup_count) {
        uint8_t p = pickup_list[pickup_count];
        if (p == 0xff) break;
        draw_pickup((Point)p);
        if (pickup_count & 1) {
            wait_for_flush();
        }
    }

    memcpy((void*)&objects, original_objs, sizeof(Object) * 8);
    set_game_mode(MODE_EDIT);
}