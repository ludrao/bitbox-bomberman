#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <stdint.h>

#define MWIDTH  19
#define MHEIGHT 13


typedef enum
{
    MT_EMPTY            = 0xFF, // no sprite for that one...
    MT_SOLID            = 0xFE, // no sprite for that one...
    MT_BREAK1           = 0,
    MT_BREAK2           = 0,
    MT_BOMB1            = 1,
    MT_BOMB2            = 2,
    MT_EXPL00           = 0xFD, // this code is used for the flame propagation
    MT_EXPL0            = 3,
    MT_EXPL1            = 4,
    MT_EXPL2            = 5,
    MT_EXPL3            = 6,
    MT_EXPL4            = 7,
    MT_EXPL5            = 8,
} ovr_type;


void load_terrain(const uint8_t* map);
void terrain_update(); // to call each frame: update terrain according to the different actions in progress

// Map that holds 16x16 tiles to draw on the screen. Each byte in the map is the id of the tile to render
// The map is organised in lines.
extern uint8_t terrain_map[MHEIGHT*2][MWIDTH*2];
// Contains overlay elements to draw, and information if the position is walkable or not
extern uint8_t terrain_ovr[MHEIGHT][MWIDTH];


// Data attached to the map. The content of each elements depends on the map element:
// MT_BOMB1 or MT_BOMB2: bit 15-12 player that droped the bomb
//                       bit 11-8 bomb range (flame length)
//                       bit 5-0: timer to explode the bomb (count down)
#define TDATA_BOMB_MASK_TIMER       0x003F
#define TDATA_BOMB_MASK_RANGE       0x0F00
#define TDATA_BOMB_MASK_PLAYER      0xF000

extern uint16_t terrain_data[MHEIGHT][MWIDTH];

// Empty means that player can walk at that location
static inline int terrain_canwalk(int i, int j)
{
    return (i>=0 && i<MWIDTH && j>=0 && j<MHEIGHT) && terrain_ovr[j][i] == MT_EMPTY;
}


#endif //__TERRAIN_H__