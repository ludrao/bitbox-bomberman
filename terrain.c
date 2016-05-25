#include <bitbox.h>
#include <string.h>
#include <stdlib.h>

#include "terrain.h"
#include "players.h"




uint8_t terrain_map[MHEIGHT*2][MWIDTH*2];
uint8_t terrain_ovr[MHEIGHT][MWIDTH];
uint16_t terrain_data[MHEIGHT][MWIDTH];


void load_terrain(const uint8_t* map)
{
    // Jsut copy the given map into the working map
    memcpy(terrain_map, map, sizeof(terrain_map));
    
    // Populate the overlay map with the solid blocks (used to actually handle player movement!)
    for(int j=0;j<MHEIGHT;j++)
        for(int i=0;i<MWIDTH;i++)
        {
            // Only need to sample very to 16x16 tiles as a solid block is always aligned on 2 tiles!
            if (((terrain_map[j*2][i*2]-1)&31) >= 16) // TODO find a more robust way to determine if a tile is walka-able or not
                terrain_ovr[j][i] = MT_SOLID;
            
            else if ((((i>2) && (i<(MWIDTH-3)))||((j>2) && (j<(MHEIGHT-3)))) && (rand() > RAND_MAX/2))
                terrain_ovr[j][i] = MT_BREAK1;
            
            else
                terrain_ovr[j][i] = MT_EMPTY;
            
        }
            
    
}


static inline void propagate_flame(int i, int j, int di, int dj, int range, int exp)
{
    while (range--)
    {
        i += di;
        j += dj;
        
        // If the flame reaches a border, stop propagation
        if (i<0 || i>=MWIDTH || j<0 || j>=MHEIGHT)
            break;
        
        uint8_t v = terrain_ovr[j][i];
        
        if (v == MT_EMPTY)
        {
            terrain_ovr[j][i] = exp;
        }
        else if (v == MT_BREAK1 || v == MT_BREAK2)
        {
            // remove the breakable block and stop propagation
            terrain_ovr[j][i] = MT_EMPTY;
            break;
        }

        else if (v == MT_BOMB1 || v == MT_BOMB2)
        {   // Ignite this bomb by expiring the timer, and stop propagation
            terrain_data[j][i] &= ~0x3F;
            terrain_data[j][i] += (di>0) || (dj>0); // in case this bomb is going to be scanned by the terrain loop,
                                                    // add 1 so that they are not exploded during the same pass, but the next one !
            break;
        }
        
        else
        {
            range = 0;
            break; // stop propagation
        }
    }
}


void terrain_update()
{
    int i, j;
    
    // Loop through the whole terrain to check for actions (bombs, explosions, or other timer)
    for(j=0;j<MHEIGHT;j++)
        for(i=0;i<MWIDTH;i++)
        {
            uint8_t v = terrain_ovr[j][i];
            if (v==MT_BOMB1 || v==MT_BOMB2)
            {
                terrain_ovr[j][i] = (v == MT_BOMB1 ? MT_BOMB2 : MT_BOMB1);
                
                // If the timer expired, need to explode the bomb, otherwise just decrement counter...
                uint16_t d = terrain_data[j][i];
                if (!(d&0x3F))
                {
                    terrain_ovr[j][i] = MT_EXPL0;
                    player[(d>>12)&1].rem_bombs++; // increment back the players remaining bomb count
                    int range = ((d >> 8) & 0xF)+1;
                    
                    // Propagate the explostion in the 4 directions untils it reach an obstacle
                    // Right and Doown flames have specia codes so that when the current pass get to them there are assigned the correct MR_EXPL0 value...
                    propagate_flame(i, j,  0,  1, range, MT_EXPL00);
                    propagate_flame(i, j,  0, -1, range, MT_EXPL0);
                    propagate_flame(i, j,  1,  0, range, MT_EXPL00);
                    propagate_flame(i, j, -1,  0, range, MT_EXPL0);                    
                }
                else
                    terrain_data[j][i]--;
            }
            else if (v == MT_EXPL00)
                terrain_ovr[j][i] = MT_EXPL0;
            else if (v == MT_EXPL5)
                terrain_ovr[j][i] = MT_EMPTY;
            else if (v>=MT_EXPL0 && v< MT_EXPL5)
            {
                terrain_ovr[j][i] = v+1;
            }
  
        }
}