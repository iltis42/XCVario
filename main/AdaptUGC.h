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

#define UCG_FONT_9x15B_MF 0x00
#define UCG_FONT_NCENR14_HR 0x01
#define UCG_FONT_FUB11_TR 0x02
#define UCG_FONT_FUB11_HR 0x03
#define UCG_FONT_FUB14_HN 0x04
#define UCG_FONT_FUB14_HR 0x05
#define UCG_FONT_FUB14_HF 0x06
#define UCG_FONT_FUB17_HF 0x07
#define UCG_FONT_FUB20_HN 0x08
#define UCG_FONT_FUB20_HR 0x09
#define UCG_FONT_FUB20_HF 0x0A
#define UCG_FONT_FUB25_HR 0x0B
#define UCG_FONT_FUB35_HN 0x0C
#define UCG_FONT_FUB35_HR 0x0D

/* to be activated as soon as ucg.h is replaced by AdaptUGC.h

const uint8_t ucg_font_9x15B_mf[] = { UCG_FONT_9x15B_MF };
const uint8_t ucg_font_ncenR14_hr[] = { UCG_FONT_NCENR14_HR };
const uint8_t ucg_font_fub11_tr[] = { UCG_FONT_FUB11_TR };
const uint8_t ucg_font_fub11_hr[] = { UCG_FONT_FUB11_HR };
const uint8_t ucg_font_fub14_hn[] = { UCG_FONT_FUB14_HN };
const uint8_t ucg_font_fub14_hr[] = { UCG_FONT_FUB14_HR };
const uint8_t ucg_font_fub14_hf[] = { UCG_FONT_FUB14_HF };
const uint8_t ucg_font_fub17_hf[] = { UCG_FONT_FUB17_HF };
const uint8_t ucg_font_fub20_hn[] = { UCG_FONT_FUB20_HN };
const uint8_t ucg_font_fub20_hr[] = { UCG_FONT_FUB20_HR };
const uint8_t ucg_font_fub20_hf[] = { UCG_FONT_FUB20_HF };
const uint8_t ucg_font_fub25_hr[] = { UCG_FONT_FUB25_HR };
const uint8_t ucg_font_fub35_hn[] = { UCG_FONT_FUB35_HN };
const uint8_t ucg_font_fub35_hr[] = { UCG_FONT_FUB35_HR };
*/

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
	inline void setColor(uint8_t idx, uint8_t r, uint8_t g, uint8_t b) { eglib_SetIndexColor(eglib, idx, r, g, b); }
	
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
	void setFont(uint8_t *f){                                                                        // adapter
		switch( f[0] ){
			case UCG_FONT_9x15B_MF:
			eglib_SetFont(eglib, &font_FreeFont_FreeMonoBold_15px);
			eglib_AddUnicodeBlockToFont(&font_FreeFont_FreeMonoBold_15px, &unicode_block_FreeFont_FreeMonoBold_15px_Latin1Supplement);	
			break;
			case UCG_FONT_NCENR14_HR:
			
			break;		
			case UCG_FONT_FUB11_TR:
			
			break;	
			case UCG_FONT_FUB11_HR:
			
			break;	
			case UCG_FONT_FUB14_HN:
			
			break;	
			case UCG_FONT_FUB14_HR:
			
			break;	
			case UCG_FONT_FUB14_HF:
			
			break;	
			case UCG_FONT_FUB17_HF:
			
			break;	
			case UCG_FONT_FUB20_HN:
			
			break;	
			case UCG_FONT_FUB20_HR:
			
			break;	
			case UCG_FONT_FUB20_HF:
			
			break;	
			case UCG_FONT_FUB25_HR:
			
			break;	
			case UCG_FONT_FUB35_HN:
			
			break;	
			default: case UCG_FONT_FUB35_HR:
			
			break;	
		}
	}
	
	inline void setPrintPos(ucg_int_t x, ucg_int_t y) { eglib_print_xpos = x; eglib_print_ypos = y; }
	inline void setPrintDir(uint8_t d) { eglib_print_dir = d; }
	
	size_t write(uint8_t c) { 
		int8_t delta;
		const struct glyph_t * g;
		g = eglib_GetGlyph(eglib, wchar_t (c));
		if ( g == NULL) { return (0); }
		eglib_DrawGlyph(eglib, eglib_print_xpos, eglib_print_ypos, g);
		delta = g->advance;
		switch(eglib_print_dir) {
    			case UCG_PRINT_DIR_LR: eglib_print_xpos += delta; break;
    			case UCG_PRINT_DIR_TD: eglib_print_ypos += delta; break;
    			case UCG_PRINT_DIR_RL: eglib_print_xpos -= delta; break;
    			default: case UCG_PRINT_DIR_BU: eglib_print_ypos -= delta; break;
  		}
		return 1;
	}
