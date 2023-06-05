#ifndef EGLIB_HAL_PARALLEL_8_BIT_8080_LIBOPENCM3_STM32F4_H
#define EGLIB_HAL_PARALLEL_8_BIT_8080_LIBOPENCM3_STM32F4_H

#include "../../hal.h"
#include <libopencm3/stm32/rcc.h>
#include <stdint.h>

/**
 * Configuration
 * =============
 */

/**
 * Configuration for :c:data:`parallel_8_bit_8080_libopencm3_stm32f4`.
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

	/** CSX line RCC GPIO (eg: ``RCC_GPIOA``). */
	enum rcc_periph_clken rcc_csx;
	/** CSX line PORT (eg: ``GPIOA``). */
	uint32_t port_csx;
	/** CSX line GPIO (eg: ``GPIO2``). */
	uint16_t gpio_csx;

	/** D/CX line RCC GPIO (eg: ``RCC_GPIOA``). */
	enum rcc_periph_clken rcc_dcx;
	/** D/CX line PORT (eg: ``GPIOA``). */
	uint32_t port_dcx;
	/** D/CX line GPIO (eg: ``GPIO2``). */
	uint16_t gpio_dcx;

	/** RDX line RCC GPIO (eg: ``RCC_GPIOA``). */
	enum rcc_periph_clken rcc_rdx;
	/** RDX line PORT (eg: ``GPIOA``). */
	uint32_t port_rdx;
	/** RDX line GPIO (eg: ``GPIO2``). */
	uint16_t gpio_rdx;

	/** WRX line RCC GPIO (eg: ``RCC_GPIOA``). */
	enum rcc_periph_clken rcc_wrx;
	/** WRX line PORT (eg: ``GPIOA``). */
	uint32_t port_wrx;
	/** WRX line GPIO (eg: ``GPIO2``). */
	uint16_t gpio_wrx;

	/** Data (D[7:0]) RCC GPIO (eg: ``RCC_GPIOA``). */
	enum rcc_periph_clken rcc_data;
	/** Data (D[7:0]) PORT (eg: ``GPIOA``). */
	uint32_t port_data;
} parallel_8_bit_8080_libopencm3_stm32f4_config_t;

/**
 * Driver
 * ======
 */

/**
 * I2C HAL driver for `libopencm3 <http://libopencm3.org/>`_ STM32F4 targets.
 *
 * :See also: :c:func:`eglib_Init`.
 */
extern const hal_t parallel_8_bit_8080_libopencm3_stm32f4;

#endif