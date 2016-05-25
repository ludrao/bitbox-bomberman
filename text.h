#ifndef __TEXT_H__
#define __TEXT_H__

extern char text_buffer[2][40];


void text_init();
void text_update();
void text_num(int i, int j, int v);
void print(int i, int j, char* str);
#endif