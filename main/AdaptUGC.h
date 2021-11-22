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
	 void begin();
	 void setColor( uint8_t idx, uint8_t r, uint8_t g, uint8_t b ) { eglib_SetIndexColor(eglib, idx, r, g, b); }
	 void setColor( uint8_t r, uint8_t g, uint8_t b ) { eglib_SetIndexColor(eglib, 0, r, g, b); }
	 void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1)  { eglib_DrawLine(eglib, x0, y0, x1, y1); }
	 void drawBox(int16_t x, int16_t y, int16_t w, int16_t h)  { eglib_DrawBox(eglib, x, y, w, h); }
	 void drawFrame(int16_t x, int16_t y, int16_t w, int16_t h)  { eglib_DrawFrame(eglib, x, y, w, h); }
	 void drawHLine(int16_t x, int16_t y, int16_t len)  { eglib_DrawHLine(eglib, x, y, len); }
	 void drawVLine(int16_t x, int16_t y, int16_t len)  { eglib_DrawVLine(eglib, x, y, len); }
	 void drawPixel(int16_t x, int16_t y)  { eglib_DrawPixel(eglib, x, y); }
	 void drawRBox(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)  { eglib_DrawRoundBox(eglib, x, y, w, h, r); }
	 void drawRFrame(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)  { eglib_DrawRoundFrame(eglib, x, y, w, h, r); }
	 void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2)  { eglib_DrawFilledTriangle(eglib, x0, y0, x1, y1, x2, y2); }
	 void drawTetragon(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3)  { eglib_DrawTetragon(eglib, x0, y0, x1, y1, x2, y2, x3, y3); }
	
	void drawCircle(int16_t x, int16_t y, int16_t radius, uint8_t options){                                                                        // adapter
/*		switch( options ){
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
*/
		eglib_DrawCircle(eglib, x, y, radius, options);
	}
	void drawDisc(int16_t x, int16_t y, int16_t radius, uint8_t options){                                                                        // adapter
/*		switch( options ){
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
*/
		eglib_DrawDisc(eglib, x, y, radius, options);
	}
	void setFont(uint8_t *f){                                                                        // adapter
		switch( f[0] ){
			case UCG_FONT_9x15B_MF:
				eglib_SetFont(eglib, &font_FreeFont_FreeMonoBold_15px);
				eglib_AddUnicodeBlockToFont(&font_FreeFont_FreeMonoBold_15px, &unicode_block_FreeFont_FreeMonoBold_15px_Latin1Supplement);	
				break;
			case UCG_FONT_NCENR14_HR:
				eglib_SetFont(eglib, &font_Adobe_NewCenturySchoolbookRoman_20px);
				break;		
			case UCG_FONT_FUB11_TR:
				eglib_SetFont(eglib, &font_FreeFont_FreeSans_16px);
				break;	
			case UCG_FONT_FUB11_HN:
				eglib_SetFont(eglib, &font_FreeFont_FreeSans_16px);
				break;
			case UCG_FONT_FUB11_HR:
				eglib_SetFont(eglib, &font_FreeFont_FreeSans_16px);
				break;	
			case UCG_FONT_FUB14_HN:
				eglib_SetFont(eglib, &font_FreeFont_FreeSans_20px);
				break;
			case UCG_FONT_FUB14_HR:
				eglib_SetFont(eglib, &font_FreeFont_FreeSans_20px);
				break;	
			case UCG_FONT_FUB14_HF:
				eglib_SetFont(eglib, &font_FreeFont_FreeSans_20px);
				eglib_AddUnicodeBlockToFont(&font_FreeFont_FreeSans_14px, &unicode_block_FreeFont_FreeMonoBold_14px_Latin1Supplement);	
			break;	
			case UCG_FONT_FUR14_HF:
				eglib_SetFont(eglib, &font_FreeFont_FreeSans_20px);
				eglib_AddUnicodeBlockToFont(&font_FreeFont_FreeSans_14px, &unicode_block_FreeFont_FreeMonoBold_14px_Latin1Supplement);
			break;
			case UCG_FONT_FUB17_HF:
				eglib_SetFont(eglib, &font_FreeFont_FreeSans_24px);
				eglib_AddUnicodeBlockToFont(&font_FreeFont_FreeSans_18px, &unicode_block_FreeFont_FreeMonoBold_18px_Latin1Supplement);	
			break;	
			case UCG_FONT_FUB20_HN:
				eglib_SetFont(eglib, &font_FreeFont_FreeSans_28px);
				break;	
			case UCG_FONT_FUB20_HR:
				eglib_SetFont(eglib, &font_FreeFont_FreeSans_28px);
				break;				
			case UCG_FONT_FUB20_HF:
				eglib_SetFont(eglib, &font_FreeFont_FreeSans_28px);
				eglib_AddUnicodeBlockToFont(&font_FreeFont_FreeSans_20px, &unicode_block_FreeFont_FreeMonoBold_20px_Latin1Supplement);	
				break;	
			case UCG_FONT_FUB25_HR:
				eglib_SetFont(eglib, &font_FreeFont_FreeSans_36px);
				break;	
			case UCG_FONT_FUB25_HF:
				eglib_SetFont(eglib, &font_FreeFont_FreeSans_36px);
				break;
			case UCG_FONT_FUR25_HF:
				eglib_SetFont(eglib, &font_FreeFont_FreeSans_36px);
				break;
			case UCG_FONT_FUB25_HN:
				eglib_SetFont(eglib, &font_FreeFont_FreeSans_36px);
				break;
			case UCG_FONT_FUB35_HN:
				eglib_SetFont(eglib, &font_FreeFont_FreeSans_54px);
				break;	
			case UCG_FONT_FUB35_HR:
				eglib_SetFont(eglib, &font_FreeFont_FreeSans_54px);
			break;	
			case UCG_FONT_PROFONT22_MR:
				eglib_SetFont(eglib, &font_FreeFont_FreeMono_20px);
			break;	
			default:
				printf("No Font found !\n");
		}
	}
	
	void setPrintPos(int16_t x, int16_t y) { eglib_print_xpos = x; eglib_print_ypos = y; }
	void setPrintDir(uint8_t d) { eglib_print_dir = d; }
	
	size_t write(uint8_t c);
	
	void invertDisplay( bool inv ) {};  	        // display driver function
	void scrollLines(int16_t lines) {};     	    // display driver function
	void scrollSetMargins( int16_t top, int16_t bottom ) {};                 // display driver function
	void setClipRange( int16_t x, int16_t y, int16_t w, int16_t h ) {};	 // seems there no clipping concept in eglib
	void setFontMode( uint8_t is_transparent ) {};  // no concept for transparent fonts in eglib, as it appears
	void setFontPosBottom() {};	                    // same as clipping, no equivalent concept in eglib
	void setFontPosCenter() {};	                    //	"
	void setRedBlueTwist( bool twist ) {};   	    // display driver function
	void setRotate180() {};	                        // Same as clipping, missing fundamental concept in eglib
	void undoClipRange() {};	                    // seems there no clipping concept in eglib
	int16_t getStrWidth( const char * s ) { return ( eglib_GetTextWidth(eglib, s) );}	
	// tbd
	int16_t getFontAscent() { return 20; };
	int16_t getFontDescent() { return 20; };

private:
	int16_t eglib_print_xpos = 0, eglib_print_ypos = 0;
	uint8_t eglib_print_dir = UCG_PRINT_DIR_LR;
	
	// two things done above, rest tbd:
	/*
	ucg->invertDisplay  	// display driver function
	ucg->scrollLines     	// display driver function
	ucg->scrollSetMargins  // display driver function
	ucg->setClipRange	// seems there no clipping concept in eglib
		
	ucg->setFontMode	// no concept for transparent fonts in eglib, as it appears
	ucg->setFontPosBottom	// same as clipping, no equivalent concept in eglib
	ucg->setFontPosCenter	//	"
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
