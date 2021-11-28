#ifndef EGLIB_H
#define EGLIB_H

// #include "eglib/config.h"
#include "eglib/types.h"
#include "eglib/drawing.h"

/**
 * These functions initialize :c:type:`eglib_t` and control sleep status of
 * the hardware.
 */

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

	struct {
		color_t color_index[4];
		struct _gradient_t gradient;
		const struct font_t *font;
		bool filled_mode;
        coordinate_t clip_xmin, clip_xmax, clip_ymin, clip_ymax;
	} drawing;
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

#endif
