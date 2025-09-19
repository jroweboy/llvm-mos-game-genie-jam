#pragma once

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
};

#define SOA_STRUCT Object
#define SOA_MEMBERS MEMBER(type) MEMBER(x) MEMBER(y) MEMBER(frame) MEMBER(timer)
#include <soa-struct.inc>
extern soa::Array<Object, 16> objects;

uint8_t load_object(ObjectType t);

