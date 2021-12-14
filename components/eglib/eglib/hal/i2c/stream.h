#ifndef EGLIB_HAL_I2C_STREAM_H
#define EGLIB_HAL_I2C_STREAM_H

#include "../../hal.h"
#include <stdio.h>

/**
 * Configuration
 * =============
 */

/**
 * Configuration for :c:data:`i2c_stream`. All values are populated
 * during initialization.
 *
 * :See also: :c:func:`eglib_Init`.
 */
typedef struct {
	/** Stream created with `open_memstream()`. */
	FILE *stream;
	/** Stream buffer (available after `fflush()` or `fclose()`). */
	char *buffer;
	/** Size of the buffer (available after `fflush()` or `fclose()`). */
	size_t sizeloc;
} i2c_stream_config_t;

/**
 * Driver
 * ======
 */

/**
 * 4-Wire SPI HAL driver that writes function call details to a C stream.
 *
 * This is intended for unit testing display drivers.
 *
 * :See also: :c:func:`eglib_Init`.
 */
extern const hal_t i2c_stream;

#endif