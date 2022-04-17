#ifndef EGLIB_DISPLAY_SH1106_H
#define EGLIB_DISPLAY_SH1106_H

#include "../hal.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * Types
 * =====
 */

/** Segment remap. */
typedef enum {
	/** Normal direction. */
	SH1106_SEGMENT_REMAP_NORMAL,
	/** reverse direction. */
	SH1106_SEGMENT_REMAP_REVERSE,
} sh1106_segment_remap_t;

/** Common pads hardware configuration. */
typedef enum {
	/** Sequential. */
	SH1106_COMMON_PADS_HARDWARE_CONFIGURATION_SEQUENTIAL,
	/** Alternative. */
	SH1106_COMMON_PADS_HARDWARE_CONFIGURATION_ALTERNATIVE,
} sh1106_common_pads_hardware_configuration_t;

/** Common output scan direction. */
typedef enum {
	/** Scan from COM0 to COM[N-1] (POR). */
	SH1106_COMMON_OUTPUT_SCAN_DIRECTION_ASC,
	/** Scan from COM[N-1] to COM0.*/
	SH1106_COMMON_OUTPUT_SCAN_DIRECTION_DESC,
} sh1106_common_output_scan_direction_t;

/** Oscillator frequency .*/
typedef enum {
	/** -25% */
	SH1106_OSCILLATOR_FREQUENCY_MINUS_25_PCT,
	/** -20% */
	SH1106_OSCILLATOR_FREQUENCY_MINUS_20_PCT,
	/** -15% */
	SH1106_OSCILLATOR_FREQUENCY_MINUS_15_PCT,
	/** -10% */
	SH1106_OSCILLATOR_FREQUENCY_MINUS_10_PCT,
	/** -5% */
	SH1106_OSCILLATOR_FREQUENCY_MINUS_5_PCT,
	/** fosc (POR) */
	SH1106_OSCILLATOR_FREQUENCY__0_PCT,
	/** 5% */
	SH1106_OSCILLATOR_FREQUENCY_PLUS_5_PCT,
	/** 10% */
	SH1106_OSCILLATOR_FREQUENCY_PLUS_10_PCT,
	/** 15% */
	SH1106_OSCILLATOR_FREQUENCY_PLUS_15_PCT,
	/** 20% */
	SH1106_OSCILLATOR_FREQUENCY_PLUS_20_PCT,
	/** 25% */
	SH1106_OSCILLATOR_FREQUENCY_PLUS_25_PCT,
	/** 30% */
	SH1106_OSCILLATOR_FREQUENCY_PLUS_30_PCT,
	/** 35% */
	SH1106_OSCILLATOR_FREQUENCY_PLUS_35_PCT,
	/** 40% */
	SH1106_OSCILLATOR_FREQUENCY_PLUS_40_PCT,
	/** 45% */
	SH1106_OSCILLATOR_FREQUENCY_PLUS_45_PCT,
	/** 50% */
	SH1106_OSCILLATOR_FREQUENCY_PLUS_50_PCT,
} sh1106_oscillator_frequency_t;

/** Pump voltage. */
typedef enum {
	/** 6.4V */
	SHH1106_DC_DC_6_4_V,
	/** 7.4V */
	SHH1106_DC_DC_7_4_V,
	/** 8.0V (POR) */
	SHH1106_DC_DC_8_0_V,
	/** 9.0V */
	SHH1106_DC_DC_9_0_V,
} sh1106_shh1106_dc_dc_t;

/**
 * Configuration
 * =============
 */

/**
 * Configuration for :c:data:`sh1106_vdd1_1_65_v` and :c:data:`sh1106_vdd1_2_4_v`.
 *
 * :See also: :c:func:`eglib_Init`.
 */
