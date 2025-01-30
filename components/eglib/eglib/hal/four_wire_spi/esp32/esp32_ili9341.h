#pragma once

/**
 * Driver
 * ======
 */

/**
 * 4-Wire SPI HAL driver for ESP32 ILI9341 four wire SPI IPS display module
 *
 *
 * :See also: :c:func:`eglib_Init`.
 */


#include "hal.h"
#include <driver/gpio.h>

extern hal_t esp32_ili9341;

typedef struct esp32_hal_config{
	uint8_t spi_num;
	uint32_t freq;
	uint8_t dataMode;
	uint8_t bitOrder;
	gpio_num_t gpio_scl;
	gpio_num_t gpio_sda;
	gpio_num_t gpio_sdo;
	gpio_num_t gpio_cs;
	gpio_num_t gpio_dc;
	gpio_num_t gpio_rs;
}esp32_hal_config_t;

// void send( eglib_t *_eglib, enum hal_dc_t dc, uint8_t *bytes, uint32_t length );
