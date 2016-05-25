
#include <stdlib.h>
#include <bitbox.h>
#include "players.h"
#include "terrain.h"


#include "char1.h"

Player player[2];

#define min(a,b) ((a)<(b) ? (a) : (b))

void players_init()
{   
    for  (int i=0; i<2; i++)
    {
        Player *p = &player[i];

        p->sprite_width = _char1_tw;
        p->sprite_height = _char1_th;
        p->data = _char1_data;
    
        p->col = 1 +i*(MWIDTH-3);
        p->row = 1 +i*(MHEIGHT-3);
    
        p->rem_bombs = 1;
        p->bomb_range = 0;
        p->life = 3;
        p->speed = 3;
    }
}




/*
 * Update player position and directions according to gamepad button pressed
 * The direction is updated as soon as the button is pressed, however the  movement
 * of the sprite follow the grid, hence the multiple states in the direction.
 */
void players_update()
{
    for(int pi=0; pi<2; pi++)
    {
        uint16_t buttons = gamepad_buttons[pi];
        Player *p = &player[pi];

        int i = p->col;
        int j = p->row;
        
        // Update timed ressources
        if (p->shield)
            p->shield--;

        // Are we still alive BTW ?
        if (terrain_ovr[j][i] >= MT_EXPL0 && terrain_ovr[j][i] <= MT_EXPL4 && p->shield == 0)
        {
            // We got fire !
            if (!p->life)
            {
                // TODO handle death here....
                message("really dead this time\n");
                break;
            }
            p->life--;
            p->shield = 120; // seconds shield
            break;
        }
        
        char btndir = ( buttons >> 8) & 0xF;

        
        // Make sure we only have one bit set in the direction button bitfield
        int c;
        for(c=1; c<=8; c<<=1)
            if (btndir&c) break;
        btndir = c & 0xF; 
        
        // Update player position if there is a button pressed
        if (btndir)
        {
            // Compute forward-backward pos depending on btndir direction
            int fbpos = (btndir&DIR_HORZ) ? p->xoff : p->yoff;
            fbpos = (btndir&DIR_RD) ? fbpos : -fbpos ;
            int step = p->speed;
            
            
            // If we are approaching the next block (in forward direction) ( => check colision with non walk-able blocks)
            if (fbpos <= 2 && fbpos+step>2)
            {     
                // Increments to compute surounding cases:
                //  * forward: +a, +b
                //  * backward: -a, -b
                //  * left: +b, -a
                //  * right: -b, +a
                int a = ((btndir>>3)&1) - ((btndir>>2)&1);
                int b = ((btndir>>1)&1) - (btndir&1);

                // Compute right-left pos depending on btndir direction
                int rlpos = (btndir&DIR_HORZ) ? p->yoff : p->xoff;
                rlpos = (btndir&DIR_UR) ? rlpos : -rlpos ;
                        
                char orthaxe = (btndir&DIR_HORZ) ? DIR_VERT : DIR_HORZ; // orthogonal axe to btndir
                char leftdir =  (btndir&DIR_UR) ? orthaxe&DIR_LU : orthaxe&DIR_RD;
                
                // Check if block in btndir direction is walk-able
                if (!terrain_canwalk(i+a, j+b))
                {
                    step =  2 - fbpos;
                    if (!step)
                        btndir = 0;
                }
                
                else
                {
                    // If the forward left block is non walk-able, need to align on the horiz grid
                    if (rlpos < -2 && !terrain_canwalk(i+a+b, j+b-a))
                    {
                        btndir = leftdir^orthaxe;
                        step = min(step, -2-rlpos);
                    }
                    // If the forward left block is non walk-able, need to align on the horiz grid
                    else if (rlpos > 2 && !terrain_canwalk(i+a-b, j+b+a))
                    {
                        btndir = leftdir;
                        step = min(step, rlpos-2);
                    }
                }


            }
            
            // Move the player sprite according to direction
            p->xoff += (((btndir>>3)&1) - ((btndir>>2)&1)) * step;
            p->yoff += ((((btndir>>1)&1) - (btndir&1))) * step;
            
            
            // Change postion on the grid
            if (p->xoff > 16 || p->xoff <= -16)
            {
                p->col += ((btndir>>3)&1) - ((btndir>>2)&1);
                p->xoff += p->xoff < 0 ? 32 : -32;
            }
            if (p->yoff > 16 || p->yoff <= -16)
            {
                p->row += ((btndir>>1)&1) - (btndir&1);
                p->yoff += p->yoff < 0 ? 32 : -32;
            }

            //message("%d %d\n", p->xoff, p->yoff);
        }
        
        // Drop bombs !
        if (buttons&gamepad_A)
        {
            if (terrain_canwalk(i, j) && p->rem_bombs)
            {
                p->rem_bombs--;
                terrain_ovr[j][i] = MT_BOMB1;
                // 2 seconds timeout for a bomb
                terrain_data[j][i] = 60 | (p->bomb_range<<8) | (pi<<12);
                
            }
            
        }
    } 
}