typedef struct {
	/** Display width */
	coordinate_t width;
	/** Display height */
	coordinate_t height;
	/** Segment remap (mirror horizontally). */
	sh1106_segment_remap_t segment_remap : 1;
	/**
	* Common pads hardware configuration.
	*
	* How rows (COM) and columns (SEG) are physically connected to the display.
	*/
	sh1106_common_pads_hardware_configuration_t common_pads_hardware_configuration : 1;
	/** Common output scan direction (mirror vertically). */
	sh1106_common_output_scan_direction_t common_output_scan_direction : 1;
	/**
	 * Vertical offset.
	 *
	 * Some displays do not start at row 0: this allows setting this offset.
	 */
	uint8_t display_offset : 6;
	/**
	 * Horizontal offset.
	 *
	 * Some displays do not start at column 0: this allows setting this offset.
	 */
	uint8_t column_offset;

	/** Pre-change period: 1-15 DCLKs (POR = 2 DCLKs) */
	uint8_t pre_charge_period : 4;
	/** Dis-change period: 1-15 DCLKs (POR = 2 DCLKs) */
	uint8_t dis_charge_period : 4;

	/** VCOM Deselect level. POR=0x35 (Beta=0.770). */
	uint8_t vcom_deselect_level;


	/** Display clock divide ratio. POR=0 (divide ratio = 1). */
	uint8_t clock_divide : 4;
	/** Display oscillator frequency .*/
	sh1106_oscillator_frequency_t oscillator_frequency : 4;

	/** DC-DC control mode on/off. */
	bool dc_dc_enable : 1;
	/** Pump voltage. */
	sh1106_shh1106_dc_dc_t dc_dc_voltage : 2;

	/** Least significant bit of 7-bit slave address: 011110(sa0). */
	bool sa0 : 1;
} sh1106_config_t;

/**
 * Pre-baked configurations
 * ------------------------
 */

/**
 * Configuration for `SparkFun Micro OLED Breakout <https://www.sparkfun.com/products/13003>`_.
 *
 * Values extracted from `example code <https://github.com/sparkfun/SparkFun_Micro_OLED_Arduino_Library/blob/master/src/SFE_MicroOLED.cpp>`_.
 */
extern sh1106_config_t sh1106_config_sparkfun_micro_oled;

/**
 * Driver
 * ======
 */

/**
 * SH1106 display driver.
 *
 * This driver supports the following data buses:
 *
 * - 4-Wire SPI.
 * - I2C.
 *
 * This display driver does not support writing pixels directly to the display
 * RAM and **must** be used in conjunction with :c:func:`eglib_Init_FrameBuffer`.
 *
 * This variant is to be used when VDD1 = 1.65 - 3.5V, TA= +25°C.
 *
 * :See also: :c:func:`eglib_Init`.
 * :See also: :c:data:`sh1106_vdd1_2_4_v`.
 * :See also: `Datasheet <https://www.crystalfontz.com/controllers/SinoWealth/SH1106/468/>`_.
 */
extern const display_t sh1106_vdd1_1_65_v;

/**
 * SH1106 display driver.
 *
 * This driver supports the following data buses:
 *
 * - 4-Wire SPI.
 * - I2C.
 *
 * This display driver does not support writing pixels directly to the display
 * RAM and **must** be used in conjunction with :c:func:`eglib_Init_FrameBuffer`.
 *
 * This variant is to be used when VDD1 = 2.4 - 3.5V, TA= +25°C
 *
 * :See also: :c:func:`eglib_Init`.
 * :See also: :c:data:`sh1106_vdd1_1_65_v`.
 * :See also: `Datasheet <https://www.crystalfontz.com/controllers/SinoWealth/SH1106/468/>`_.
 */
extern const display_t sh1106_vdd1_2_4_v;

/**
 * Functions
 * =========
 *
 * These functions can be used exclusively with :c:type:`eglib_t` initialized
 * with :c:data:`sh1106_vdd1_1_65_v` or `sh1106_vdd1_2_4_v`.
 */

/** Set display start line. */
void sh1106_SetStartLine(eglib_t *eglib, uint8_t line);

/** Set contrast. POR=128. */
void sh1106_SetContrast(eglib_t *eglib, uint8_t contrast);

/** Set entire display on/off */
void sh1106_EntireDisplayOn(eglib_t *eglib, bool entire_display_on);

/** Set reverse/normal display. */
void sh1106_Reverse(eglib_t *eglib, bool reverse);

#endif