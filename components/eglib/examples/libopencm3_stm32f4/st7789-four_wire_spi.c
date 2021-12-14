#include <eglib.h>
#include <eglib/display/frame_buffer.h>
#include <eglib/display/st7789.h>
#include <eglib/hal/four_wire_spi/libopencm3_stm32f4.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <stdio.h>

int main(void) {
	eglib_t eglib;
	eglib_t *eglib_st7789;

	four_wire_spi_libopencm3_stm32f4_config_t  four_wire_spi_libopencm3_stm32f4_config = {
		// rst
		.rcc_rst = RCC_GPIOA,
		.port_rst = GPIOA,
		.gpio_rst = GPIO4,
		// cd
		.rcc_dc = RCC_GPIOA,
		.port_dc = GPIOA,
		.gpio_dc = GPIO3,
		// cs
		.rcc_cs = RCC_GPIOA,
		.port_cs = GPIOA,
		.gpio_cs = GPIO2,
		// spi
		.rcc_spi = RCC_SPI1,
		.spi = SPI1,
		// sck
		.rcc_sck = RCC_GPIOA,
		.port_sck = GPIOA,
		.gpio_sck = GPIO5,
		// mosi
		.rcc_mosi = RCC_GPIOA,
		.port_mosi = GPIOA,
		.gpio_mosi = GPIO7,
	};

	st7789_config_t st7789_config = {
		.width = 240,
		.height = 240,
		.color = ST7789_COLOR_16_BIT,
		.page_address = ST7789_PAGE_ADDRESS_TOP_TO_BOTTOM,
		.colum_address = ST7789_COLUMN_ADDRESS_LEFT_TO_RIGHT,
		.page_column_order = ST7789_PAGE_COLUMN_ORDER_NORMAL,
		.vertical_refresh = ST7789_VERTICAL_REFRESH_TOP_TO_BOTTOM,
		.horizontal_refresh = ST7789_HORIZONTAL_REFRESH_LEFT_TO_RIGHT,
	};

	rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_84MHZ]);

	frame_buffer_config_t frame_buffer_config;
	eglib_st7789 = eglib_Init_FrameBuffer(
		&eglib, &frame_buffer_config,
		&four_wire_spi_libopencm3_stm32f4, &four_wire_spi_libopencm3_stm32f4_config,
		&st7789, &st7789_config
	);

	for(uint16_t y=0;y<st7789_config.width;y++) {
		uint8_t v;
		v = y / (float)st7789_config.width * 255;
		eglib_SetIndexColor(&eglib, 0, v, v, v);
		eglib_DrawLine(&eglib,0, y, st7789_config.width, y);
	}

	eglib_SetIndexColor(&eglib, 0, 255, 0, 0);
	eglib_DrawLine(&eglib,0, 0, st7789_config.width-1, st7789_config.height/2);

	eglib_SetIndexColor(&eglib, 0, 0, 255, 0);
	eglib_DrawLine(&eglib,0, 0, st7789_config.width-1, st7789_config.height-1);

	eglib_SetIndexColor(&eglib, 0, 0, 0, 255);
	eglib_DrawLine(&eglib,0, 0, st7789_config.width/2, st7789_config.height-1);

	eglib_FrameBuffer_SendPartial(
		&eglib,
		st7789_config.width / 8, st7789_config.height / 8,
		3 * st7789_config.width / 4, 3 * st7789_config.height / 4
	);
	
	st7789_SetDisplayInversion(eglib_st7789, true);
}