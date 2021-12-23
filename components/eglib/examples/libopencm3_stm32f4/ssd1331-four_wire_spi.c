#include <eglib.h>
#include <eglib/display/frame_buffer.h>
#include <eglib/display/ssd1331.h>
#include <eglib/hal/four_wire_spi/libopencm3_stm32f4.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <stdio.h>

int main(void) {
	eglib_t eglib;
	eglib_t *eglib_ssd1331;

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
	rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_84MHZ]);

	eglib_Init(
		&eglib,
		&four_wire_spi_libopencm3_stm32f4, &four_wire_spi_libopencm3_stm32f4_config,
		&ssd1331, &ssd1331_config_buydisplay_65k_colors
	);

	for(uint16_t y=0;y<SSD1331_HEIGHT;y++) {
		uint8_t v;
		v = y / ((float)SSD1331_HEIGHT-1) * 255;
		eglib_SetIndexColor(&eglib, 0, v, v, v);
		eglib_DrawLine(&eglib,0, y, SSD1331_WIDTH, y);
	}

	eglib_SetIndexColor(&eglib, 0, 255, 255, 255);
	eglib_DrawVLine(&eglib, SSD1331_WIDTH/2, 0, SSD1331_HEIGHT);

	eglib_SetIndexColor(&eglib, 0, 255, 0, 0);
	eglib_DrawLine(&eglib, 0, 0, SSD1331_WIDTH-1, SSD1331_HEIGHT/2);

	eglib_SetIndexColor(&eglib, 0, 0, 255, 0);
	eglib_DrawLine(&eglib,0, 0, SSD1331_WIDTH-1, SSD1331_HEIGHT-1);

	eglib_SetIndexColor(&eglib, 0, 0, 0, 255);
	eglib_DrawLine(&eglib,0, 0, SSD1331_WIDTH/2, SSD1331_HEIGHT-1);

	// Accelerated versions

	eglib_SetIndexColor(&eglib, 0, 0, 255, 255);
	ssd1331_DrawLine(&eglib, 0, 63, 95, 0);

	eglib_SetIndexColor(&eglib, 0, 255, 255, 0);
	ssd1331_DrawFrame(&eglib, 10, 20, 10, 20);

	eglib_SetIndexColor(&eglib, 0, 255, 0, 255);
	ssd1331_DrawFrame(&eglib, 0, 16, 95, 32);

	eglib_SetIndexColor(&eglib, 0, 255, 255, 255);
	ssd1331_DrawBox(&eglib, 40, 20, 40, 20);

	eglib_SetIndexColor(&eglib, 0, 0, 0, 0);
	ssd1331_DrawFrame(&eglib, 22, 20, 10, 20);
}