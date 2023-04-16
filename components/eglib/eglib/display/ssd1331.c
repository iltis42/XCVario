#include "ssd1331.h"
#include "../display.h"
#include <stdlib.h>

//
// Defines
//

#define SSD1331_RESET_LOW_PULSE_WIDTH_US 2
#define SSD1331_FOSC_MIN_HZ 800000UL
#define SSD1331_FOSC_MAX_PERIOD_NS (1000000000UL/SSD1331_FOSC_MIN_HZ)

// Configuration commands

#define SSD1331_SET_COLUMN_ADDRESS 0x15
#define SSD1331_SET_ROW_ADDRESS 0x75

#define SSD1331_SET_CONTRAST_FOR_COLOR_A 0x81
#define SSD1331_SET_CONTRAST_FOR_COLOR_B 0x82
#define SSD1331_SET_CONTRAST_FOR_COLOR_C 0x83

#define SSD1331_MASTER_CURRENT_CONTROL 0x87

#define SSD1331_SET_SECOND_PRE_CHARGE_SPEED_FOR_COLOR_A 0x8a
#define SSD1331_SET_SECOND_PRE_CHARGE_SPEED_FOR_COLOR_B 0x8b
#define SSD1331_SET_SECOND_PRE_CHARGE_SPEED_FOR_COLOR_C 0x8c

#define SSD1331_REMAP_AND_COLOR_DEPTH_SETTING 0xa0
#define SSD1331_REMAP_HORIZONTAL_ADDRESS_INCREMENT (0<<0)
#define SSD1331_REMAP_VERTICAL_ADDRESS_INCREMENT (1<<0)
#define SSD1331_REMAP_RAM_COLUMN_0_TO_95_MAPS_TO_PIN_SEG_0_TO_95 (0<<1)
#define SSD1331_REMAP_RAM_COLUMN_0_TO_95_MAPS_TO_PIN_SEG_95_TO_0 (1<<1)
#define SSD1331_COLOR_DEPTH_RGB (0<<2)
#define SSD1331_COLOR_DEPTH_BGR (1<<2)
#define SSD1331_REMAP_DISABLE_LEFT_RIGHT_SWAPPING_ON_COM (0<<3)
#define SSD1331_REMAP_SET_LEFT_RIGHT_SWAPPING_ON_COM (1<<3)
#define SSD1331_REMAP_SCAN_FROM_COM_0_TO_COM_N_1 (0<<4)
#define SSD1331_REMAP_SCAN_FROM_COM_N_1_TO_COM_0 (1<<4)
#define SSD1331_REMAP_DISABLE_COM_SPLIT_ODD_EVEN (0<<5)
#define SSD1331_REMAP_ENABLE_COM_SPLIT_ODD_EVEN (1<<5)
#define SSD1331_COLOR_DEPTH_256_COLOR_FORMAT (0<<6)
#define SSD1331_COLOR_DEPTH_65k_COLOR_FORMAT (1<<6)
#define SSD1331_COLOR_DEPTH_65k_COLOR_FORMAT_2 (2<<6)

#define SSD1331_SET_DISPLAY_START_LINE 0xa1

#define SSD1331_SET_DISPLAY_OFFSET 0xa2

#define SSD1331_SET_DISPLAY_MODE_NORMAL 0xa4
#define SSD1331_SET_DISPLAY_MODE_ENTIRE_DISPLAY_ON 0xa5
#define SSD1331_SET_DISPLAY_MODE_ENTIRE_DISPLAY_OFF 0xa6
#define SSD1331_SET_DISPLAY_MODE_INVERSE_DISPLAY 0xa7

#define SSD1331_SET_MULTIPLEX_RATIO 0xa8

#define SSD1331_DIM_MODE_SETTING 0xab

#define SSD1331_SET_MASTER_CONFIGURATION 0xad
#define SSD1331_SET_MASTER_CONFIGURATION_EXTERNAL_VCC 0x8e

