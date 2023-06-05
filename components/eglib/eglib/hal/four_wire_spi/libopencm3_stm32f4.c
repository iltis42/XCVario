#include "libopencm3_stm32f4.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include "../common/libopencm3/stm32f4/delay.c"

#define MAX(x,y) ((x) >= (y)) ? (x) : (y)

#define wait_spi_not_busy(spi) while(SPI_SR((uintptr_t)spi) & SPI_SR_BSY);

//
// Helpers
//

static void set_dc(
	eglib_t *eglib,
	bool state
) {
	four_wire_spi_libopencm3_stm32f4_config_t *config;
	hal_four_wire_spi_config_t *four_wire_spi_config_comm;

	config = eglib_GetHalConfig(eglib);
	four_wire_spi_config_comm = eglib_GetHalFourWireSpiConfigComm(eglib);

	wait_spi_not_busy(config->spi);
	_delay_ns(four_wire_spi_config_comm->dc_setup_ns);
	if(state)
		gpio_set(
			config->port_dc,
			config->gpio_dc
		);
	else
		gpio_clear(
			config->port_dc,
			config->gpio_dc
		);
	_delay_ns(four_wire_spi_config_comm->dc_hold_ns);
}

static void set_cs(
	eglib_t *eglib,
	bool state
) {
	four_wire_spi_libopencm3_stm32f4_config_t *config;
	hal_four_wire_spi_config_t *four_wire_spi_config_comm;

	config = eglib_GetHalConfig(eglib);
	four_wire_spi_config_comm = eglib_GetHalFourWireSpiConfigComm(eglib);

	wait_spi_not_busy(config->spi);
	if(state) {
		_delay_ns(four_wire_spi_config_comm->cs_hold_ns);
		gpio_set(
			config->port_cs,
			config->gpio_cs
		);
		_delay_ns(four_wire_spi_config_comm->cs_disable_ns);
	} else {
		gpio_clear(
			config->port_cs,
			config->gpio_cs
		);
		_delay_ns(four_wire_spi_config_comm->cs_setup_ns);
	}
}

//
// Driver
//

static void init(
	eglib_t *eglib
) {
	four_wire_spi_libopencm3_stm32f4_config_t *config;
	hal_four_wire_spi_config_t *four_wire_spi_config_comm;
	uint32_t serial_clk_hz;
	uint32_t br;
	uint32_t cpol = 0;
	uint32_t cpha = 0;
	uint32_t lsbfirst = 0;

	config = eglib_GetHalConfig(eglib);
	four_wire_spi_config_comm = eglib_GetHalFourWireSpiConfigComm(eglib);

	if(config->rcc_rst) {
		rcc_periph_clock_enable(config->rcc_rst);
		gpio_mode_setup(
			config->port_rst,
			GPIO_MODE_OUTPUT,
			GPIO_PUPD_NONE,
			config->gpio_rst
		);
	}

	if(config->rcc_busy) {
		rcc_periph_clock_enable(config->rcc_busy);
		gpio_mode_setup(
			config->port_busy,
			GPIO_MODE_INPUT,
			GPIO_PUPD_NONE,
			config->gpio_busy
		);
	}

	rcc_periph_clock_enable(config->rcc_dc);
	gpio_mode_setup(
		config->port_dc,
		GPIO_MODE_OUTPUT,
		GPIO_PUPD_NONE,
		config->gpio_dc
	);

	rcc_periph_clock_enable(config->rcc_cs);
	gpio_mode_setup(
		config->port_cs,
		GPIO_MODE_OUTPUT,
		GPIO_PUPD_NONE,
		config->gpio_cs
	);

	rcc_periph_clock_enable(config->rcc_sck);
	gpio_mode_setup(
		config->port_sck,
		GPIO_MODE_AF,
		GPIO_PUPD_NONE,
		config->gpio_sck
	);
	gpio_set_af(
		config->port_sck,
		GPIO_AF5,
		config->gpio_sck
	);

	rcc_periph_clock_enable(config->rcc_mosi);
	gpio_mode_setup(
		config->port_mosi,
		GPIO_MODE_AF,
		GPIO_PUPD_NONE,
		config->gpio_mosi
	);
	gpio_set_af(
		config->port_mosi,
		GPIO_AF5,
		config->gpio_mosi
	);

	rcc_periph_clock_enable(config->rcc_spi);

	serial_clk_hz = 1000000000UL / (four_wire_spi_config_comm->sck_cycle_ns);

	if(serial_clk_hz < (rcc_apb2_frequency / 128))
		br = SPI_CR1_BAUDRATE_FPCLK_DIV_256;
	else if(serial_clk_hz < (rcc_apb2_frequency / 64))
		br = SPI_CR1_BAUDRATE_FPCLK_DIV_128;
	else if(serial_clk_hz < (rcc_apb2_frequency / 32))
		br = SPI_CR1_BAUDRATE_FPCLK_DIV_64;
	else if(serial_clk_hz < (rcc_apb2_frequency / 16))
		br = SPI_CR1_BAUDRATE_FPCLK_DIV_32;
	else if(serial_clk_hz < (rcc_apb2_frequency / 8))
		br = SPI_CR1_BAUDRATE_FPCLK_DIV_16;
	else if(serial_clk_hz < (rcc_apb2_frequency / 4))
		br = SPI_CR1_BAUDRATE_FPCLK_DIV_8;
	else if(serial_clk_hz < (rcc_apb2_frequency / 2))
		br = SPI_CR1_BAUDRATE_FPCLK_DIV_4;
	else
		br = SPI_CR1_BAUDRATE_FPCLK_DIV_2;

	switch(four_wire_spi_config_comm->mode) {
		case 0:
			cpol = SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE;
			cpha = SPI_CR1_CPHA_CLK_TRANSITION_1;
			break;
		case 1:
			cpol = SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE;
			cpha = SPI_CR1_CPHA_CLK_TRANSITION_2;
			break;
		case 2:
			cpol = SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE;
			cpha = SPI_CR1_CPHA_CLK_TRANSITION_1;
			break;
		case 3:
			cpol = SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE;
			cpha = SPI_CR1_CPHA_CLK_TRANSITION_2;
			break;
	}

	switch(four_wire_spi_config_comm->bit_numbering) {
		case HAL_LSB_FIRST:
			lsbfirst = SPI_CR1_LSBFIRST;
			break;
		case HAL_MSB_FIRST:
			lsbfirst = SPI_CR1_MSBFIRST;
			break;
	}

	spi_init_master(
		config->spi,
		br,
		cpol,
		cpha,
		SPI_CR1_DFF_8BIT,
		lsbfirst
	);

	spi_enable(config->spi);
}

