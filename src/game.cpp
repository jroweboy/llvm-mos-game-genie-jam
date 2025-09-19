
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


uint8_t load_object(ObjectType t) {
    if (t == ObjectType::PLAYER) {

        return 0;
    } else {
        return 1;
    }
}


void draw_level() {

}