#define SSD1331_SET_DISPLAY_ON_IN_DIM_MODE 0xac
#define SSD1331_SET_DISPLAY_OFF 0xae
#define SSD1331_SET_DISPLAY_ON_IN_NORMAL_MODE 0xaf
#define SSD1331_SET_DISPLAY_ON_DELAY_MS 100

#define SSD1331_POWER_SAVE_MODE 0xb0
#define SSD1331_POWER_SAVE_MODE_ENABLED 0x1a
#define SSD1331_POWER_SAVE_MODE_DISABLED 0x0b

#define SSD1331_PHASE_1_AND_2_PERIOD_ADJUSTMENT 0xb1
#define SSD1331_PHASE_1_AND_2_PERIOD_ADJUSTMENT_ARG(phase1, phase2) ((phase1&0x0f)|((phase2<<4)|0xf0))

#define SSD1331_DISPLAY_CLOCK_DIVIDER_AND_OSCILLATOR_FREQUENCY 0xb3
#define SSD1331_DISPLAY_CLOCK_DIVIDER_AND_OSCILLATOR_FREQUENCY_ARG(clock_divider, oscillator_frequency) ((clock_divider&0x0f)|((oscillator_frequency<<4)|0xf0))

#define SSD1331_SET_GRAY_SCALE_TABLE 0xb8

#define SSD1331_ENABLE_LINEAR_GRAY_SCALE_TABLE 0xb9

#define SSD1331_SET_PRE_CHARGE_LEVEL 0xbb
#define SSD1331_PRE_CHARGE_LEVEL_ARG(level) (level<<1)

#define SSD1331_NOP_BC 0xbc
#define SSD1331_NOP_BD 0xbd

#define SSD1331_SET_VCOMH 0xbe
#define SSD1331_VCOMH_ARG(level) (level<<1)

#define SSD1331_NOP_E3 0xe3

#define SSD1331_SET_COMMAND_LOCK 0xfd
#define SSD1331_SET_COMMAND_LOCK_DISABLE 0x12
#define SSD1331_SET_COMMAND_LOCK_ENABLE 0x16

// Graphic Acceleration Commands

#define SSD1331_DRAW_LINE 0x21
#define SSD1331_DRAWING_RECTANGLE 0x22
#define SSD1331_COPY 0x23
#define SSD1331_DIM_WINDOW 0x24
#define SSD1331_CLEAR_WINDOW 0x25

#define SSD1331_FILL 0x26
#define SSD1331_DISABLE_FILL_FOR_DRAW_RECTANGLE_COMMAND (0<<0)
#define SSD1331_ENABLE_FILL_FOR_DRAW_RECTANGLE_COMMAND (1<<0)
#define SSD1331_FILL_DISABLE_REVERSE_COPY (0<<4)
#define SSD1331_FILL_ENABLE_REVERSE_DURING_COPY_COMMAND (1<<4)

#define SSD1331_CONTINUOUS_HORIZONTAL_AND_VERTICAL_SCROLLING_SETUP 0x27
#define SSD1331_DEACTIVATE_SCROLLING 0x2e
#define SSD1331_ACTIVATE_SCROLLING 0x2f

//
// Functions
//

// Helpers

