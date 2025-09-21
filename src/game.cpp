
#include <soa.h>

#include "game.hpp"


struct LevelCommand {
    uint8_t id;
    uint8_t x;
    uint8_t y;
};

#define SOA_STRUCT LevelCommand
#define SOA_MEMBERS MEMBER(x) MEMBER(y) MEMBER(id)
#include <soa-struct.inc>

soa::Array<Object, 16> objects;

uint8_t main_commands[12];
uint8_t one_commands[9];
uint8_t two_commands[9];

uint8_t find_obj_slot(ObjectType t) {
    if (t == ObjectType::PLAYER) {
        return 0;
    } else {
        return 1;
    }
}


void draw_level() {

}