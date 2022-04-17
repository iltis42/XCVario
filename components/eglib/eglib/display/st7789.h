#ifndef EGLIB_DISPLAY_ST7789_H
#define EGLIB_DISPLAY_ST7789_H

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
	ST7789_COLOR_12_BIT,
	/** 16 bit */
	ST7789_COLOR_16_BIT,
	/** 18 bit (encoded as 24 bit in memory) */
	ST7789_COLOR_18_BIT,
} st7789_color_t;

/** Memory data access control: page address order. */
typedef enum {
	/** Top to bottom. */
	ST7789_PAGE_ADDRESS_TOP_TO_BOTTOM,
	/** Bottom to top. */
	ST7789_PAGE_ADDRESS_BOTTOM_TO_TOP,
} st7789_page_address_t;

/** Memory data access control: column address order. */
typedef enum {
	/** Left to right. */
	ST7789_COLUMN_ADDRESS_LEFT_TO_RIGHT,
	/** Right to left. */
	ST7789_COLUMN_ADDRESS_RIGHT_TO_LEFT,
} st7789_colum_address_t;

/** Memory data access control: page/column order. */
typedef enum {
	/** Normal mode */
	ST7789_PAGE_COLUMN_ORDER_NORMAL,
	/** Reverse mode */
	ST7789_PAGE_COLUMN_ORDER_REVERSE,
} st7789_page_colum_order_t;


/** Memory data access control: line address order. */
typedef enum {
	/** LCD refresh top to bottom. */
	ST7789_VERTICAL_REFRESH_TOP_TO_BOTTOM,
	/** LCD refresh bottom to top. */
	ST7789_VERTICAL_REFRESH_BOTTOM_TO_TOP,
} st7789_vertical_refresh_t;

/** Memory data access control: display data latch data order. */
typedef enum {
	/** LCD refresh left to right. */
	ST7789_HORIZONTAL_REFRESH_LEFT_TO_RIGHT,
	/** LCD refresh right to left. */
	ST7789_HORIZONTAL_REFRESH_RIGHT_TO_LEFT,
} st7789_horizontal_refresh_t;

/**
 * Configuration
 * =============
 */

/**
 * Configuration for :c:data:`st7789`.
 *
 * :See also: :c:func:`eglib_Init`.
 */
typedef struct {
	/** Display width. */
	coordinate_t width;
	/** Display height. */
	coordinate_t height;
	/** Display color depth. */
	st7789_color_t color : 2;
	/** Page address order. */
	st7789_page_address_t page_address : 1;
	/** Column address order. */
	st7789_colum_address_t colum_address : 1;
	/** Page/column order. */
	st7789_page_colum_order_t page_column_order : 1;
	/** Vertical refresh direction. */
	st7789_vertical_refresh_t vertical_refresh : 1;
	/** Horizontal refresh direction. */
	st7789_horizontal_refresh_t horizontal_refresh : 1;
} st7789_config_t;

/**
 * Driver
 * ======
 */

/**
 * ST7789 display driver.
 *
  * This driver supports the following data buses:
 *
 * - 4-Wire SPI.
 *
 * :See also: :c:func:`eglib_Init`.
 * :See also: `Datasheet <https://www.newhavendisplay.com/appnotes/datasheets/LCDs/ST7789V.pdf>`_.
 */
extern const display_t st7789;

/**
 * Functions
 * =========
 *
 * These functions can be used exclusively with :c:type:`eglib_t` initialized
 * with :c:data:`st7789`.
 */

/**Set display inversion on/off. */
void st7789_SetDisplayInversion(eglib_t *eglib, bool inversion);

/**
 * Set display idle mode on/off.
 *
 * When in idle on mode, colors are reduced to 8.
 */
void st7789_SetIdleMode(eglib_t *eglib, bool idle);

#endif