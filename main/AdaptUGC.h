#include "eglib.h"
#include "inttypes.h"

// later we want to get rid of UGC, so lets add all needed API definitions here

#define UCG_DRAW_UPPER_RIGHT 0x01
#define UCG_DRAW_UPPER_LEFT  0x02
#define UCG_DRAW_LOWER_LEFT 0x04
#define UCG_DRAW_LOWER_RIGHT  0x08

class AdaptUGC {
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


	
	// two things done above, rest tbd:
	/*
	
	ucg->drawTetragon

	ucg->invertDisplay
	ucg->print
	ucg->printf
	ucg->scrollLines
	ucg->scrollSetMargins
	ucg->setClipRange
	ucg->setColor
	ucg->setFont
	ucg->setFontMode
	ucg->setFontPosBottom
	ucg->setFontPosCenter
	ucg->setPrintPos
	ucg->setRedBlueTwist
	ucg->setRotate180
	ucg->undoClipRange

*/
	eglib_t * eglib;
};