private:
	int16_t eglib_print_xpos = 0, eglib_print_ypos = 0;
	uint8_t eglib_print_dir = UCG_PRINT_DIR_LR;
	
	// two things done above, rest tbd:
	/*
	ucg->invertDisplay  	// display driver function
	ucg->scrollLines     	// display driver function
	ucg->scrollSetMargins  // display driver function
	ucg->setClipRange	// seems there no clipping concept in eglib
	ucg->setFont		
	ucg->setFontMode
	ucg->setFontPosBottom
	ucg->setFontPosCenter
	ucg->setRedBlueTwist  	// display driver function
	ucg->setRotate180	// Same as clipping, missing fundamental concept in eglib
	ucg->undoClipRange	// seems there no clipping concept in eglib

// proposed font assignments
// looks like there is no transparent font type available in eglib...

// ucg_font_ncenR14_hr
// extern struct font_t font_Adobe_NewCenturySchoolbookRoman_14px;

// ucg_font_9x15B_mf
// extern struct font_t font_FreeFont_FreeMonoBold_15px;
// can be extended with :c:func:`eglib_AddUnicodeBlockToFont` to support
// extra unicode blocks defined below. Creates a set equivalent to the f suffix
// extern const struct glyph_unicode_block_t unicode_block_FreeFont_FreeMonoBold_15px_Latin1Supplement;

// ucg_font_fub11_tr
// extern struct font_t font_FreeFont_FreeSans_11px;
// ucg_font_fub11_hr
// extern const struct glyph_unicode_block_t unicode_block_FreeFont_FreeSans_11px_BasicLatin;

// ucg_font_fub14_hn
// extern struct font_t font_FreeFont_FreeSans_14px;
// ucg_font_fub14_hr
// can be extended with :c:func:`eglib_AddUnicodeBlockToFont` to support
// extra unicode blocks defined below. Creates a set equivalent to the f suffix
// extern const struct glyph_unicode_block_t unicode_block_FreeFont_FreeSans_14px_Latin1Supplement;
// ucg_font_fub14_hf

// ucg_font_fub17_hf
// extern struct font_t font_FreeFont_FreeSans_18px;
// can be extended with :c:func:`eglib_AddUnicodeBlockToFont` to support
// extra unicode blocks defined below. Creates a set equivalent to the f suffix
// extern const struct glyph_unicode_block_t unicode_block_FreeFont_FreeSans_18px_Latin1Supplement;

// ucg_font_fub20_hn
// extern struct font_t font_FreeFont_FreeSans_20px;
// ucg_font_fub20_hr
// ucg_font_fub20_hf
// can be extended with :c:func:`eglib_AddUnicodeBlockToFont` to support
// extra unicode blocks defined below. Creates a set equivalent to the f suffix
// extern const struct glyph_unicode_block_t unicode_block_FreeFont_FreeSans_20px_Latin1Supplement;

// ucg_font_fub25_hr
// extern struct font_t font_FreeFont_FreeSans_26px;

// ucg_font_fub35_hn
// extern struct font_t font_FreeFont_FreeSans_36px;
// ucg_font_fub35_hr



*/
	eglib_t * eglib;
};
