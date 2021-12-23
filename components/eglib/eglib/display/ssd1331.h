#ifndef EGLIB_DISPLAY_SSD1331_H
#define EGLIB_DISPLAY_SSD1331_H

#include "../../eglib.h"

/** Display width */
#define SSD1331_WIDTH 96
/** Display height */
#define SSD1331_HEIGHT 64

/**
 * Types
 * =====
 */

/** Column address mapping. */
enum ssd1331_column_address_mapping {
	/** RAM Column 0 to 95 maps to Pin Seg (SA,SB,SC) 0 to 95. */
	SSD1331_RAM_COLUMN_0_TO_95_MAPS_TO_PIN_SEG_0_TO_95,
	/** RAM Column 0 to 95 maps to Pin Seg (SA,SB,SC) 95 to 0. */
	SSD1331_RAM_COLUMN_0_TO_95_MAPS_TO_PIN_SEG_95_TO_0,
};

/** COM scan (where N is the multiplex ratio). */
enum ssd1331_com_scan {
	/** Scan from COM0 to COM[N-1]. */
	SSD1331_SCAN_FROM_COM0_TO_COM_N_1,
	/** Scan from COM[N-1] to COM0. */
	SSD1331_SCAN_FROM_COM_N_1_TO_COM0,
};

/** Color format. */
enum ssd1331_color_format {
	/** 256 color. */
	SSD1331_256_COLORS,
	/** 65k color. */
	SSD1331_65k_COLORS,
};

/** Display mode. */
enum ssd1331_display_mode {
	/** Normal display */
	SSD1331_DISPLAY_MODE_NORMAL,
	/** Entire display ON, all pixels turn ON at GS63. */
	SSD1331_DISPLAY_MODE_ENTIRE_DISPLAY_ON,
	/** Entire display OFF, all pixels turn OFF. */
	SSD1331_DISPLAY_MODE_ENTIRE_DISPLAY_OFF,
	/**Inverse display. */
	SSD1331_DISPLAY_MODE_INVERSE,
};

/**
 * Configuration
 * =============
 */

/**
 * Configuration for :c:data:`ssd1331`.
 *
 * :See also: :c:func:`eglib_Init`.
 */
typedef struct {
	/** Contrast for color "A" segment (Pins: SA0-SA95). RESET=128. */
	uint8_t color_a_contrast;
	/** Contrast for color "B" segment (Pins: SA0-SA95). RESET=128. */
	uint8_t color_b_contrast;
	/** Contrast for color "C" segment (Pins: SA0-SA95). RESET=128. */
	uint8_t color_c_contrast;
	/** Second pre-charge speed for color A. Default value equals to ``color_a_contrast``. */
	uint8_t second_pre_charge_speed_for_color_a;
	/** Second pre-charge speed for color B. Default value equals to ``color_b_contrast``. */
	uint8_t second_pre_charge_speed_for_color_b;
	/** Second pre-charge speed for color C. Default value equals to ``color_c_contrast``. */
	uint8_t second_pre_charge_speed_for_color_c;
	/** Column address mapping (mirror horizontally). */
	enum ssd1331_column_address_mapping column_address_mapping;
	/** Left-right swapping on COM. */
	bool left_right_swapping_on_com;
	/** COM scan (mirror vertically). */
	enum ssd1331_com_scan com_scan;
	/** COM split odd even. */
	bool com_split_odd_even;
	/** Color format. If 9/18bit mode is selected, color depth will be fixed to 65k. */
	enum ssd1331_color_format color_format;
	/** Mapping of display start line to one of COM0-63. */
	uint8_t display_offset : 6;
	/** Phase 1 period in N DCLK (1-15). RESET=7. */
	uint8_t phase1_period : 4;
	/** Phase 2 period in N DCLK (1-15). RESET=4. */
	uint8_t phase2_period : 4;
	/** Divide ratio (D) of the display clocks (DCLK (1-16)). RESET=0. */
	uint8_t clock_divider : 4;
	/** Fosc frequency. RESET=13. */
	uint8_t oscillator_frequency : 4;
	/**
	 * When set to NULL, use linear gray scale table.
	 *
	 * When set to not NULL, uses this gray scale table from GS1 to GS63.
	 */
	uint8_t *grayscale_table;
	/** Set pre-charge voltage level from 0.10*Vcc (0) to 0x50*Vcc (31). RESET=31. */
	uint8_t pre_charge_level : 5;
	/** COM deselect voltage level (V COMH) from 0.44*Vcc (0) to 0.83*Vcc (31). RESET=3. */
	uint8_t v_comh : 5;
} ssd1331_config_t;

