#include "AdaptUGC.h"
#include "eglib.h"
#include <eglib/display/ili9341.h>
#include <eglib/hal/four_wire_spi/esp32/esp32_ili9341.h>
#include "sensor.h"
#include "logdef.h"
#include "Setup.h"

#define HSPI 2

static eglib_t myeglib;

const uint8_t ucg_font_9x15B_mf[] = { UCG_FONT_9x15B_MF };
const uint8_t ucg_font_ncenR14_hr[] = { UCG_FONT_NCENR14_HR };
const uint8_t ucg_font_fub11_tr[] = { UCG_FONT_FUB11_TR };
const uint8_t ucg_font_fub11_hr[] = { UCG_FONT_FUB11_HR };
const uint8_t ucg_font_fub14_hn[] = { UCG_FONT_FUB14_HN };
const uint8_t ucg_font_fub14_hr[] = { UCG_FONT_FUB14_HR };
const uint8_t ucg_font_fub14_hf[] = { UCG_FONT_FUB14_HF };
const uint8_t ucg_font_fur14_hf[] = { UCG_FONT_FUR14_HF };
const uint8_t ucg_font_fub17_hf[] = { UCG_FONT_FUB17_HF };
const uint8_t ucg_font_fub20_hn[] = { UCG_FONT_FUB20_HN };
const uint8_t ucg_font_fub20_hr[] = { UCG_FONT_FUB20_HR };
const uint8_t ucg_font_fub20_hf[] = { UCG_FONT_FUB20_HF };
const uint8_t ucg_font_fur20_hf[] = { UCG_FONT_FUR20_HF };
const uint8_t ucg_font_fub25_hr[] = { UCG_FONT_FUB25_HR };
const uint8_t ucg_font_fub25_hf[] = { UCG_FONT_FUB25_HF };
const uint8_t ucg_font_fur25_hf[] = { UCG_FONT_FUR25_HF };
const uint8_t ucg_font_fub35_hn[] = { UCG_FONT_FUB35_HN };
const uint8_t ucg_font_fub35_hr[] = { UCG_FONT_FUB35_HR };
const uint8_t ucg_font_profont22_mr[] = {  UCG_FONT_PROFONT22_MR };
const uint8_t ucg_font_fub25_hn[] = { UCG_FONT_FUB25_HN };
const uint8_t ucg_font_fub11_hn[] = { UCG_FONT_FUB11_HN };
const uint8_t eglib_font_free_sansbold_66[] = { EGLIB_FONT_FREE_SANSBOLD_66 };

static ili9341_config_t ili9341_config = {
		.width = 240,
		.height = 320,
		.color = ILI9341_COLOR_18_BIT,
		.page_address = ILI9341_PAGE_ADDRESS_TOP_TO_BOTTOM,
		.colum_address = ILI9341_COLUMN_ADDRESS_LEFT_TO_RIGHT,
		.page_column_order = ILI9341_PAGE_COLUMN_ORDER_NORMAL,
		.vertical_refresh = ILI9341_VERTICAL_REFRESH_TOP_TO_BOTTOM,
		.horizontal_refresh = ILI9341_HORIZONTAL_REFRESH_LEFT_TO_RIGHT,
};

static esp32_hal_config_t esp32_ili9341_config = {
		.spi_num = 	VSPI,
		.freq = 	13111111*3,  // max 40 MHz
		.dataMode = SPI_MODE0,
		.bitOrder = MSBFIRST,
		.gpio_scl = SPI_SCLK,
		.gpio_sda = SPI_MOSI, // I/O Pin 27
		.gpio_sdo = SPI_MISO, // I/O Pin 32
		.gpio_cs  = CS_Display,
		.gpio_dc  = SPI_DC,
		.gpio_rs  = RESET_Display,
};

