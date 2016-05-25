#ifndef __PLAYERS_H__
#define __PLAYERS_H__

typedef struct
{
    // Position/direction
    int row, col;   // position in the map
    int xoff;       // offset in pixels relative to the row,col position
    int yoff;       // offset in pixels relative to the row,col position
    uint8_t currdir;   // Current Direction
    
    // Sprite metrics/data
    uint8_t sprite_width;
    uint8_t sprite_height;
    const unsigned short **data;
    
    // Player ressources
    uint16_t shield; // when non zero, protected from flames. This is a timer
    uint8_t rem_bombs; // remaining bombs
    uint8_t bomb_range; // range of bomb blast minus 1
    uint8_t life; // remaining lives
    uint8_t speed; // moving speed
    
    
    
} Player;

typedef enum
{
    DIR_UP      = 0x1,
    DIR_DOWN    = 0x2,
    DIR_LEFT    = 0x4,
    DIR_RIGHT   = 0x8,
    DIR_VERT    = 0x3,
    DIR_HORZ    = 0xC,
    DIR_RD      = 0xA, // Diag Right Down
    DIR_LU      = 0x5, // Diag Left Up
    DIR_UR      = 0x9, // Diag Up Right
    DIR_DL      = 0x6, // Diag Down Left
} Direction;
    

extern Player player[2];
void players_update();
void players_init();

#endif //__PLAYERS_H__