static uint8_t get_remap_aand_color_depth_setting(ssd1331_config_t *config) {
	uint8_t remap_and_color_depth_setting;

	remap_and_color_depth_setting = 0;

	remap_and_color_depth_setting |= SSD1331_REMAP_HORIZONTAL_ADDRESS_INCREMENT;

	switch(config->column_address_mapping) {
		case SSD1331_RAM_COLUMN_0_TO_95_MAPS_TO_PIN_SEG_0_TO_95:
			remap_and_color_depth_setting |= SSD1331_REMAP_RAM_COLUMN_0_TO_95_MAPS_TO_PIN_SEG_0_TO_95;
			break;
		case SSD1331_RAM_COLUMN_0_TO_95_MAPS_TO_PIN_SEG_95_TO_0:
			remap_and_color_depth_setting |= SSD1331_REMAP_RAM_COLUMN_0_TO_95_MAPS_TO_PIN_SEG_95_TO_0;
			break;
	}

	remap_and_color_depth_setting |= SSD1331_COLOR_DEPTH_RGB;

	if(config->left_right_swapping_on_com)
		remap_and_color_depth_setting |= SSD1331_REMAP_SET_LEFT_RIGHT_SWAPPING_ON_COM;
	else
		remap_and_color_depth_setting |= SSD1331_REMAP_DISABLE_LEFT_RIGHT_SWAPPING_ON_COM;

	switch(config->com_scan) {
		case SSD1331_SCAN_FROM_COM0_TO_COM_N_1:
			remap_and_color_depth_setting |= SSD1331_REMAP_SCAN_FROM_COM_0_TO_COM_N_1;
			break;
		case SSD1331_SCAN_FROM_COM_N_1_TO_COM0:
			remap_and_color_depth_setting |= SSD1331_REMAP_SCAN_FROM_COM_N_1_TO_COM_0;
			break;
	}

	if(config->com_split_odd_even)
		remap_and_color_depth_setting |= SSD1331_REMAP_ENABLE_COM_SPLIT_ODD_EVEN;
	else
		remap_and_color_depth_setting |= SSD1331_REMAP_DISABLE_COM_SPLIT_ODD_EVEN;

	switch(config->color_format) {
		case SSD1331_256_COLORS:
			remap_and_color_depth_setting |= SSD1331_COLOR_DEPTH_256_COLOR_FORMAT;
			break;
		case SSD1331_65k_COLORS:
			remap_and_color_depth_setting |= SSD1331_COLOR_DEPTH_65k_COLOR_FORMAT;
			break;
	}

	return remap_and_color_depth_setting;
}

static void set_addresses(
	eglib_t *eglib,
	coordinate_t x_start,
	coordinate_t x_end,
	coordinate_t y_start,
	coordinate_t y_end
) {
	uint8_t commands[] = {
		SSD1331_SET_COLUMN_ADDRESS,
		x_start, x_end,
		SSD1331_SET_ROW_ADDRESS,
		y_start, y_end
	};
	eglib_SendCommands(eglib, commands, sizeof(commands));
}

// display_t

