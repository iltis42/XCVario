#include "eglib.h"
#include "inttypes.h"
#include "Arduino.h"

// later we want to get rid of UGC, so lets add all needed API definitions here

#define UCG_DRAW_UPPER_RIGHT 0x01
#define UCG_DRAW_UPPER_LEFT  0x02
#define UCG_DRAW_LOWER_LEFT 0x04
#define UCG_DRAW_LOWER_RIGHT  0x08
#define UCG_PRINT_DIR_LR 0x00
#define UCG_PRINT_DIR_TD 0x01
#define UCG_PRINT_DIR_RL 0x02
#define UCG_PRINT_DIR_BU 0x03

class AdaptUGC : public Print{
public:

	void begin() {
		// eglib = ... tbd.
	}

	inline void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1)  { eglib_DrawLine(eglib, x0, y0, x1, y1); }   // inline
	inline void drawBox(int16_t x, int16_t y, int16_t w, int16_t h)  { eglib_DrawBox(eglib, x, y, w, h); }
	inline void drawFrame(int16_t x, int16_t y, int16_t w, int16_t h)  { eglib_DrawFrame(eglib, x, y, w, h); }	
	inline void drawHLine(int16_t x, int16_t y, int16_t len)  { eglib_DrawHLine(eglib, x, y, len); }	
	inline void drawVLine(int16_t x, int16_t y, int16_t len)  { eglib_DrawVLine(eglib, x, y, len); }	
	inline void drawPixel(int16_t x, int16_t y)  { eglib_DrawPixel(eglib, x, y); }	
	inline void drawRBox(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)  { eglib_DrawRoundBox(eglib, x, y, w, h, r); }
	inline void drawRFrame(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)  { eglib_DrawRoundFrame(eglib, x, y, w, h, r); }
	inline void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2)  { eglib_DrawTriangle(eglib, x0, y0, x1, y1, x2, y2); }	
	inline void drawTetragon(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3)  { eglib_DrawTriangle(eglib, x0, y0, x1, y1, x2, y2); eglib_DrawTriangle(eglib, x0, y0, x2, y2, x3, y3); }	
	
	
	void drawCircle(int16_t x, int16_t y, int16_t radius, uint8_t options){                                                                        // adapter
		switch( options ){
		case UCG_DRAW_ALL:
			eglib_DrawCircle(eglib, x, y, radius);
			break;
		case UCG_DRAW_UPPER_RIGHT:
			eglib_DrawArc(eglib, x, y, radius, 0.0, 90.0);
			break;
		case UCG_DRAW_UPPER_LEFT:
			eglib_DrawArc(eglib, x, y, radius, 0.0, -90.0);
			break;
		case UCG_DRAW_LOWER_RIGHT:
			eglib_DrawArc(eglib, x, y, radius,90.0, 180.0);
			break;
		case UCG_DRAW_LOWER_LEFT:
			eglib_DrawArc(eglib, x, y, radius, 180.0, 270.0);
			break;

		}
	}
	void drawDisc(int16_t x, int16_t y, int16_t radius, uint8_t options){                                                                        // adapter
		switch( options ){
		case UCG_DRAW_ALL:
			eglib_DrawDisc(eglib, x, y, radius);
			break;
		case UCG_DRAW_UPPER_RIGHT:
			eglib_DrawFilledArc(eglib, x, y, radius, 0.0, 90.0);
			break;
		case UCG_DRAW_UPPER_LEFT:
			eglib_DrawFilledArc(eglib, x, y, radius, 0.0, -90.0);
			break;
		case UCG_DRAW_LOWER_RIGHT:
			eglib_DrawFilledArc(eglib, x, y, radius,90.0, 180.0);
			break;
		case UCG_DRAW_LOWER_LEFT:
			eglib_DrawFilledArc(eglib, x, y, radius, 180.0, 270.0);
			break;

		}
	}
	inline void setPrintPos(ucg_int_t x, ucg_int_t y) { eglib_print_xpos = x; eglib_print_ypos = y; }
	inline void setPrintDir(uint8_t d) { eglib_print_dir = d; }
	
	size_t write(uint8_t c) { 
		int8_t delta;
	//  delta = ucg_DrawGlyph(get_ucg(), get_tx(), get_ty(), get_tdir(), c); 
	// eglib_DrawGlyph(eglib_t * eglib, coordinate_t x, coordinate_t y, const struct glyph_t * glyph)
 	//	eglib_DrawWChar(eglib, eglib_print_xpos, eglib_print_ypos, wchar_t (c) );
		const struct glyph_t * g;
		g = eglib_GetGlyph(eglib, wchar_t (c));
		eglib_DrawGlyph(eglib, eglib_print_xpos, eglib_print_ypos, g);
		delta = g->advance;
		switch(eglib_print_dir) {
    			case 0: eglib_print_xpos += delta; break;
    			case 1: eglib_print_ypos += delta; break;
    			case 2: eglib_print_xpos -= delta; break;
    			default: case 3: eglib_print_ypos -= delta; break;
  		}
		return 1;
	}
private:
	int16_t eglib_print_xpos = 0, eglib_print_ypos = 0;
	uint8_t eglib_print_dir = UCG_PRINT_DIR_LR;
	
	// two things done above, rest tbd:
	/*
 	ucg->drawGlyph
    	ucg->drawString
    
	ucg->invertDisplay  // Treiberfunktion
	ucg->print
	ucg->printf
	ucg->scrollLines     // Treiberfunktion
	ucg->scrollSetMargins  // Treiberfunktion
	ucg->setClipRange
	ucg->setColor
	ucg->setFont
	ucg->setFontMode
	ucg->setFontPosBottom
	ucg->setFontPosCenter
	ucg->setPrintPos
	ucg->setRedBlueTwist  // Treiberfunktion
	ucg->setRotate180
	ucg->undoClipRange

*/
	eglib_t * eglib;
};
