
#include <stdint.h>

#include <neslib.h>

#include "graphics.hpp"
#include "common.hpp"
#include "game.hpp"

extern __zp unsigned char SPRID;
extern char OAM_BUF[256];

const uint8_t arrow_left_metasprite[]={

	  6,(uint8_t)- 2,0x01,0|OAM_BEHIND,
	  6,  2,0x03,1,
	  4,(uint8_t)- 2,0x01,1,
	  2,  4,0x05,1,

	  2, 10,0x08,0|OAM_BEHIND,
	  0,  6,0x0e,1,
	0x80
};

const uint8_t arrow_up_metasprite[]={
	(uint8_t)- 2,  6,0x04,0|OAM_FLIP_H|OAM_BEHIND,
	  2,  0,0x0e,1|OAM_FLIP_V,
	  4,(uint8_t)- 2,0x0c,1|OAM_FLIP_V,
	  6,  6,0x05,1,

	 10,  6,0x01,0|OAM_FLIP_V|OAM_BEHIND,
	 10,  0,0x01,1,
	0x80

};

const uint8_t arrow_right_metasprite[]={

	  2,(uint8_t)- 2,0x01,0|OAM_FLIP_H|OAM_BEHIND,
	  2,  2,0x03,1|OAM_FLIP_H,
	  4,(uint8_t)- 2,0x01,1|OAM_FLIP_H,
	  6,  4,0x05,1|OAM_FLIP_H,

	  2, 10,0x04,0|OAM_FLIP_H|OAM_BEHIND,
	  8,  6,0x0e,1|OAM_FLIP_H,
	0x80
};

const uint8_t arrow_down_metasprite[]={

	(uint8_t)- 2,  2,0x04,0|OAM_FLIP_H|OAM_FLIP_V|OAM_BEHIND,
	 10,  2,0x01,0|OAM_BEHIND,
	  2,  8,0x0e,1,
	  4, 10,0x0c,1,

	 10,  8,0x01,1|OAM_FLIP_V,
	  6,  2,0x05,1|OAM_FLIP_V,
	0x80
};

SPLIT_ARRAY(player_msprite,
    arrow_up_metasprite,
    arrow_right_metasprite,
    arrow_down_metasprite,
    arrow_left_metasprite
)

void draw_player() {
    auto player = objects[0];
    // param1 == facing dir
    auto mspr = (uint8_t*)SPLIT_ARRAY_POINTER(player_msprite, player->facing_dir);
    oam_meta_spr(player->x, player->y, mspr);
}

constinit FIXED static const uint8_t cursor_expand_timer_lut[] = {
    12, 10, 8, 6, 6, 6, 8, 10
};

void draw_cursor(uint8_t slot) {
    auto cursor = objects[slot];
    if (cursor->timer == 0) {
        cursor.anim_timer++;
        cursor.anim_timer &= 7;
        cursor.timer = cursor_expand_timer_lut[cursor.anim_timer];
        cursor.param2 += cursor->facing_dir ? -1 : 1;
        cursor.long_timer = cursor->long_timer - 1;
        if (cursor->long_timer == 0) {
            cursor.long_timer = 4;
            cursor.facing_dir = !cursor->facing_dir;
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
    // top left
    OAM_BUF[SPRID+ 0] = cursor->y - 4 - 1 - cursor->param2;
    OAM_BUF[SPRID+ 3] = cursor->x - 4 - cursor->param2;
    // top right
    OAM_BUF[SPRID+ 4] = cursor->y - 4 - 1 - cursor->param2;
    OAM_BUF[SPRID+ 7] = cursor->x - 4 + cursor->anim_state + cursor->param2;
    // bot left
    OAM_BUF[SPRID+ 8] = cursor->y - 4 - 1 + cursor->state + cursor->param2;
    OAM_BUF[SPRID+11] = cursor->x - 4 - cursor->param2;
    // bot right
    OAM_BUF[SPRID+12] = cursor->y - 4 - 1 + cursor->state + cursor->param2;
    OAM_BUF[SPRID+15] = cursor->x - 4 + cursor->anim_state + cursor->param2;
    SPRID += 16;
}

void pal_fade_to(char from, char to, char duration) {
    while (from != to) {
        from = (from < to) ? from + 1 : from - 1;
        pal_bright(from);
        delay(duration);
    }
    // Wait one additional frame to make sure the last fade happens before exiting
    ppu_wait_nmi();
}
