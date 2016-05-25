#include <bitbox.h>
#include <string.h>
#include <stdint.h>

#include "bomberman.h"
#include "map_tiles.h"
#include "map_ovr.h"
#include "text.h"
#include "font.h"
#include "terrain.h"
#include "players.h"
#include "border.h"

#include "map1.h"



void game_init()
{
    load_terrain(map1_data);
    players_init();
    text_init();
}





void game_frame()
{
    static int count = 0;
    kbd_emulate_gamepad();
    players_update();
    
    // Only update terrain every 50ms
    if (count%3==0) 
    {
        terrain_update();
        text_update();
    }
    
    count++;
}

void graph_frame(){}





// Draw the game !
#define XOFF    16
void graph_line()
{
    // dline is the line count, removing the top part that contains the text
    int dline = vga_line -48;
    
    // The top part is reserved for information display
    if (vga_line < 48)
    {
        for(int col=0; col<640/16; col++)
        {
            // Draw the back ground
            int bid = vga_line < 32 ? 4 : 1;
            if (col==0 && vga_line >= 32)
                bid = 0;
            else if (col == 39 && vga_line >= 32)
                bid = 2;
            memcpy(draw_buffer+16*col, _border_data[bid]+16*(vga_line%16), 16*2);
            
            int text_line = vga_line-6;
            if (text_line >= 0 && text_line < 32)
            {
                
                // Draw the text
                uint8_t c = (uint8_t) text_buffer[text_line>>4][col];
                if (c)
                {
                    for(int i = 0; i < 16; i++) // TODO replace by a transparent blit function
                    {
                        unsigned short v = *(_font_data[c]+16*(text_line%16)+i);
                        if (v&0x8000)
                            *(draw_buffer+16*col+i) = v;
                    } 
                }
            }

        }
    }
    // Print the game board
    else if (vga_line < 464)
    {
        // Draw the map
        int row2 = dline / 16;      // compute the row in the map
        int row  = dline / 32;
        int yoff2 = dline % 16;     // line of the sprite to draw
        int yoff  = dline % 32;

        for(int col=0; col<MWIDTH; col++)
        {
            if (!terrain_ovr[row][col]) { // overlay is covering everything ? render it, opaque
                memcpy(draw_buffer+XOFF+32*col, _map_ovr_data[terrain_ovr[row][col]]+32*yoff, 32*2);
            } else {
                // Draw the ground (map)
                int col2 = 2 *col;
                memcpy(draw_buffer+XOFF+16*col2, _map_tiles_data[terrain_map[row2][col2]-1]+16*yoff2, 16*2);
                col2++;
                memcpy(draw_buffer+XOFF+16*col2, _map_tiles_data[terrain_map[row2][col2]-1]+16*yoff2, 16*2);
                
                // Draw the overlay
                if (! (terrain_ovr[row][col]&0x80)) // bit7 set means nothing to draw
                {
                    //memcpy(draw_buffer+XOFF+32*col, _map_ovr_data[terrain_ovr[row][col]]+32*yoff, 32*2);
                    const unsigned short *src = _map_ovr_data[terrain_ovr[row][col]]+32*yoff;
                    for(int i = 0; i < 32; i++) // TODO replace by a transparent blit function
                    {
                        unsigned short v = *(src+i);
                        if (v&0x8000)
                            *(draw_buffer+XOFF+32*col+i) = v;
                    } 
                }
            }
        }
        
        // Draw the border on each side
        memcpy(draw_buffer, _border_data[3]+16*(vga_line%16), 16*2);
        memcpy(draw_buffer+640-16, _border_data[5]+16*(vga_line%16), 16*2);
        
    }
    
    else // Bottom line
    {
        for(int col=0; col<640/16; col++)
        {
            int bid = 7;
            if (col==0)
                bid = 6;
            else if (col == 39)
                bid = 8;
            memcpy(draw_buffer+16*col, _border_data[bid]+16*(vga_line%16), 16*2);
        }
    }
    
    
    // Draw the player sprites (32x40 sprites)
    int pi;
    for (pi=0; pi<2; pi++)
    {
        Player *p = &player[pi];
        int y = p->row*32 + p->yoff - p->sprite_height + 30; //  sprite is aligned to the bottom (vertically) (-2 pixel margin)
    
        if (dline>=y && dline<y+p->sprite_height)
        {
            int x = p->col*32 + p->xoff + 16 - p->sprite_width/2; // sprite is centered horizontally
            for(int i = 0; i < p->sprite_width; i++)
            {
                unsigned short v = *(p->data[0]+(dline-y)*p->sprite_width+i);
                if (v != 0x83e0)
                    *(draw_buffer+XOFF+x+i) = v;
            }                
        }
    }


}