static void init(eglib_t *eglib) {
	ssd1331_config_t *config;

	config = eglib_GetDisplayConfig(eglib);

	eglib_SetReset(eglib, 1);
	eglib_SetReset(eglib, 0);
	eglib_DelayUs(eglib, SSD1331_RESET_LOW_PULSE_WIDTH_US);
	eglib_SetReset(eglib, 1);

	eglib_CommBegin(eglib);

	uint8_t commands_init[] = {
		SSD1331_SET_CONTRAST_FOR_COLOR_A, config->color_a_contrast,
		SSD1331_SET_CONTRAST_FOR_COLOR_B, config->color_b_contrast,
		SSD1331_SET_CONTRAST_FOR_COLOR_C, config->color_c_contrast,
		SSD1331_MASTER_CURRENT_CONTROL, 15,
		SSD1331_SET_SECOND_PRE_CHARGE_SPEED_FOR_COLOR_A, config->second_pre_charge_speed_for_color_a,
		SSD1331_SET_SECOND_PRE_CHARGE_SPEED_FOR_COLOR_B, config->second_pre_charge_speed_for_color_b,
		SSD1331_SET_SECOND_PRE_CHARGE_SPEED_FOR_COLOR_C, config->second_pre_charge_speed_for_color_c,
		SSD1331_REMAP_AND_COLOR_DEPTH_SETTING, get_remap_aand_color_depth_setting(config),
		SSD1331_SET_MULTIPLEX_RATIO, SSD1331_HEIGHT - 1,
		SSD1331_SET_MASTER_CONFIGURATION, SSD1331_SET_MASTER_CONFIGURATION_EXTERNAL_VCC,
		SSD1331_SET_DISPLAY_OFFSET, config->display_offset,
		SSD1331_PHASE_1_AND_2_PERIOD_ADJUSTMENT, SSD1331_PHASE_1_AND_2_PERIOD_ADJUSTMENT_ARG(
			config->phase1_period,
			config->phase2_period
		),
		SSD1331_DISPLAY_CLOCK_DIVIDER_AND_OSCILLATOR_FREQUENCY, SSD1331_DISPLAY_CLOCK_DIVIDER_AND_OSCILLATOR_FREQUENCY_ARG(
			config->clock_divider,
			config->oscillator_frequency
		),
	};
	eglib_SendCommands(eglib, commands_init, sizeof(commands_init));

	if(config->grayscale_table) {
		eglib_SendCommandByte(eglib, SSD1331_SET_GRAY_SCALE_TABLE);
		eglib_SendCommands(eglib, config->grayscale_table, 32);
	} else {
		eglib_SendCommandByte(eglib, SSD1331_ENABLE_LINEAR_GRAY_SCALE_TABLE);
	}

	eglib_SendCommandByte(eglib, SSD1331_SET_PRE_CHARGE_LEVEL);
	eglib_SendCommandByte(eglib, SSD1331_PRE_CHARGE_LEVEL_ARG(config->pre_charge_level));

	eglib_SendCommandByte(eglib, SSD1331_SET_VCOMH);
	eglib_SendCommandByte(eglib, SSD1331_VCOMH_ARG(config->v_comh));

	set_addresses(eglib, 0, SSD1331_WIDTH - 1, 0, SSD1331_HEIGHT - 1);
	for(uint32_t addr=0 ; addr < SSD1331_HEIGHT * SSD1331_WIDTH * 2 ; addr++)
		eglib_SendDataByte(eglib, 0x00);

	eglib_SendCommandByte(eglib, SSD1331_SET_DISPLAY_ON_IN_NORMAL_MODE);
	eglib_DelayMs(eglib, SSD1331_SET_DISPLAY_ON_DELAY_MS);

	eglib_CommEnd(eglib);
};

static void sleep_in(eglib_t *eglib) {
	eglib_CommBegin(eglib);
	eglib_SendCommandByte(eglib, SSD1331_SET_DISPLAY_OFF);
	uint8_t enable_power_save[] = {
		SSD1331_POWER_SAVE_MODE,
		SSD1331_POWER_SAVE_MODE_ENABLED,
	};
	eglib_SendCommands(eglib, enable_power_save, sizeof(enable_power_save));
	eglib_CommEnd(eglib);
};

static void sleep_out(eglib_t *eglib) {
	uint8_t disable_power_save[] = {
		SSD1331_POWER_SAVE_MODE,
		SSD1331_POWER_SAVE_MODE_DISABLED,
	};
	eglib_CommBegin(eglib);
	eglib_SendCommands(eglib, disable_power_save, sizeof(disable_power_save));
	eglib_SendCommandByte(eglib, SSD1331_SET_DISPLAY_ON_IN_NORMAL_MODE);
	eglib_DelayMs(eglib, SSD1331_SET_DISPLAY_ON_DELAY_MS);
	eglib_CommEnd(eglib);
};

static void get_dimension(
	eglib_t *eglib,
	coordinate_t *width, coordinate_t*height
) {
	(void)eglib;
	*width = 96;
	*height = 64;
};

static void get_pixel_format(eglib_t *eglib, enum pixel_format_t *pixel_format) {
	ssd1331_config_t *config;

	config = eglib_GetDisplayConfig(eglib);

	switch(config->color_format) {
		case SSD1331_256_COLORS:
			*pixel_format = PIXEL_FORMAT_8BIT_RGB;
			break;
		case SSD1331_65k_COLORS:
			*pixel_format = PIXEL_FORMAT_16BIT_RGB;
			break;
	}
}

