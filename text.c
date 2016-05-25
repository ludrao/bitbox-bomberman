#include <stdint.h>
#include <bitbox.h>

#include "text.h"
#include "players.h"

// Handle text display on the top of the screen
// 2 lines or 40 chars
char text_buffer[2][40];

void text_init()
{

    print(5, 0, "Player 1");
    print(26, 0, "Player 2");

    for (int i=0; i<2; i++)
    {
        print(2+i*21, 1, "L   B   F   S   ");
    }
}

void text_update()
{
    for (int i=0; i<2; i++)
    {
        Player *p = &player[i];
        text_num(3+i*21, 1, p->life);
        text_num(7+i*21, 1, p->rem_bombs);
        text_num(11+i*21, 1, p->bomb_range+1);
        text_num(15+i*21, 1, p->speed);
    }
    
}


void text_num(int i, int j, int v)
{
    char* str = &text_buffer[j][i];
    if (v<0)
    {
        *str++ = 13;
        v = -v;
    }
    char* beg = str;    
    do
    {
        *str++ = 16 + (v%10);
        v /= 10;
    } while (v>0);
    
     char t;
     while (beg < str)
     {
         t = *beg;
         *beg++ = *--str;
         *str = t;
     }
    
    
}

void print(int i, int j, char* str)
{
    for(int c = 0; str[c] && (c+i<40) ; c++)
    {
        uint8_t v = str[c];
        if (v>122)
            v = 62; // unhandled code are transformed into '.'
        else if (v>90)
            v -= 32; // small letters are transformed in caps
        text_buffer[j][i+c]=v-32;
    }
}