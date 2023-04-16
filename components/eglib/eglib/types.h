#ifndef EGLIB_TYPES_H
#define EGLIB_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * These types are used by various eglib functions.
 */

/**
 * Data type for each color channel: red, green, blue.
 *
 * All :doc:`drawing functions <drawing/index>` use this fixed 8-bit color depth and
 * :doc:`display drivers <display/index>` do
 * `color quantization <https://en.wikipedia.org/wiki/Color_quantization>`_ when
 * they only support a lower color depth.
 */
typedef uint8_t color_channel_t;

/** Data type for display coordinates used by :doc:`drawing functions <drawing/index>`. */
typedef int16_t coordinate_t;

// Internal: state for gradient drawing functions
struct _gradient_channel_t {
	color_channel_t color_channel;
	coordinate_t count;
	double step;
};

// Internal: state for gradient drawing functions
struct _gradient_t {
	struct _gradient_channel_t r;
	struct _gradient_channel_t g;
	struct _gradient_channel_t b;
};


/**
 * Pixel format used in-memory by displays.
 *
 * :note: some formats have a difference between the color depth and the bit
 *  depth in memory!
 */

enum pixel_format_t {
	/**
	 * 1bit per pixel where each number of bytes equal to the display width
	 * is a page, and each page has 8 rows: MSB bits are the upper line, LSB
	 * bits are the lower line.
	 */
	PIXEL_FORMAT_1BIT_BW_PAGED,
	/**
	 * 2bits per pixel common across EPD / e-ink displays. The format in memory
	 * are 2 independent 1bit per pixel buffers: the first one for 0=black / 1=white,
	 * the second one for 0=BW / 1=color (eg: red, yellow).
	 */
	PIXEL_FORMAT_2BIT_EPD,
	/**
	 * Each pixel is encoded in 1 byte:
	 *
	 * ``RRRBBBAA``
	 */
	PIXEL_FORMAT_8BIT_RGB,
	/**
	 * Each pair of pixels are packed in 3 bytes:
	 *
	 * ``RRRRGGGG BBBBRRRR GGGGBBBB``
	 */
	PIXEL_FORMAT_12BIT_RGB,
	/**
	 * Each pixel is encoded in 2 bytes:
	 *
	 * ``RRRRRGGG GGGBBBBB``
	 */
	PIXEL_FORMAT_16BIT_RGB,
	/**
	 * Each pixel is encoded in 3 bytes. Note that the 2 least significant bits
	 * of each byte are always ``0``: the color depth is 18bit, but the
	 * in-memory bit depth is 24bit.
	 *
	 * ``RRRRRR00 GGGGGG00 BBBBBB00``
	 */
	PIXEL_FORMAT_18BIT_RGB_24BIT,
	/**
	 * Each pixel is encoded in 3 bytes:
	 *
	 * ``RRRRRRRR GGGGGGGG BBBBBBBB``
	 */
	PIXEL_FORMAT_24BIT_RGB,
	PIXEL_FORMAT_COUNT,
};


/**
 * Color used by :doc:`drawing functions <drawing/index>`.
 */
typedef struct {
	/** Red */
	color_channel_t r;
	/** Green */
	color_channel_t g;
	/** Blue */
	color_channel_t b;
} color_t;



struct _eglib_struct;

/**
 * Handle used by all eglib functions. It wraps the HAL driver, Display driver
 * and internal library state.
 */
typedef struct _eglib_struct eglib_t;

struct hal_struct;

/** Alias for :c:type:`hal_struct`. */
typedef struct hal_struct hal_t;

struct display_struct;

/** Alias for :c:type:`display_struct`. */
typedef struct display_struct display_t;

#endif
