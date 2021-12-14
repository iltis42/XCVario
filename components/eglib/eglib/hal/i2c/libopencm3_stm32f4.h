#ifndef EGLIB_HAL_I2C_LIBOPENCM3_STM32F4_H
#define EGLIB_HAL_I2C_LIBOPENCM3_STM32F4_H

#include "../../hal.h"
#include <libopencm3/stm32/rcc.h>
#include <stdint.h>

/**
 * Configuration
 * =============
 */

/**
 * Configuration for :c:data:`i2c_libopencm3_stm32f4`.
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

	/** I2C RCC GPIO (eg: ``RCC_GPIOB``). */
	enum rcc_periph_clken rcc_gpio_i2c;
	/** I2C PORT (eg: ``GPIOB``). */
	uint32_t port_i2c;
	/** I2C SCL GPIO (eg: ``GPIO8``). */
	uint16_t gpio_scl;
	/** I2C SDA GPIO (eg: ``GPIO9``). */
	uint16_t gpio_sda;
	/** I2C RCC (eg: ``RCC_I2C1``). */
	enum rcc_periph_clken rcc_i2c;
	/** I2C peripheral (eg: ``I2C1``). */
	uint32_t i2c;
	/**
	 * I2C peripheral clock (eg: ``rcc_apb1_frequency / 1e6``).
	 *
	 * :See also: ``i2c_set_speed`` from libopencm3.
	 */
	uint32_t i2c_peripheral_clock_mhz;
	// TODO I2C Filter
} i2c_libopencm3_stm32f4_config_t;

/**
 * Driver
 * ======
 */

/**
 * I2C HAL driver for `libopencm3 <http://libopencm3.org/>`_ STM32F4 targets.
 *
 * :See also: :c:func:`eglib_Init`.
 */
extern const hal_t i2c_libopencm3_stm32f4;

#endif