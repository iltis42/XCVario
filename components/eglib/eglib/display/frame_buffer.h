#ifndef EGLIB_DISPLAY_FRAME_BUFFER_H
#define EGLIB_DISPLAY_FRAME_BUFFER_H

#include "../hal.h"
#include "../../eglib.h"
#include "../display.h"

/**
 * Configuration
 * =============
 */

/**
 * Configuration for frame buffer display.
 *
 * :c:func:`eglib_Init_FrameBuffer` populates the values here.
 */
typedef struct {
	display_t frame_buffer;
	eglib_t eglib_buffered;
	void *buffer;
	coordinate_t x_start;
	coordinate_t x_end;
	coordinate_t y_start;
	coordinate_t y_end;
} frame_buffer_config_t;

/**
 * Functions
 * =========
 *
 * These functions can be used exclusively with :c:type:`eglib_t` initialized
 * for frame buffer.
 */

/**
 * Works similarly to :c:func:`eglib_Init`, but creates a frame buffer where data
 * is written to, instead of directly to the display. The buffer is only sent to
 * the display when :c:func:`eglib_FrameBuffer_Send` is called.
 *
 * Using a frame buffer makes sense when:
 *
 * - You are using a display that does not support directly writing pixes to
 *   display memory (eg: :c:data:`sh1106_vdd1_2_4_v`).
 * - You have enough RAM.
 * - You want to have faster refresh rates.
 * - You want to avoid visual artifacts when drawing complex shapes.
 *
 * You can not use a frame buffer only when you don't have enough RAM available.
 *
 * This function allocates the memory required for the buffer dynamically based
 * on :c:type:`display_struct` width, height and pixel format (width * height *
 * bit depth / 8). This can be freed
 * by :c:func:`frame_buffer_Free`.
 *
 * :param eglib: Pointer to :c:type:`eglib_t` to be initialized.
 * :param frame_buffer_config: Pointer to :c:type:`frame_buffer_config_t` to be
 *   initialized.
 * :param hal_driver: Pointer to the HAL driver :c:type:`hal_t` to use.
 * :param hal_config_ptr: Pointer to the HAL driver configuration.
 *   Please refer each HAL driver documentation for details.
 * :param display_driver: Pointer to the display driver :c:type:`display_t` to use.
 * :param display_config_ptr: Pointer to the display driver configuration.
 *   Please refer each display driver documentation for details.
 * :return: Pointer to an initialized :c:type:`eglib_t` for the given HAL and
 *   display parameters. This reference **must** be used when using display
 *   specific functions, such as :c:func:`st7789_SetDisplayInversion`.
 */
eglib_t *eglib_Init_FrameBuffer(
	eglib_t *eglib,
	frame_buffer_config_t *frame_buffer_config,
	const hal_t *hal_driver,
	void *hal_config_ptr,
	const display_t *display_driver,
	void *display_config_ptr
);

/** Send part of the frame buffer to the display RAM. */
void eglib_FrameBuffer_SendPartial(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y,
	coordinate_t width, coordinate_t height
);

/** Send the whole frame buffer to the display RAM. */
#define eglib_FrameBuffer_Send(eglib) eglib_FrameBuffer_SendPartial(eglib, 0, 0, eglib_GetWidth(eglib), eglib_GetHeight(eglib))

/** Send only updated parts of the frame buffer to the display RAM. */
void eglib_FrameBuffer_SendUpdated(eglib_t *eglib);

/** Free memory previously allocated by :c:func:`eglib_Init_FrameBuffer`. */
void frame_buffer_Free(eglib_t *eglib);

#endif