static void draw_pixel_color(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y, color_t color
) {
	ssd1331_config_t *config;
	uint8_t buffer[2];

	config = eglib_GetDisplayConfig(eglib);

	eglib_CommBegin(eglib);

	set_addresses(eglib, x, x, y, y);

	switch(config->color_format) {
		case SSD1331_256_COLORS:
			buffer[0] = 0x00;
			buffer[0] |= color.r & 0xE0;
			buffer[0] |= (color.g & 0xE0)>>3;
			buffer[0] |= (color.b & 0xC0)>>6;
			eglib_SendDataByte(eglib, buffer[0]);
			break;
		case SSD1331_65k_COLORS:
			buffer[0] = color.r & 0xf8;
			buffer[0] |= color.g >> 5;
			buffer[1] = (color.g >> 2) << 5;
			buffer[1] |= color.b >> 3;
			eglib_SendData(eglib, buffer, 2);
			break;
	}

	eglib_CommEnd(eglib);
};

static void draw_line(
	eglib_t *eglib,
	coordinate_t x,
	coordinate_t y,
	enum display_line_direction_t direction,
	coordinate_t length,
	color_t (*get_next_color)(eglib_t *eglib)
) {
	ssd1331_config_t *config;
	uint8_t buffer[2];

	config = eglib_GetDisplayConfig(eglib);

	switch(direction) {
		case DISPLAY_LINE_DIRECTION_RIGHT:
			eglib_CommBegin(eglib);
			set_addresses(eglib, x, x + length, y, y);
			break;
		case DISPLAY_LINE_DIRECTION_DOWN:
		case DISPLAY_LINE_DIRECTION_LEFT:
		case DISPLAY_LINE_DIRECTION_UP:
			display_default_draw_line(
				eglib,
				x, y,
				direction,
				length,
				get_next_color
			);
			return;
	}
	while(length--) {
		color_t color;

		color = get_next_color(eglib);
		switch(config->color_format) {
			case SSD1331_256_COLORS:
				buffer[0] = 0x00;
				buffer[0] |= color.r & 0xE0;
				buffer[0] |= (color.g & 0xE0)>>3;
				buffer[0] |= (color.b & 0xC0)>>6;
				eglib_SendData(eglib, buffer, 1);
				break;
			case SSD1331_65k_COLORS:
				buffer[0] = color.r & 0xf8;
				buffer[0] |= color.g >> 5;
				buffer[1] = (color.g >> 2) << 5;
				buffer[1] |= color.b >> 3;
				eglib_SendData(eglib, buffer, 2);
				break;
		}
	}
	eglib_CommEnd(eglib);
}

static void send_buffer(
	eglib_t *eglib,
	void *buffer_ptr,
	coordinate_t x, coordinate_t y,
	coordinate_t width, coordinate_t height
) {
	ssd1331_config_t *config;
	uint8_t *buffer = (uint8_t *)buffer_ptr;
	uint8_t bytes_per_pixel;

	config = eglib_GetDisplayConfig(eglib);

	eglib_CommBegin(eglib);
	set_addresses(
		eglib,
		x, x + width - 1,
		y, y + height - 1
	);

	switch(config->color_format) {
		case SSD1331_256_COLORS:
			bytes_per_pixel = 1;
			break;
		case SSD1331_65k_COLORS:
			bytes_per_pixel = 2;
			break;
		default:
			while(true);
	}
	for(coordinate_t v=y ; v < y + height ; v++) {
		eglib_SendData(
			eglib,
			buffer + (
				SSD1331_WIDTH * v + x
			) * bytes_per_pixel,
			(uint32_t)width * bytes_per_pixel
		);
	}
	eglib_CommEnd(eglib);
}

static bool refresh(eglib_t *eglib) {
	(void)eglib;
	return false;
}

// Custom functions

void ssd1331_SetMasterCurrent(eglib_t *eglib, uint8_t value) {
	eglib_CommBegin(eglib);
	uint8_t set_master_current[] = {
		SSD1331_MASTER_CURRENT_CONTROL, value & 0x0f,
	};
	eglib_SendCommands(eglib, set_master_current, sizeof(set_master_current));
	eglib_CommEnd(eglib);
};

