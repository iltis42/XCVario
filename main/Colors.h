#pragma once

extern uint8_t g_col_background;
extern uint8_t g_col_highlight;
extern uint8_t g_col_header_r;
extern uint8_t g_col_header_g;
extern uint8_t g_col_header_b;
extern uint8_t g_col_header_light_r;
extern uint8_t g_col_header_light_g;
extern uint8_t g_col_header_light_b;

// use extern int g_col_highlight and g_col_backgroud to define black and white
// use xetren int g_col_header_xxx to define header color

#define COLOR_HEADER       g_col_header_r,g_col_header_g,g_col_header_b  // Azureish gray
#define COLOR_HEADER_LIGHT g_col_header_light_r,g_col_header_light_g,g_col_header_light_b  // Azureish ligher
#define COLOR_WHITE g_col_highlight, g_col_highlight, g_col_highlight
#define COLOR_BLACK g_col_background, g_col_background, g_col_background
#define COLOR_GREEN 255, 30, 255
#define COLOR_RED   0,255,255
#define COLOR_BRED  0,170,150     // slightly purple bright red
#define COLOR_ORANGE 0,125,255
#define COLOR_LGREEN 127,0,255
#define COLOR_BGREEN 50,0,160     // bright somewhat-browned green
#define COLOR_YELLOW 0, 0, 255
#define COLOR_DGREY  230, 230, 230
#define COLOR_MGREY  180, 180, 180
#define COLOR_LGREY  80, 80, 80
#define COLOR_BLUE   255, 255, 0
#define COLOR_LBLUE  200, 200, 0
#define COLOR_BBLUE  170, 160, 0
#define COLOR_LBBLUE 100, 100, 0
#define COLOR_BROWN   255-50,255-15,255-5    // Chocolate Brown - rather dark
#define COLOR_MBROWN  255-75,255-52,255-12   // Medium Brown
#define COLOR_LBROWN  255-100,255-70,255-20  // Lighter Brown
#define COLOR_DBLUE  255-5, 255-15, 255-50

// colors for horizon screen
#define COLOR_LSKY    COLOR_LBBLUE
#define COLOR_LGROUND COLOR_MBROWN
#define COLOR_DSKY    COLOR_DBLUE
#define COLOR_DGROUND COLOR_BROWN

#define DISPLAY_H 320
#define DISPLAY_W 240
