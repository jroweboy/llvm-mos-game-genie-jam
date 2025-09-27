#pragma once

#include <cstdint>
#include "fixed_point2.hpp"
#include <soa.h>

#include "common.hpp"
#include "levels.hpp"

using namespace fixedpoint_literals;

enum Command {
    CMD_WAIT,
    CMD_MOVE,
    CMD_TURN_LEFT,
    CMD_TURN_RIGHT,
    CMD_PICKUP,
    CMD_RETURN,
    CMD_JMP_ONE,
    CMD_JMP_TWO,
    CMD_END, // used when storing level data to signal we finished the commands
};

union Point {
    uint8_t raw;
    struct {
        uint8_t y : 4;
        uint8_t x : 4;
    };
};

struct PlayerExtra {
    uint8_t frame;
};

struct Object {
    ObjectType type;
    fu8_8 x;
    fu8_8 y;
    fs8_8 x_vel;
    fs8_8 y_vel;
    uint8_t target_x;
    uint8_t target_y;
    uint8_t state;
    uint8_t frame;
    uint8_t timer;
    uint8_t long_timer;
    uint8_t anim_timer;
    uint8_t anim_state;
    uint8_t facing_dir;
    uint8_t param2;
    uint8_t is_moving;
};
#define SOA_STRUCT Object
#define SOA_MEMBERS MEMBER(type) MEMBER(x) MEMBER(y) MEMBER(x_vel) MEMBER(y_vel) \
    MEMBER(target_x) MEMBER(target_y) MEMBER(state) MEMBER(frame) MEMBER(timer) \
    MEMBER(long_timer) \
    MEMBER(anim_timer) MEMBER(anim_state) MEMBER(facing_dir) \
    MEMBER(param2) MEMBER(is_moving)
#include <soa-struct.inc>
extern soa::Array<Object, 8> objects;


struct Cursor {
    // fu8_8 x;
    // fu8_8 y;
    // fs8_8 x_vel;
    // fs8_8 y_vel;
    uint8_t x;
    uint8_t y;
    int8_t x_vel;
    int8_t y_vel;
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
extern soa::Array<Cursor, 2> cursors;

enum SelectionMode {
    SELECT_SUB,
    SELECT_ONE,
    SELECT_TWO,
};

extern const uint8_t command_attr_lut[9];
extern uint8_t commands[12 + 9 + 9];
extern uint8_t command_index[3];
extern uint8_t current_sub;

extern uint8_t pickup_count;

extern uint8_t level_metatiles[(10 * 9) / 2];

void update_sub_attribute(uint8_t new_val = 3);
void game_mode_edit_main();
void game_mode_execute_main();
void update_command_list(uint8_t new_command);

// uint8_t find_obj_slot(ObjectType t);

