#include "AdaptUGC.h"
#include "eglib.h"
#include <eglib/display/st7789.h>
#include <eglib/hal/four_wire_spi/esp32/esp32_ili9341.h>
#include "sensor.h"

#define HSPI 2

static eglib_t myeglib;

void  AdaptUGC::begin() {
	st7789_config_t st7789_config = {
			.width = 240,
			.height = 320,
			.color = ST7789_COLOR_16_BIT,
			.page_address = ST7789_PAGE_ADDRESS_TOP_TO_BOTTOM,
			.colum_address = ST7789_COLUMN_ADDRESS_LEFT_TO_RIGHT,
			.page_column_order = ST7789_PAGE_COLUMN_ORDER_NORMAL,
			.vertical_refresh = ST7789_VERTICAL_REFRESH_TOP_TO_BOTTOM,
			.horizontal_refresh = ST7789_HORIZONTAL_REFRESH_LEFT_TO_RIGHT,
	};

	esp32_hal_config_t esp32_ili9341_config = {
			.spi_num = 	HSPI,
			.freq = 	13111111*3,  // max 40 MHz
			.dataMode = SPI_MODE3,
			.bitOrder = MSBFIRST,
			.gpio_scl = SPI_SCLK,
			.gpio_sda = SPI_MOSI,
			.gpio_sdi = SPI_MISO,
			.gpio_cs  = CS_Display,
			.gpio_dc  = SPI_DC,
			.gpio_rs  = RESET_Display,
	};
	eglib_Init( &myeglib, &esp32_ili9341, &esp32_ili9341_config, &st7789, &st7789_config );
};