void ssd1331_SetDisplayStartLine(eglib_t *eglib, uint8_t line) {
	eglib_CommBegin(eglib);
	uint8_t set_start_line[] = {
		SSD1331_SET_DISPLAY_START_LINE, line & 0x3F,
	};
	eglib_SendCommands(eglib, set_start_line, sizeof(set_start_line));
	eglib_CommEnd(eglib);
}

void ssd1331_SetDisplayMode(eglib_t *eglib, enum ssd1331_display_mode mode) {
	eglib_CommBegin(eglib);
	switch(mode) {
		case SSD1331_DISPLAY_MODE_NORMAL:
			eglib_SendCommandByte(eglib, SSD1331_SET_DISPLAY_MODE_NORMAL);
			break;
		case SSD1331_DISPLAY_MODE_ENTIRE_DISPLAY_ON:
			eglib_SendCommandByte(eglib, SSD1331_SET_DISPLAY_MODE_ENTIRE_DISPLAY_ON);
			break;
		case SSD1331_DISPLAY_MODE_ENTIRE_DISPLAY_OFF:
			eglib_SendCommandByte(eglib, SSD1331_SET_DISPLAY_MODE_ENTIRE_DISPLAY_OFF);
			break;
		case SSD1331_DISPLAY_MODE_INVERSE:
			eglib_SendCommandByte(eglib, SSD1331_SET_DISPLAY_MODE_INVERSE_DISPLAY);
			break;
	}
	eglib_CommEnd(eglib);
}

void ssd1331_SetDimMode(
	eglib_t *eglib,
	uint8_t color_a_contrast,
	uint8_t color_b_contrast,
	uint8_t color_c_contrast,
	uint8_t pre_charge_voltage
) {
	uint8_t commands[] = {
		SSD1331_DIM_MODE_SETTING,
		0x00,
		color_a_contrast,
		color_b_contrast,
		color_c_contrast,
		pre_charge_voltage & 0x1F,
		SSD1331_SET_DISPLAY_ON_IN_DIM_MODE
	};

	eglib_CommBegin(eglib);
	eglib_SendCommands(eglib, commands, sizeof(commands));
	eglib_DelayMs(eglib, SSD1331_SET_DISPLAY_ON_DELAY_MS);
	eglib_CommEnd(eglib);
}

void ssd1331_DrawLine(
	eglib_t *eglib,
	uint8_t x1, uint8_t y1,
	uint8_t x2, uint8_t y2
) {
	coordinate_t xd, yd;
	uint8_t commands[] = {
		SSD1331_DRAW_LINE,
		x1, y1,
		x2, y2,
		eglib->drawing.color_index[0].r,
		eglib->drawing.color_index[0].g,
		eglib->drawing.color_index[0].b
	};

	eglib_CommBegin(eglib);
	eglib_SendCommands(eglib, commands, sizeof(commands));
	// Extracted from https://github.com/WolfWings/SSD1331_t3/blob/d7d5a537226ba6d0ade89c97f2bc5048a5a8b5e0/SSD1331_t3.cpp
	xd = abs(x1 - x2);
	yd = abs(y1 - y2);
	eglib_DelayNs(eglib, SSD1331_FOSC_MAX_PERIOD_NS * ((xd > yd) ? xd : yd) / 16);
	eglib_CommEnd(eglib);
}

void ssd1331_DrawFrame(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y,
	coordinate_t width, coordinate_t height
) {
	uint8_t commands[] = {
		SSD1331_FILL,
		SSD1331_DISABLE_FILL_FOR_DRAW_RECTANGLE_COMMAND,
		SSD1331_DRAWING_RECTANGLE,
		x, y,
		x + width, y + height,
		eglib->drawing.color_index[0].r,
		eglib->drawing.color_index[0].g,
		eglib->drawing.color_index[0].b,
		0, 0, 0 // Fill color not used.
	};

	eglib_CommBegin(eglib);
	eglib_SendCommands(eglib, commands, sizeof(commands));
	// Extracted from https://github.com/WolfWings/SSD1331_t3/blob/d7d5a537226ba6d0ade89c97f2bc5048a5a8b5e0/SSD1331_t3.cpp
	eglib_DelayNs(eglib, SSD1331_FOSC_MAX_PERIOD_NS * (width + height + 2) / 8);
	eglib_CommEnd(eglib);
}

