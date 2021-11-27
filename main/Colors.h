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
#define COLOR_ORANGE 0,125,255
#define LIGHT_GREEN 127,0,255
#define COLOR_YELLOW 0, 0, 255
#define DARK_GREY    230, 230, 230
#define COLOR_MGREY  180, 180, 180
#define COLOR_BLUE   255, 255, 0
#define COLOR_LBLUE  200, 200, 0
#define COLOR_BBLUE  170, 160, 0



#define DISPLAY_H 320
#define DISPLAY_W 240
