#ifndef EGLIB_DISPLAY_H
#define EGLIB_DISPLAY_H

#include "../eglib.h"
#include "hal.h"

/**
 * Types
 * =====
 */

/**
 * Line direction to draw.
 */
enum display_line_direction_t {
	/** Right: x increments. */
	DISPLAY_LINE_DIRECTION_RIGHT,
	/** Right: x decrements. */
	DISPLAY_LINE_DIRECTION_LEFT,
	/** Right: y increments. */
	DISPLAY_LINE_DIRECTION_DOWN,
	/** Right: y decrements. */
	DISPLAY_LINE_DIRECTION_UP,
};

/**
 * Communication bus configuration required by the display.
 *
 * Buses not supported by the display can be set to ``NULL``.
 */
struct display_comm_struct {
	/** 4-Wire SPI */
	hal_four_wire_spi_config_t *four_wire_spi;
	/** I2C */
	hal_i2c_config_t *i2c;
	/** Parallel 8-bit 8080 */
	hal_parallel_8_bit_8080_config_t *parallel_8_bit_8080;
};

/**
 * Display driver
 * ==============
 */

/**
 * Display driver definition.
 *
 * Communication with the data bus can be done using the
 * :ref:`display driver HAL interface functions <Display driver HAL interface functions>`
 *
 * :note: :c:func:`eglib_GetDisplayConfig` can be used to retrieve the HAL driver
 *   configuration.
 *
 * Aliased as :c:type:`display_t`.
 */
