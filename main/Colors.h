#pragma once

extern int g_col_background;
extern int g_col_highlight;

// use extern int col_highlight and col_backgroud to define black and white

#define COLOR_HEADER       (101+g_col_background/5),(108+g_col_background/5),g_col_highlight  // Azureish gray
#define COLOR_HEADER_LIGHT (161-g_col_background/4),(168-g_col_background/3),g_col_highlight  // Azureish ligher

#define COLOR_WHITE g_col_highlight, g_col_highlight, g_col_highlight
#define COLOR_BLACK g_col_background, g_col_background, g_col_background
#define COLOR_GREEN 255, 30, 255
#define COLOR_RED   0,255,255
#define LIGHT_GREEN 127,0,255
#define COLOR_YELLOW 0, 0, 255
#define DARK_GREY    230, 230, 230
#define COLOR_MGREY  180, 180, 180
#define COLOR_BLUE   255, 255, 0
#define COLOR_LBLUE  200, 200, 0



#define DISPLAY_H 320
#define DISPLAY_W 240