void ssd1331_DrawBox(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y,
	coordinate_t width, coordinate_t height
) {
	uint8_t commands[] = {
		SSD1331_FILL,
		SSD1331_ENABLE_FILL_FOR_DRAW_RECTANGLE_COMMAND,
		SSD1331_DRAWING_RECTANGLE,
		x, y,
		x + width, y + height,
		eglib->drawing.color_index[0].r,
		eglib->drawing.color_index[0].g,
		eglib->drawing.color_index[0].b,
		eglib->drawing.color_index[0].r,
		eglib->drawing.color_index[0].g,
		eglib->drawing.color_index[0].b,
	};

	eglib_CommBegin(eglib);
	eglib_SendCommands(eglib, commands, sizeof(commands));
	// Extracted from https://github.com/WolfWings/SSD1331_t3/blob/d7d5a537226ba6d0ade89c97f2bc5048a5a8b5e0/SSD1331_t3.cpp
	eglib_DelayNs(eglib, SSD1331_FOSC_MAX_PERIOD_NS * ((width + 1) * (height + 1)) / 16);
	eglib_CommEnd(eglib);
}

// SSD1331_COPY

// SSD1331_DIM_WINDOW

// SSD1331_CLEAR_WINDOW

// SSD1331_FILL
// SSD1331_DISABLE_FILL_FOR_DRAW_RECTANGLE_COMMAND
// SSD1331_ENABLE_FILL_FOR_DRAW_RECTANGLE_COMMAND
// SSD1331_FILL_DISABLE_REVERSE_COPY
// SSD1331_FILL_ENABLE_REVERSE_DURING_COPY_COMMAND

// SSD1331_CONTINUOUS_HORIZONTAL_AND_VERTICAL_SCROLLING_SETUP

// SSD1331_DEACTIVATE_SCROLLING

// SSD1331_ACTIVATE_SCROLLING

//
// display_t
//

const display_t ssd1331 = {
	.comm = {
		.four_wire_spi = &((hal_four_wire_spi_config_t){
			.mode = 0,
			.bit_numbering = HAL_MSB_FIRST,
			.cs_setup_ns = 75,
			.cs_hold_ns = 60,
			.cs_disable_ns = 0,
			.dc_setup_ns = 40,
			.dc_hold_ns = 40,
			.sck_cycle_ns = 150, // 6.66 MHz
		}),
		.i2c = NULL,
	},
	.init = init,
	.sleep_in = sleep_in,
	.sleep_out = sleep_out,
	.get_dimension = get_dimension,
	.get_pixel_format = get_pixel_format,
	.draw_pixel_color = draw_pixel_color,
	.draw_line = draw_line,
	.send_buffer = send_buffer,
	.refresh = refresh,
};

const display_t ssd1331_overclock = {
	.comm = {
		.four_wire_spi = &((hal_four_wire_spi_config_t){
			.mode = 0,
			.bit_numbering = HAL_MSB_FIRST,
			.cs_setup_ns = 75,
			.cs_hold_ns = 60,
			.cs_disable_ns = 0,
			.dc_setup_ns = 40,
			.dc_hold_ns = 40,
			.sck_cycle_ns = 47, // 21.27MHz overclock
		}),
		.i2c = NULL,
	},
	.init = init,
	.sleep_in = sleep_in,
	.sleep_out = sleep_out,
	.get_dimension = get_dimension,
	.get_pixel_format = get_pixel_format,
	.draw_pixel_color = draw_pixel_color,
	.draw_line = draw_line,
	.send_buffer = send_buffer,
	.refresh = refresh,
};

//
// ssd1331_config_t
//

