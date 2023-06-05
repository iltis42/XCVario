extern "C" {
#include "eglib.h"
}
#include "inttypes.h"
#include "Arduino.h"

#pragma once

// later we want to get rid of UGC, so lets add all needed API definitions here

typedef struct _ucg_color_t
{
	uint8_t color[3];             /* 0: Red, 1: Green, 2: Blue */
}ucg_color_t;


#define UCG_DRAW_UPPER_RIGHT EGLIB_DRAW_UPPER_RIGHT
#define UCG_DRAW_UPPER_LEFT  EGLIB_DRAW_UPPER_LEFT
#define UCG_DRAW_LOWER_LEFT  EGLIB_DRAW_LOWER_LEFT
#define UCG_DRAW_LOWER_RIGHT EGLIB_DRAW_LOWER_RIGHT
#define UCG_DRAW_ALL         EGLIB_DRAW_ALL

#define UCG_PRINT_DIR_LR 0x00
#define UCG_PRINT_DIR_TD 0x01
#define UCG_PRINT_DIR_RL 0x02
#define UCG_PRINT_DIR_BU 0x03
#define UCG_FONT_POS_BASE 0x01
#define UCG_FONT_POS_BOTTOM 0x02
#define UCG_FONT_POS_CENTER 0x03
#define UCG_FONT_POS_TOP 0x04

typedef enum _e_font_mode { UCG_FONT_MODE_TRANSPARENT, UCG_FONT_MODE_SOLID } e_font_mode;


typedef enum _fonts_enum {
	UCG_FONT_9x15B_MF,
	UCG_FONT_NCENR14_HR,
	UCG_FONT_FUB11_TR,
	UCG_FONT_FUB11_HR,
	UCG_FONT_FUB14_HR,
	UCG_FONT_FUB14_HF,
	UCG_FONT_FUB17_HF,
	UCG_FONT_FUB20_HN,
	UCG_FONT_FUB20_HR,
	UCG_FONT_FUB20_HF,
	UCG_FONT_FUB25_HR,
	UCG_FONT_FUB25_HF,
	UCG_FONT_FUR25_HF,
	UCG_FONT_FUB35_HN,
	UCG_FONT_FUB35_HR,
	UCG_FONT_FUR14_HF,
	UCG_FONT_FUR20_HF,
	UCG_FONT_PROFONT22_MR,
	UCG_FONT_FUB25_HN,
	UCG_FONT_FUB11_HN,
	UCG_FONT_FUB14_HN
} e_fonts_enum;

// to be activated as soon as ucg.h is replaced by AdaptUGC.h

extern uint8_t ucg_font_9x15B_mf[];
extern uint8_t ucg_font_ncenR14_hr[];
extern uint8_t ucg_font_fub11_tr[];
extern uint8_t ucg_font_fub11_hr[];
extern uint8_t ucg_font_fub14_hn[];
extern uint8_t ucg_font_fub14_hr[];
extern uint8_t ucg_font_fub14_hf[];
extern uint8_t ucg_font_fur14_hf[];
extern uint8_t ucg_font_fub17_hf[];
extern uint8_t ucg_font_fub20_hn[];
extern uint8_t ucg_font_fub20_hr[];
extern uint8_t ucg_font_fub20_hf[];
extern uint8_t ucg_font_fur20_hf[];
extern uint8_t ucg_font_fub25_hr[];
extern uint8_t ucg_font_fub25_hf[];
extern uint8_t ucg_font_fur25_hf[];
extern uint8_t ucg_font_fub35_hn[];
extern uint8_t ucg_font_fub35_hr[];
extern uint8_t ucg_font_profont22_mr[];
extern uint8_t ucg_font_fub25_hn[];
extern uint8_t ucg_font_fub11_hn[];


