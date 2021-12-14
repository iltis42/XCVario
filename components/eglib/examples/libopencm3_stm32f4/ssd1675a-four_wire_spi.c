#include <eglib.h>
#include <eglib/hal/four_wire_spi/libopencm3_stm32f4.h>
#include <eglib/display/frame_buffer.h>
#include <eglib/display/ssd1675a.h>
#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

int main(void) {
	eglib_t eglib;
	eglib_t *eglib_ssd1675a;

	frame_buffer_config_t frame_buffer_config;

	four_wire_spi_libopencm3_stm32f4_config_t  four_wire_spi_libopencm3_stm32f4_config = {
		// rst
		.rcc_rst = RCC_GPIOA,
		.port_rst = GPIOA,
		.gpio_rst = GPIO4,
		// busy
		.rcc_busy = RCC_GPIOB,
		.port_busy = GPIOB,
		.gpio_busy = GPIO2,
		// dc
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

	eglib_Init_FrameBuffer(
		&eglib, &frame_buffer_config,
		&four_wire_spi_libopencm3_stm32f4, &four_wire_spi_libopencm3_stm32f4_config,
		&ssd1675a, &ssd1675a_config_buydisplay_red_2_9_inch_e_ink_display_module_fast_update
	);

	eglib_SetIndexColor(&eglib, 0, 255, 0, 0);
	eglib_DrawLine(&eglib,0, 0, ssd1675a_config_buydisplay_red_2_9_inch_e_ink_display_module_fast_update.width-1, ssd1675a_config_buydisplay_red_2_9_inch_e_ink_display_module_fast_update.height-1);
	eglib_DrawLine(&eglib,0, ssd1675a_config_buydisplay_red_2_9_inch_e_ink_display_module_fast_update.height-1, ssd1675a_config_buydisplay_red_2_9_inch_e_ink_display_module_fast_update.width-1, 0);

	eglib_SetIndexColor(&eglib, 0, 255, 255, 255);
	eglib_SetFont(&eglib, &font_Adobe_Helvetica_14px);
	eglib_DrawText(&eglib, 10, 14, "eglib");

	eglib_FrameBuffer_Send(&eglib);
	while(eglib_Refresh(&eglib));
}