static void sleep_in(
	eglib_t *eglib
) {
	four_wire_spi_libopencm3_stm32f4_config_t *config;

	config = eglib_GetHalConfig(eglib);

	spi_disable(config->spi);
}

static void sleep_out(
	eglib_t *eglib
) {
	init(eglib);
}

static void delay_ns(
	eglib_t *eglib,
	volatile uint32_t ns
) {
	(void)eglib;

	_delay_ns(ns);
}

static void set_reset(
	eglib_t *eglib,
	bool state
) {
	four_wire_spi_libopencm3_stm32f4_config_t *config;

	config = eglib_GetHalConfig(eglib);

	if(!(config->rcc_rst))
		return;

	if(state)
		gpio_set(config->port_rst, config->gpio_rst);
	else
		gpio_clear(config->port_rst, config->gpio_rst);
}

static bool get_busy(eglib_t *eglib) {
	four_wire_spi_libopencm3_stm32f4_config_t *config;

	config = eglib_GetHalConfig(eglib);

	return gpio_get(config->port_busy, config->gpio_busy);
}

static void comm_begin(eglib_t *eglib) {
	set_cs(eglib, false);
}

static void send(
	eglib_t *eglib,
	enum hal_dc_t dc,
	uint8_t *bytes,
	uint32_t length
) {
	four_wire_spi_libopencm3_stm32f4_config_t *config;

	config = eglib_GetHalConfig(eglib);

	set_dc(eglib, dc);
	
	for(;length;bytes++,length--) {
		spi_send(config->spi, *bytes);
	}
}

static void comm_end(eglib_t *eglib) {
	four_wire_spi_libopencm3_stm32f4_config_t *config;

	config = eglib_GetHalConfig(eglib);

	wait_spi_not_busy(config->spi);
	set_cs(eglib, true);
}

const hal_t four_wire_spi_libopencm3_stm32f4 = {
	.init = init,
	.sleep_in = sleep_in,
	.sleep_out = sleep_out,
	.delay_ns = delay_ns,
	.set_reset = set_reset,
	.get_busy = get_busy,
	.comm_begin = comm_begin,
	.send = send,
	.comm_end = comm_end,
};