class AdaptUGC : public Print{
public:
	// init
	void begin();
	void invertDisplay( bool inv ) {invertDisp=inv;};  	        // solved in grafic layer
	void setRedBlueTwist( bool twist ) {twistRB= twist;};   	    // no more needed, type of displays phased out
	inline void undoClipRange() { eglib_undoClipRange(eglib);};
	// color
	inline void setColor( uint8_t idx, uint8_t r, uint8_t g, uint8_t b ) {
		twistRB?
			eglib_SetIndexColor(eglib, idx, invertDisp?~b:b, invertDisp?~g:g, invertDisp?~r:r):
			eglib_SetIndexColor(eglib, idx, invertDisp?~r:r, invertDisp?~g:g, invertDisp?~b:b);
	}
	inline void setColor( uint8_t r, uint8_t g, uint8_t b ) {
		twistRB?
			eglib_SetIndexColor(eglib, 0, invertDisp?~b:b, invertDisp?~g:g, invertDisp?~r:r):
			eglib_SetIndexColor(eglib, 0, invertDisp?~r:r, invertDisp?~g:g, invertDisp?~b:b);
	}	
	// graphics
	inline void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1)  { eglib_DrawLine(eglib, x0, y0, x1, y1); }
	inline void drawBox(int16_t x, int16_t y, int16_t w, int16_t h)  { eglib_DrawBox(eglib, x, y, w, h); }
	inline void drawFrame(int16_t x, int16_t y, int16_t w, int16_t h)  { eglib_DrawFrame(eglib, x, y, w, h); }
	inline void drawHLine(int16_t x, int16_t y, int16_t len)  { eglib_DrawHLine(eglib, x, y, len); }
	inline void drawVLine(int16_t x, int16_t y, int16_t len)  { eglib_DrawVLine(eglib, x, y, len); }
	inline void drawPixel(int16_t x, int16_t y)  { eglib_DrawPixel(eglib, x, y); }
	inline void drawRBox(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)  { eglib_DrawRoundBox(eglib, x, y, w, h, r); }
	inline void drawRFrame(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)  { eglib_DrawRoundFrame(eglib, x, y, w, h, r); }
	inline void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2)  { eglib_DrawFilledTriangle(eglib, x0, y0, x1, y1, x2, y2); }
	inline void drawTetragon(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3)  { eglib_DrawTetragon(eglib, x0, y0, x1, y1, x2, y2, x3, y3); }
	inline void drawCircle(int16_t x, int16_t y, int16_t radius, uint8_t options=EGLIB_DRAW_ALL){ eglib_DrawCircle(eglib, x, y, radius, options); }
	inline void drawDisc(int16_t x, int16_t y, int16_t radius, uint8_t options){	eglib_DrawDisc(eglib, x, y, radius, options);	}

	// Text Printing
	size_t write(uint8_t c);
	size_t write(const uint8_t *buffer, size_t size);
	inline void setPrintPos(int16_t x, int16_t y) { eglib_print_xpos = x; eglib_print_ypos = y; };
	inline void setPrintDir(uint8_t d) { eglib_print_dir = d; }
	inline int16_t getStrWidth( const char * s ) { return ( eglib_GetTextWidth(eglib, s) ); };
	// Font related
	void setFont(uint8_t *f, bool filled=false );
	void setFontMode( uint8_t is_transparent ) {};  // no concept for transparent fonts in eglib, as it appears
	inline void setFontPosBottom() {  eglib_setFontOrigin( eglib, FONT_BOTTOM ); };
	inline void setFontPosCenter() {   eglib_setFontOrigin( eglib, FONT_MIDDLE ); };
	inline int16_t getFontAscent() { const struct font_t *font; font = eglib->drawing.font;  return font->ascent;  };
	inline int16_t getFontDescent() { const struct font_t *font; font = eglib->drawing.font; return font->descent; };

	// scrolling, clipping, clear
	inline void clearScreen(){ eglib_ClearScreen( eglib ); };
	inline void scrollLines(int16_t lines) {  eglib_scrollScreen( eglib, lines ); };     	    // display driver function  tbd.
	inline void scrollSetMargins( int16_t top, int16_t bottom ) { eglib_setScrollMargins( eglib, top, bottom ); };                 // display driver function
	inline void setClipRange( int16_t x, int16_t y, int16_t w, int16_t h ) { eglib_setClipRange(eglib, x, y, w, h );};

private:
	inline void advanceCursor( size_t delta );

	int16_t eglib_print_xpos = 0, eglib_print_ypos = 0;
	int8_t eglib_font_pos = UCG_FONT_POS_BOTTOM;
	uint8_t eglib_print_dir = UCG_PRINT_DIR_LR;
	eglib_t * eglib;
	bool twistRB;
	bool invertDisp;
};