ssd1331_config_t ssd1331_config_adafruit_65k_colors = {
	.color_a_contrast = 0x91,
	.color_b_contrast = 0x50,
	.color_c_contrast = 0x7d,
	.second_pre_charge_speed_for_color_a = 0x64,
	.second_pre_charge_speed_for_color_b = 0x78,
	.second_pre_charge_speed_for_color_c = 0x64,
	.column_address_mapping = SSD1331_RAM_COLUMN_0_TO_95_MAPS_TO_PIN_SEG_95_TO_0,
	.left_right_swapping_on_com = false,
	.com_scan = SSD1331_SCAN_FROM_COM_N_1_TO_COM0,
	.com_split_odd_even = true,
	.color_format = SSD1331_65k_COLORS,
	.display_offset = 0,
	.phase1_period = 1,
	.phase2_period = 3,
	.clock_divider = 0,
	.oscillator_frequency = 13,
	.grayscale_table = NULL,
	.pre_charge_level = 29,
	.v_comh = 31,
};

ssd1331_config_t ssd1331_config_adafruit_256_colors = {
	.color_a_contrast = 0x91,
	.color_b_contrast = 0x50,
	.color_c_contrast = 0x7d,
	.second_pre_charge_speed_for_color_a = 0x64,
	.second_pre_charge_speed_for_color_b = 0x78,
	.second_pre_charge_speed_for_color_c = 0x64,
	.column_address_mapping = SSD1331_RAM_COLUMN_0_TO_95_MAPS_TO_PIN_SEG_95_TO_0,
	.left_right_swapping_on_com = false,
	.com_scan = SSD1331_SCAN_FROM_COM_N_1_TO_COM0,
	.com_split_odd_even = true,
	.color_format = SSD1331_256_COLORS,
	.display_offset = 0,
	.phase1_period = 1,
	.phase2_period = 3,
	.clock_divider = 0,
	.oscillator_frequency = 13,
	.grayscale_table = NULL,
	.pre_charge_level = 29,
	.v_comh = 31,
};

ssd1331_config_t ssd1331_config_buydisplay_65k_colors = {
	.color_a_contrast = 255,
	.color_b_contrast = 255,
	.color_c_contrast = 255,
	.second_pre_charge_speed_for_color_a = 0x64,
	.second_pre_charge_speed_for_color_b = 0x78,
	.second_pre_charge_speed_for_color_c = 0x64,
	.column_address_mapping = SSD1331_RAM_COLUMN_0_TO_95_MAPS_TO_PIN_SEG_95_TO_0,
	.left_right_swapping_on_com = false,
	.com_scan = SSD1331_SCAN_FROM_COM_N_1_TO_COM0,
	.com_split_odd_even = true,
	.color_format = SSD1331_65k_COLORS,
	.display_offset = 0,
	.phase1_period = 1,
	.phase2_period = 3,
	.clock_divider = 0,
	.oscillator_frequency = 15,
	.grayscale_table = NULL,
	.pre_charge_level = 29,
	.v_comh = 31,
};

ssd1331_config_t ssd1331_config_buydisplay_256_colors = {
	.color_a_contrast = 255,
	.color_b_contrast = 255,
	.color_c_contrast = 255,
	.second_pre_charge_speed_for_color_a = 0x64,
	.second_pre_charge_speed_for_color_b = 0x78,
	.second_pre_charge_speed_for_color_c = 0x64,
	.column_address_mapping = SSD1331_RAM_COLUMN_0_TO_95_MAPS_TO_PIN_SEG_95_TO_0,
	.left_right_swapping_on_com = false,
	.com_scan = SSD1331_SCAN_FROM_COM_N_1_TO_COM0,
	.com_split_odd_even = true,
	.color_format = SSD1331_256_COLORS,
	.display_offset = 0,
	.phase1_period = 1,
	.phase2_period = 3,
	.clock_divider = 0,
	.oscillator_frequency = 15,
	.grayscale_table = NULL,
	.pre_charge_level = 29,
	.v_comh = 31,
};