struct display_struct {
	/** Communication bus configuration required by the display. */
	struct display_comm_struct comm;
	/**
	 * Pointer to a function that initializes the display based on the driver
	 * configuration (:c:func:`eglib_GetDisplayConfig`).
	 *
	 * It must initialize the display memory to all black pixels.
	 */
	void (*init)(eglib_t *eglib);
	/** Pointer to a function that puts the display in sleep mode. */
	void (*sleep_in)(eglib_t *eglib);
	/** Pointer to a function that puts the display out of sleep mode. */
	void (*sleep_out)(eglib_t *eglib);
	/**
	 * Pointer to a function that returns the display dimensions:
	 *
	 * :param eglib: :c:type:`eglib_t` handle.
	 * :param width: Pointer where to write display width to.
	 * :param height: Pointer where to write display height to.
	 * :return: ``void``.
	 */
	void (*get_dimension)(
		eglib_t *eglib,
		coordinate_t *width,
		coordinate_t *height
	);
	/**
	 * Pointer to a function that returns the pixel format used in-memory by
	 * the display.
	 *
	 * This is used by the frame buffer (:c:type:`eglib_Init_FrameBuffer`) to
	 * decide how to write pixels to the buffer, which will then be handled by
	 * the ``send_buffer`` function from :c:type:`display_struct`.
	 *
	 * :param eglib: :c:type:`eglib_t` handle.
	 * :param pixel_format: Pointer to :c:type:`pixel_format_t` where to write
	 *   pixel format to.
	 * :return: :c:type:`pixel_format_t`.
	 */
	void (*get_pixel_format)(eglib_t *eglib, enum pixel_format_t *pixel_format);
	/**
	 * Pointer to a function that that draws directly to the display memory at
	 * ``(x, y)`` using given ``color``.
	 *
	 * Some displays can not support this method. Eg: black and white displays,
	 * where each bit is a pixel, requires reading a whole byte from the display
	 * memory, changing the required bit and sending it back to the display
	 * memory. As Eglib does not support reading the display memory, such
	 * displays must implement this as an empty function:
	 * :c:type:`eglib_Init_FrameBuffer` can be used to interface with the
	 * display using a frame buffer.
	 *
	 * :param eglib: :c:type:`eglib_t` handle.
	 * :param x: X :c:type:`coordinate_t`.
	 * :param y: Y :c:type:`coordinate_t`.
	 * :param color: Color :c:type:`color_t`.
	 */
	void (*draw_pixel_color)(
		eglib_t *eglib,
		coordinate_t x,
		coordinate_t y,
		color_t color
	);
	/**
	 * Pointer to a function that draws a line directly to the display memory.
	 *
	 * This is meant to be an accelerator for drawing lines for displays that
	 * support auto increment/decrement of the memory address written.
	 *
	 * A typical implementation should roughly look like:
	 *
	 * .. code-block:: c
	 *
	 *   eglib_CommBegin(eglib);
	 *   switch(direction) {
	 *       case DISPLAY_LINE_DIRECTION_RIGHT:
	 *           set_display_memory_address_auto_increment_x(eglib);
	 *           break;
	 *       case DISPLAY_LINE_DIRECTION_LEFT:
	 *           set_display_memory_address_auto_decrement_x(eglib);
	 *           break;
	 *       case DISPLAY_LINE_DIRECTION_DOWN:
	 *           set_display_memory_address_auto_increment_y(eglib);
	 *           break;
	 *       case DISPLAY_LINE_DIRECTION_UP:
	 *           set_display_memory_address_auto_decrement_y(eglib);
	 *           break;
	 *   }
	 *   set_display_memory_address(eglib, x, y);
	 *   while(length--)
	 *       send_pixel_to_memory(eglib, get_next_color(eglib));
	 *   eglib_CommEnd(eglib);
	 *
	 * For displays that **do not** support auto increment/decrement of x/y
	 * (for all or some cases) the implementation can be set to
	 * :c:func:`display_default_draw_line`.
	 *
	 * :param eglib: :c:type:`eglib_t` handle.
	 * :param x: line x start.
	 * :param y: line y start.
	 * :param direction: Line :c:type:`enum display_line_direction_t` (right, left, down up).
	 * :param length: Line length in pixels.
	 * :param get_next_color: Pointer to a function that must be called for each
	 *   line pixel to get its color.
	 */
	void (*draw_line)(
		eglib_t *eglib,
		coordinate_t x,
		coordinate_t y,
		enum display_line_direction_t direction,
		coordinate_t length,
		color_t (*get_next_color)(eglib_t *eglib)
	);
	/**
	 * Pointer to a function that sends given buffer to display memory.
	 *
	 * :param eglib: :c:type:`eglib_t` handle.
	 * :param buffer: Pointer to the memory buffer to be sent. The format is
	 *   dependent on both :c:type:`pixel_format_t` from ``get_pixel_format``
	 *   and the display dimensions from ``get_dimension``.
	 * :param x: X :c:type:`coordinate_t` of data to be sent: X pixels lower
	 *    than ``x`` must not be sent to the display.
	 * :param y: Y :c:type:`coordinate_t` of data to be sent: Y pixels lower
	 *    than ``y`` must not be sent to the display.
	 * :param width: Width of data to be sent: X pixels greater than
	 *   ``x + width`` must not be sent to the display.
	 * :param height: Height of data to be sent: Y pixels greater than
	 *   ``y + height`` must not be sent to the display.
	 */
	void (*send_buffer)(
		eglib_t *eglib,
		void *buffer,
		coordinate_t x, coordinate_t y,
		coordinate_t width, coordinate_t height
	);
	/**
	 * Pointer to a function that refreshes the display image.
	 *
	 * This method is only relevant to displays such as e-ink / e-paper, where
	 * writing to the display memory (``draw_pixel_color`` / ``send_buffer``)
	 * does not affect the displayed image: it needs to be sent a *refresh*
	 * command to do so. Other displays (eg: LCD, OLED) can implement this
	 * function with an empty body returning ``false``.
	 *
	 * A typical implementation of this function will issue the refresh command
	 * to the display when first called and then poll the busy data line to
	 * return ``true`` when refresh is ongoing and ``false`` when refresh finished:
	 *
	 * .. code-block:: c
	 *
	 *   if(eglib_IsRefreshing(eglib)) {
	 *   	return eglib_GetBusy(eglib);
	 *   } else {
	 *   	send_refresh_command_to_display(eglib);
	 *   	return true;
	 *   }
	 *
	 * :c:func:`eglib_IsRefreshing` queries the current state of refresh (that
	 * eglib keeps track) and :c:func:`eglib_GetBusy` can be used no read the
	 * state of the busy data line.
	 *
	 * User code can then refresh the display with:
	 *
	 * .. code-block:: c
	 *
	 *   while(eglib_Refresh(eglib));
	 *
	 * :param eglib: :c:type:`eglib_t` handle.
	 * :return: ``true`` when refresh is ongoing and ``false`` when refresh finished.
	 */
	bool (*refresh)(eglib_t *eglib);
};