/**
 * Pre-baked configurations
 * ------------------------
 */

/**
 * Configuration for `Adafruit OLED Breakout Board - 16-bit Color 0.96" w/microSD holder <http://www.adafruit.com/products/684>`_.
 *
 * Values extracted from `Adafruit's example code <https://github.com/adafruit/Adafruit-SSD1331-OLED-Driver-Library-for-Arduino>`_.
 *
 * :Note: this configuration should work just fine with "generic" SSD1331 boards that can be easily found on eBay or Aliexpress.
 */
extern ssd1331_config_t ssd1331_config_adafruit_65k_colors;

/**
 * Variant of :c:data:`ssd1331_config_adafruit_65k_colors` with only 256 colors instead of 65k.
 */
extern ssd1331_config_t ssd1331_config_adafruit_256_colors;

/**
 * Configuration for `Buy Display module <https://www.buydisplay.com/full-color-0-95-inch-arduino-raspberry-pi-oled-display-module-96x64>`_.
 *
 * Values extracted from `Buy Display's example code <https://www.buydisplay.com/arduino/ER-OLEDM0.95-2_Arduino_Tutorial.zip>`_.
 */
extern ssd1331_config_t ssd1331_config_buydisplay_65k_colors;

/**
 * Variant of :c:data:`ssd1331_config_buydisplay_65k_colors` with only 256 colors instead of 65k.
 */
extern ssd1331_config_t ssd1331_config_buydisplay_256_colors;

/**
 * Driver
 * ======
 */

/**
 * SSD1331 display driver.
 *
  * This driver supports the following data buses:
 *
 * - 4-Wire SPI.
 *
 * :See also: :c:func:`eglib_Init`.
 * :See also: `Datasheet <https://cdn-shop.adafruit.com/datasheets/SSD1331_1.2.pdf>`_.
 */
extern const display_t ssd1331;

/**
 * Variant of :c:data:`ssd1331` which sets the SPI bus to 21.27Mhz. The datasheet
 * states that the maximum bus speed is 6.66MHz, so mileage may vary when using
 * this.
 */
extern const display_t ssd1331_overclock;

/**
 * Functions
 * =========
 *
 * These functions can be used exclusively with :c:type:`eglib_t` initialized
 * with :c:data:`ssd1331` or `ssd1331_overclock`.
 */

/**
 * Set master current attenuation factor from 0 (1/16) to 15 (16/16).
 *
 * This effectively controls the how dim / bright the display is. 
 */
void ssd1331_SetMasterCurrent(eglib_t *eglib, uint8_t value);

/**
 * Set the display start line from 0 to 63.
 *
 * This controls vertical scrolling.
 */
void ssd1331_SetDisplayStartLine(eglib_t *eglib, uint8_t line);

/** Set display mode */
void ssd1331_SetDisplayMode(eglib_t *eglib, enum ssd1331_display_mode mode);

/**
 * Set Dim mode.
 *
 * :param color_a_contrast: Contrast setting for color A.
 * :param color_b_contrast: Contrast setting for color B.
 * :param color_c_contrast: Contrast setting for color C.
 * :param pre_charge_voltge: Precharge voltage setting (0-31). Setting this to~15 should keep contrast tame with for low contrast values.
 */
void ssd1331_SetDimMode(
	eglib_t *eglib,
	uint8_t color_a_contrast,
	uint8_t color_b_contrast,
	uint8_t color_c_contrast,
	uint8_t pre_charge_voltage
);

/**
 * Similar to :c:func:`eglib_DrawLine`, draw a line using color from index 0. It
 * draws directly to display memory using a graphic accelerated command.
 * Coordinates must be inside the display area.
 */
void ssd1331_DrawLine(
	eglib_t *eglib,
	uint8_t x1, uint8_t y1,
	uint8_t x2, uint8_t y2
);

/**
 * Similar to :c:func:`eglib_DrawFrame`, draw a frame using color from index 0.
 * It draws directly to display memory using a graphic accelerated command.
 * Coordinates must be inside the display area.
 */
void ssd1331_DrawFrame(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y,
	coordinate_t width, coordinate_t height
);

/**
 * Similar to :c:func:`eglib_DrawBox`, draw a box using color from index 0.
 * It draws directly to display memory using a graphic accelerated command.
 * Coordinates must be inside the display area.
 */
void ssd1331_DrawBox(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y,
	coordinate_t width, coordinate_t height
);

#endif