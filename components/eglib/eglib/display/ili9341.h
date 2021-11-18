#ifndef EGLIB_DISPLAY_ILI9341_H
#define EGLIB_DISPLAY_ILI9341_H

#include "../../eglib.h"
#include <stdbool.h>

/**
 * Types
 * =====
 */

/**
 * Display color depth. Higher color depths require more data to be transfered,
 * meaning slower refresh rates and more memory required when using a frame
 * buffer.
 */
typedef enum {
	/** 12 bit */
	ILI9341_COLOR_12_BIT,
	/** 16 bit */
	ILI9341_COLOR_16_BIT,
	/** 18 bit (encoded as 24 bit in memory) */
	ILI9341_COLOR_18_BIT,
} ili9341_color_t;

/** Memory data access control: page address order. */
typedef enum {
	/** Top to bottom. */
	ILI9341_PAGE_ADDRESS_TOP_TO_BOTTOM,
	/** Bottom to top. */
	ILI9341_PAGE_ADDRESS_BOTTOM_TO_TOP,
} ili9341_page_address_t;

/** Memory data access control: column address order. */
typedef enum {
	/** Left to right. */
	ILI9341_COLUMN_ADDRESS_LEFT_TO_RIGHT,
	/** Right to left. */
	ILI9341_COLUMN_ADDRESS_RIGHT_TO_LEFT,
} ili9341_colum_address_t;

/** Memory data access control: page/column order. */
typedef enum {
	/** Normal mode */
	ILI9341_PAGE_COLUMN_ORDER_NORMAL,
	/** Reverse mode */
	ILI9341_PAGE_COLUMN_ORDER_REVERSE,
} ili9341_page_colum_order_t;


/** Memory data access control: line address order. */
typedef enum {
	/** LCD refresh top to bottom. */
	ILI9341_VERTICAL_REFRESH_TOP_TO_BOTTOM,
	/** LCD refresh bottom to top. */
	ILI9341_VERTICAL_REFRESH_BOTTOM_TO_TOP,
} ili9341_vertical_refresh_t;

/** Memory data access control: display data latch data order. */
typedef enum {
	/** LCD refresh left to right. */
	ILI9341_HORIZONTAL_REFRESH_LEFT_TO_RIGHT,
	/** LCD refresh right to left. */
	ILI9341_HORIZONTAL_REFRESH_RIGHT_TO_LEFT,
} ili9341_horizontal_refresh_t;

/**
 * Configuration
 * =============
 */

/**
 * Configuration for :c:data:`ili9341`.
 *
 * :See also: :c:func:`eglib_Init`.
 */
typedef struct {
	/** Display width. */
	coordinate_t width;
	/** Display height. */
	coordinate_t height;
	/** Display color depth. */
	ili9341_color_t color : 2;
	/** Page address order. */
	ili9341_page_address_t page_address : 1;
	/** Column address order. */
	ili9341_colum_address_t colum_address : 1;
	/** Page/column order. */
	ili9341_page_colum_order_t page_column_order : 1;
	/** Vertical refresh direction. */
	ili9341_vertical_refresh_t vertical_refresh : 1;
	/** Horizontal refresh direction. */
	ili9341_horizontal_refresh_t horizontal_refresh : 1;
} ili9341_config_t;

/**
 * Driver
 * ======
 */

/**
 * ILI9341 display driver.
 *
  * This driver supports the following data buses:
 *
 * - 4-Wire SPI.
 *
 * :See also: :c:func:`eglib_Init`.
 * :See also: `Datasheet <https://www.newhavendisplay.com/appnotes/datasheets/LCDs/ILI9341V.pdf>`_.
 */
extern const display_t ili9341;

/**
 * Functions
 * =========
 *
 * These functions can be used exclusively with :c:type:`eglib_t` initialized
 * with :c:data:`ili9341`.
 */

/**Set display inversion on/off. */
void ili9341_SetDisplayInversion(eglib_t *eglib, bool inversion);

/**
 * Set display idle mode on/off.
 *
 * When in idle on mode, colors are reduced to 8.
 */
void ili9341_SetIdleMode(eglib_t *eglib, bool idle);

#endif