/**
 * Display Driver Helper Functions
 * -------------------------------
 */

/**
 * Not accelerated implementation of :c:type:`display_struct` ``draw_line``
 * based on ``draw_pixel_color``.
 */
void display_default_draw_line(
	eglib_t *eglib,
	coordinate_t x,
	coordinate_t y,
	enum display_line_direction_t direction,
	coordinate_t length,
	color_t (*get_next_color)(eglib_t *eglib)
);

/**
 * Returns a pointer to the display driver configuration that was passed to
 * :c:func:`eglib_Init`.
 */
#define eglib_GetDisplayConfig(eglib) ((eglib)->display.config_ptr)

/**
 * HAL Driver Helper Functions
 * ===========================
 *
 * These functions are used by :doc:`HAL Drivers <../hal/index>` to interface with the HAL definitions
 * :c:type:`display_comm_struct` from the display driver :c:type:`display_struct`.
 */

/**
 * Returns :c:type:`hal_four_wire_spi_config_t` for :c:type:`display_struct`
 * from given :c:type:`eglib_t`.
 */
#define eglib_GetHalFourWireSpiConfigComm(eglib) (\
	(eglib)->display.driver->comm.four_wire_spi \
)

/**
 * Returns :c:type:`hal_i2c_config_t` for :c:type:`display_struct`
 * from given :c:type:`eglib_t`.
 */
#define eglib_GetHalI2cConfigComm(eglib) (\
	(eglib)->display.driver->comm.i2c \
)

/**
 * Returns :c:type:`hal_parallel_8_bit_8080_t` for :c:type:`display_struct`
 * from given :c:type:`eglib_t`.
 */
#define eglib_GetHalParallel8bit8080ConfigComm(eglib) (\
	(eglib)->display.driver->comm.parallel_8_bit_8080 \
)


/**
 * See :c:type:`hal_i2c_config_t` ``get_7bit_slave_addr`` for details on this
 *   function.
 */
#define eglib_GetI2c7bitSlaveAddr(eglib, dc) \
	(eglib_GetHalI2cConfigComm(eglib)->get_7bit_slave_addr(eglib, dc))

/**
 * :See also: :c:type:`hal_i2c_config_t` ``send`` for details on this
 *   function.
 */
#define eglib_I2cSend(eglib, i2c_write, dc, bytes, length) \
	(eglib_GetHalI2cConfigComm(eglib)->send(eglib, i2c_write, dc, bytes, length))

/**
 * Display Functions
 * =================
 *
 * These functions can be used by end users to query and send commands to the
 * display driver.
 */

/**
 * Get display dimensions.
 *
 * :param eglib: :c:type:`eglib_t` handle.
 * :param width: Pointer where to write display width to.
 * :param height: Pointer where to write display height to.
 */
#define eglib_GetDimension(eglib, width, height) ( \
	(eglib)->display.driver->get_dimension(eglib, width, height) \
)

/**
 * Returns the pixel format used in-memory by the display.
 *
 * :param eglib: :c:type:`eglib_t` handle.
 * :param pixel_format: Pointer to :c:type:`pixel_format_t` where to write
 *   pixel format to.
 * :return: :c:type:`pixel_format_t`.
 */
#define eglib_GetPixelFormat(eglib, pixel_format) ( \
	(eglib)->display.driver->get_pixel_format(eglib, pixel_format) \
)

/** Returns display width as :c:type:`coordinate_t`. */
coordinate_t eglib_GetWidth(eglib_t *eglib);

/** Returns display height as :c:type:`coordinate_t`. */
coordinate_t eglib_GetHeight(eglib_t *eglib);

/**
 * Refreshes the display image. Only applicable to some displays (eg: e-ink /
 * e-paper). Typical usage:
 *
 * .. code-block:: c
 *
 *   while(eglib_Refresh(eglib));
 *
 * :param eglib: :c:type:`eglib_t` handle.
 * :return: ``true`` when refresh is ongoing and ``false`` when refresh finished.
 */
bool eglib_Refresh(eglib_t *eglib);

/** Whether a display refresh initiated by :c:func:`eglib_Refresh` is ongoing */
#define eglib_IsRefreshing(eglib) ((eglib)->display.refreshing)

#endif