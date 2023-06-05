#ifndef EGLIB_HAL_FOUR_WIRE_SPI_LIBOPENCM3_STM32F4_H
#define EGLIB_HAL_FOUR_WIRE_SPI_LIBOPENCM3_STM32F4_H

#include "../../hal.h"
#include <libopencm3/stm32/rcc.h>
#include <stdint.h>

/**
 * Configuration
 * =============
 */

/**
 * Configuration for :c:data:`four_wire_spi_libopencm3_stm32f4`.
 *
 * :See also: :c:func:`eglib_Init`.
 * :See also: `libopencm3 <http://libopencm3.org/>`_ documentation.
 */
typedef struct {
	/** Optional: reset line RCC (eg: ``RCC_GPIOA``). */
	enum rcc_periph_clken rcc_rst;
	/** Optional: reset line PORT (eg: ``GPIOA``). */
	uint32_t port_rst;
	/** Optional: reset line GPIO (eg: ``GPIO4``). */
	uint16_t gpio_rst;

	/** Optional: busy line RCC (eg: ``RCC_GPIOA``). */
	enum rcc_periph_clken rcc_busy;
	/** Optional: busy line PORT (eg: ``GPIOA``). */
	uint32_t port_busy;
	/** Optional: busy line GPIO (eg: ``GPIO4``). */
	uint16_t gpio_busy;

	/** Data / command line RCC GPIO (eg: ``RCC_GPIOA``). */
	enum rcc_periph_clken rcc_dc;
	/** Data / command line PORT (eg: ``GPIOA``). */
	uint32_t port_dc;
	/** Data / command line GPIO (eg: ``GPIO3``). */
	uint16_t gpio_dc;

	/** SPI CS line RCC GPIO (eg: ``RCC_GPIOA``). */
	enum rcc_periph_clken rcc_cs;
	/** SPI CS line PORT (eg: ``GPIOA``). */
	uint32_t port_cs;
	/** SPI CS line GPIO (eg: ``GPIO2``). */
	uint16_t gpio_cs;

	/** SPI RCC (eg: ``RCC_SPI1``). */
	enum rcc_periph_clken rcc_spi;
	/** SPI peripheral (eg: ``SPI1``). */
	uint32_t spi;

	/** SPI SCK line RCC GPIO (eg: ``RCC_GPIOA``). */
	enum rcc_periph_clken rcc_sck;
	/** SPI SCK line PORT (eg: ``GPIOA``). */
	uint32_t port_sck;
	/** SPI SCK line GPIO (eg: ``GPIO5``). */
	uint16_t gpio_sck;

	/** SPI MOSI line RCC GPIO (eg: ``RCC_GPIOA``). */
	enum rcc_periph_clken rcc_mosi;
	/** SPI MOSI line PORT (eg: ``GPIOA``). */
	uint32_t port_mosi;
	/** SPI MOSI line GPIO (eg: ``GPIO7``). */
	uint16_t gpio_mosi;
} four_wire_spi_libopencm3_stm32f4_config_t;

/**
 * Driver
 * ======
 */

/**
 * 4-Wire SPI HAL driver for `libopencm3 <http://libopencm3.org/>`_ STM32F4 targets.
 *
 * :See also: :c:func:`eglib_Init`.
 */
extern const hal_t four_wire_spi_libopencm3_stm32f4;

#endif