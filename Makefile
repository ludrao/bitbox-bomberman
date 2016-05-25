#Bomberman makefile
NO_SDCARD=1

NAME = bomberman
GAME_C_FILES = main.c terrain.c char1.c players.c font.c text.c border.c map1.c map_tiles.c map_ovr.c
#GAME_BINARY_FILES = 
GAME_C_OPTS = -DVGAMODE_640
DISPLAY_PROFILING=1
include $(BITBOX)/lib/bitbox.mk