void AdaptUGC::setFont(const uint8_t *f, bool filled ){    // adapter
	eglib_setFilledMode( eglib, filled );
	switch( f[0] ){
	case UCG_FONT_9x15B_MF:
		eglib_SetFont(eglib, &font_FreeFont_FreeMonoBold_15px);
		break;
	case UCG_FONT_NCENR14_HR:
		eglib_SetFont(eglib, &font_Adobe_NewCenturySchoolbookRoman_20px);
		break;
	case UCG_FONT_FUB11_TR:
		eglib_SetFont(eglib, &font_Adobe_HelveticaBold_17px);
		break;
	case UCG_FONT_FUB11_HN:
		eglib_SetFont(eglib, &font_FreeFont_FreeSansBold_18px);
		break;
	case UCG_FONT_FUB11_HR:
		eglib_SetFont(eglib, &font_Adobe_HelveticaBold_17px);
		break;
	case UCG_FONT_FUB14_HN:
		eglib_SetFont(eglib, &font_FreeFont_FreeSansBold_18px);
		break;
	case UCG_FONT_FUB14_HR:
		eglib_SetFont(eglib, &font_FreeFont_FreeSansBold_20px);
		break;
	case UCG_FONT_FUB14_HF:
		eglib_SetFont(eglib, &font_FreeFont_FreeSansBold_20px);
		break;
	case UCG_FONT_FUR14_HF:
		eglib_SetFont(eglib, &font_FreeFont_FreeSans_20px);
		break;
	case UCG_FONT_FUB17_HF:
		eglib_SetFont(eglib, &font_FreeFont_FreeSansBold_24px);
		break;
	case UCG_FONT_FUB20_HN:
		eglib_SetFont(eglib, &font_FreeFont_FreeSansBold_28px);
		break;
	case UCG_FONT_FUB20_HR:
		eglib_SetFont(eglib, &font_FreeFont_FreeSansBold_28px);
		break;
	case UCG_FONT_FUB20_HF:
		eglib_SetFont(eglib, &font_FreeFont_FreeSansBold_28px);
		break;
	case UCG_FONT_FUB25_HR:
		eglib_SetFont(eglib, &font_FreeFont_FreeSansBold_32px);
		break;
	case UCG_FONT_FUB25_HF:
		eglib_SetFont(eglib, &font_FreeFont_FreeSansBold_32px);
		break;
	case UCG_FONT_FUR25_HF:
		eglib_SetFont(eglib, &font_FreeFont_FreeSansBold_32px);
		break;
	case UCG_FONT_FUB25_HN:
		eglib_SetFont(eglib, &font_FreeFont_FreeSansBold_32px);
		break;
	case UCG_FONT_FUB35_HN:
		eglib_SetFont(eglib, &font_FreeFont_FreeSansBold_48px);
		break;
	case UCG_FONT_FUB35_HR:
		eglib_SetFont(eglib, &font_FreeFont_FreeSansBold_48px);
		break;
	case EGLIB_FONT_FREE_SANSBOLD_66:
                eglib_SetFont(eglib, &font_FreeFont_FreeSansBold_66px);
                break;
	case UCG_FONT_PROFONT22_MR:
		eglib_SetFont(eglib, &font_FreeFont_FreeMonoBold_20px);
		break;
	default:
		printf("No Font found !\n");
	}
};

void  AdaptUGC::begin() {
	eglib = &myeglib;
	if( display_orientation.get() == DISPLAY_TOPDOWN ){  // default is DISPLAY_NORMAL
		ili9341_config.page_address =  ILI9341_PAGE_ADDRESS_BOTTOM_TO_TOP;
		ili9341_config.colum_address = ILI9341_COLUMN_ADDRESS_RIGHT_TO_LEFT;
	}
	esp32_ili9341_config.freq = rint( FREQ_BMP_SPI * 3 * ((100.0 + display_clock_adj.get())/100.0));
	ESP_LOGI(FNAME, "eglib_Send() &eglib:%x  hal-driv:%x config:%x  clk:%.3f MHz\n", (unsigned int)eglib, (unsigned int)&esp32_ili9341, (unsigned int)&esp32_ili9341_config, (double)(esp32_ili9341_config.freq/1000000.0) );
	eglib_Init( &myeglib, &esp32_ili9341, &esp32_ili9341_config, &ili9341, &ili9341_config );
	setClipRange( 0,0, 240, 320 );
};

void AdaptUGC::advanceCursor( size_t delta ){
	switch(eglib_print_dir) {
	case UCG_PRINT_DIR_LR:
		eglib_print_xpos += delta;
		break;
	case UCG_PRINT_DIR_RL:
		eglib_print_xpos -= delta;
		break;
	case UCG_PRINT_DIR_TD:
		delta = eglib->drawing.font->ascent - eglib->drawing.font->descent;
		eglib_print_ypos += delta;
		break;
	default: case UCG_PRINT_DIR_BU:
		delta = eglib->drawing.font->ascent - eglib->drawing.font->descent;
		eglib_print_ypos -= delta;
	}
	// ESP_LOGI(FNAME, "advanceCursor() delta: %d newc:%d", delta,  eglib_print_xpos );
}

size_t AdaptUGC::write(const uint8_t *buffer, size_t size){
	size_t delta = eglib_DrawText(eglib, eglib_print_xpos, eglib_print_ypos, (const char *)buffer );
	advanceCursor( delta );
	return size;
}

size_t AdaptUGC::write(uint8_t c) {
	size_t delta = 0;
	switch (eglib_font_pos) {
	case UCG_FONT_POS_BOTTOM: {
		const struct font_t *font;
		font = eglib->drawing.font;
		int16_t ypos = eglib_print_ypos + font->descent; // descent is negativ value!
		delta = eglib_DrawWChar(eglib, eglib_print_xpos, ypos, wchar_t (c));
		break;
	}
	case UCG_FONT_POS_CENTER: {
		const struct font_t *font;
		font = eglib->drawing.font;
		int16_t ypos = eglib_print_ypos+(font->ascent/2)+(font->descent);
		delta = eglib_DrawWChar(eglib, eglib_print_xpos, ypos, wchar_t (c));
		break;
	}
	case UCG_FONT_POS_TOP: {
		const struct font_t *font;
		font = eglib->drawing.font;
		int16_t ypos = eglib_print_ypos+font->ascent;
		delta = eglib_DrawWChar(eglib, eglib_print_xpos, ypos, wchar_t (c));
		break;
	}
	default: case UCG_FONT_POS_BASE: {
		delta = eglib_DrawWChar(eglib, eglib_print_xpos, eglib_print_ypos, wchar_t (c));
		break;
	}
	}
	advanceCursor( delta );
	return 1;
};


