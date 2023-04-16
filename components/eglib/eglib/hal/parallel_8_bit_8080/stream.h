#ifndef EGLIB_HAL_PARALLEL_8_BIT_8080_STREAM_H
#define EGLIB_HAL_PARALLEL_8_BIT_8080_STREAM_H

#include "../../hal.h"
#include <stdio.h>

/**
 * Configuration
 * =============
 */

/**
 * Configuration for :c:data:`parallel_8_bit_8080_stream`. All values are
 * populated during initialization.
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
} parallel_8_bit_8080_stream_config_t;

/**
 * Driver
 * ======
 */

/**
 * Parallel 8-bit 8080 HAL driver that writes function call details to a C
 * stream.
 *
 * This is intended for unit testing display drivers.
 *
 * :See also: :c:func:`eglib_Init`.
 */
extern const hal_t parallel_8_bit_8080_stream;

#endif