#ifndef EGLIB_HAL_FOUR_WIRE_SPI_NONE_H
#define EGLIB_HAL_FOUR_WIRE_SPI_NONE_H

#include "../../hal.h"

/**
 * Driver
 * ======
 */

/**
 * 4-Wire SPI HAL driver that does nothing.
 *
 * This is useful for testing only.
 *
 * :See also: :c:func:`eglib_Init`.
 */
extern const hal_t four_wire_spi_none;

#endif