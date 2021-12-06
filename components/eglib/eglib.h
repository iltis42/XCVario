#ifndef EGLIB_H
#define EGLIB_H

// #include "eglib/config.h"
#include "eglib/types.h"
#include "eglib/drawing.h"

/**
 * These functions initialize :c:type:`eglib_t` and control sleep status of
 * the hardware.
 */

struct _eglib_struct {
	struct {
		const hal_t *driver;
		void *config_ptr;
		// Set in between hal_begin and hal_end
		bool comm_active : 1;
		// Whether hal_t send() must send slave address for I2C
		uint8_t i2c_send_slave_addr : 2;
	} hal;

	struct {
		const display_t *driver;
		void *config_ptr;
		bool refreshing : 1;
	} display;

	drawing_t drawing;
};

/**
 * Initializer for :c:type:`eglib_t`. The given HAL driver must be of a bus that
 * the display driver supports.
 *
 * :param eglib: Pointer to :c:type:`eglib_t` to be initialized.
 * :param hal_driver: Pointer to the :doc:`HAL driver<hal/index>` (:c:type:`hal_t`) to use.
 * :param hal_driver_config_ptr: Pointer to the HAL driver configuration.
 *   Please refer each HAL driver documentation for details.
 * :param display_driver: Pointer to the :doc:`display driver<display/index>` (:c:type:`display_t`) to use.
 * :param display_config_ptr: Pointer to the display driver configuration.
 *   Please refer each display driver documentation for details.
 *
 * :See also: :c:func:`eglib_Init_FrameBuffer`.
 */
void eglib_Init(
	eglib_t *eglib,
	const hal_t *hal_driver,
	void *hal_driver_config_ptr,
	const display_t *display_driver,
	void *display_config_ptr
);

/** Puts both HAL and display to sleep. See :c:func:`eglib_SleepOut`. */
void eglib_SleepIn(eglib_t *eglib);

/** Takes both HAL and display out of sleep. See :c:func:`eglib_SleepIn` */
void eglib_SleepOut(eglib_t *eglib);

// Define scroll area, usually there is some space for a footer (bottom) or a header (top) on the screen to indicate what's inside
void eglib_setScrollMargins( eglib_t *eglib, coordinate_t top, coordinate_t bottom );

// scroll by number of lines (pixels)
void eglib_scrollScreen( eglib_t *eglib, coordinate_t num_lines );

#endif
