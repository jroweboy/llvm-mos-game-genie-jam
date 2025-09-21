#pragma once

#include <cstdint>
#include <fixed_point.h>
#include <soa.h>

#include "common.hpp"

using namespace fixedpoint_literals;

union Point {
    uint8_t raw;
    struct {
        uint8_t y : 4;
        uint8_t x : 4;
    };
};

struct Player {
    fu8_8 x;
    fu8_8 y;
    uint8_t frame;
};

struct Object {
    ObjectType type;
    uint8_t x;
    uint8_t y;
    uint8_t frame;
    uint8_t timer;
    uint8_t param1;
};


enum Command {
    CMD_WAIT,
    CMD_MOVE,
    CMD_TURN_LEFT,
    CMD_TURN_RIGHT,
    CMD_PICKUP,
    CMD_JMP_ONE,
    CMD_JMP_TWO,
};

#define SOA_STRUCT Object
#define SOA_MEMBERS MEMBER(type) MEMBER(x) MEMBER(y) MEMBER(frame) MEMBER(timer) MEMBER(param1)
#include <soa-struct.inc>
extern soa::Array<Object, 16> objects;

extern uint8_t main_commands[12];
extern uint8_t one_commands[9];
extern uint8_t two_commands[9];


uint8_t find_obj_slot(ObjectType t);

