#include "sh1106.h"

//
// Defines from Datasheet
//

#define SH1106_RESET_LOW_MS 10
#define SH1106_RESET_HIGH_MS 2

#define SH1106_SET_LOWER_COLUMN_ADDRESS(addr) (0x00|((addr)&0x0F))
#define SH1106_SET_HIGHER_COLUMN_ADDRESS(addr) (0x10|((addr)>>4))

#define SHH1106_SET_PUMP_VOLTAGE(voltage) (0x30|((voltage)&0x3))

#define SH1106_SET_DISPLAY_START_LINE(line) (0x40|((line)&0x3F))

#define SH1106_SET_CONTRAST_CONTROL_REGISTER 0x81

#define SH1106_SET_SEGMENT_REMAP(dir) (0xA0|((dir)&0x1))

#define SH1106_SET_ENTIRE_DISPLAY_OFF 0xA4
#define SH1106_SET_ENTIRE_DISPLAY_ON 0xA5

#define SH1106_SET_NORMAL_DISPLAY 0xA6
#define SH1106_SET_REVERSE_DISPLAY 0xA7

#define SH1106_SET_MULTIPLEX_RATIO 0xA8
#define SH1106_SET_MULTIPLEX_RATIO_ARG(ratio) ((ratio)&0x3F)

#define SH1106_DC_DC_CONTROL_MODE_SET 0xAD
#define SH1106_DC_DC_OFF 0x8A
#define SH1106_DC_DC_ON 0x8B

#define SH1106_DISPLAY_OFF 0xAE
#define SH1106_DISPLAY_ON 0xAF
#define SH1106_DISPLAY_ON_MS 100

#define SH1106_SET_PAGE_ADDRESS(addr) (0xB0|((addr)&0x0F))

#define SH1106_SET_COMMON_OUTPUT_SCAN_DIRECTION(dir) (0xC0|(((dir)&0x1)<<3))

#define SH1106_SET_DISPLAY_OFFSET 0xD3

#define SH1106_SET_DISPLAY_CLOCK_DIVIDE_RATIO_OSCILLATOR_FREQUENCY 0xD5
#define SH1106_SET_DISPLAY_CLOCK_DIVIDE_RATIO_OSCILLATOR_FREQUENCY_ARG( \
	oscillator_frequency, \
	divite_ratio \
) ( \
	(0xF0&((oscillator_frequency)<<4)) | \
	(0xF&(divite_ratio)) \
)

#define SH1106_DISCHARGE_PRECHARGE_PERIOD_MODE_SET 0xD9
#define SH1106_DISCHARGE_PRECHARGE_PERIOD_MODE_SET_ARG( \
	discharge_period, \
	precharge_period \
) ( \
	(0xF0&((discharge_period)<<4)) | \
	(0xF&(precharge_period)) \
)

#define SH1106_COMMON_PADS_HARDWARE_CONFIGURATION_MODE_SET 0xDA
#define SH1106_COMMON_PADS_HARDWARE_CONFIGURATION_MODE_SET_ARG(value) (0x02|(((value)&0x1)<<4))

#define SH1106_VCOM_DESELECT_LEVEL_MODE_SET 0xDB

#define SH1106_READ_MODIFY_WRITE 0xE0

#define SH1106_END 0xEE

#define SH1106_NOP 0xE3

#define SH1106_I2C_CO 7
#define SH1106_I2C_DC 6

//
// Functions
//

// Helpers

static inline void set_column_address(
	eglib_t *eglib,
	coordinate_t column
) {
	sh1106_config_t *config;
	uint8_t buff[2];

	config = eglib_GetDisplayConfig(eglib);

	buff[0] = SH1106_SET_HIGHER_COLUMN_ADDRESS(column + config->column_offset);
	buff[1] = SH1106_SET_LOWER_COLUMN_ADDRESS(column + config->column_offset);
	eglib_SendCommands(eglib, buff, sizeof(buff));
}

static inline void display_on(eglib_t *eglib) {
	uint8_t buff[] = {
		SH1106_DISPLAY_ON,
	};

	eglib_SendCommands(eglib, buff, sizeof(buff));
	eglib_DelayMs(eglib, SH1106_DISPLAY_ON_MS);
}

// display_t

static uint8_t get_7bit_slave_addr(eglib_t *eglib, enum hal_dc_t dc) {
	sh1106_config_t *config;

	(void)dc;
	config = eglib_GetDisplayConfig(eglib);

	return 0x3C | (config->sa0);
}

static void init(eglib_t *eglib) {
	sh1106_config_t *config;

	config = eglib_GetDisplayConfig(eglib);

	// Hardware reset

	eglib_SetReset(eglib, 0);
	eglib_DelayMs(eglib, SH1106_RESET_LOW_MS);
	eglib_SetReset(eglib, 1);
	eglib_DelayMs(eglib, SH1106_RESET_HIGH_MS);

	// comm begin

	eglib_CommBegin(eglib);


	uint8_t commands_init[] = {
		// Display physical construction

		SH1106_SET_SEGMENT_REMAP(config->segment_remap),

		SH1106_SET_MULTIPLEX_RATIO,
		SH1106_SET_MULTIPLEX_RATIO_ARG(config->height-1),

		SH1106_COMMON_PADS_HARDWARE_CONFIGURATION_MODE_SET,
		SH1106_COMMON_PADS_HARDWARE_CONFIGURATION_MODE_SET_ARG(
			config->common_pads_hardware_configuration
		),

		SH1106_SET_COMMON_OUTPUT_SCAN_DIRECTION(
			config->common_output_scan_direction
		),

		SH1106_SET_DISPLAY_OFFSET,
		config->display_offset,

		// Change period

		SH1106_DISCHARGE_PRECHARGE_PERIOD_MODE_SET,
		SH1106_DISCHARGE_PRECHARGE_PERIOD_MODE_SET_ARG(
			config->dis_charge_period,
			config->pre_charge_period
		),

		// VCOM deselect

		SH1106_VCOM_DESELECT_LEVEL_MODE_SET,
		config->vcom_deselect_level,

		// Internal display clocks

		SH1106_SET_DISPLAY_CLOCK_DIVIDE_RATIO_OSCILLATOR_FREQUENCY,
		SH1106_SET_DISPLAY_CLOCK_DIVIDE_RATIO_OSCILLATOR_FREQUENCY_ARG(
			config->oscillator_frequency,
			config->clock_divide
		),
	};

	eglib_SendCommands(eglib, commands_init, sizeof(commands_init));


	// Charge Pump Regulator
	if(config->dc_dc_enable) {
		eglib_SendCommandByte(eglib, SH1106_DC_DC_CONTROL_MODE_SET);
		eglib_SendCommandByte(eglib, SH1106_DC_DC_ON);
		eglib_SendCommandByte(eglib, SHH1106_SET_PUMP_VOLTAGE(config->dc_dc_voltage));
	} else {
		eglib_SendCommandByte(eglib, SH1106_DC_DC_CONTROL_MODE_SET);
		eglib_SendCommandByte(eglib, SH1106_DC_DC_OFF);
	}

	// Clear RAM
	for(uint8_t page=0 ; page < (config->height / 8) ; page++) {
		eglib_SendCommandByte(eglib, SH1106_SET_PAGE_ADDRESS(page));
		set_column_address(eglib, 0);
		for(coordinate_t column=0 ; column < config->width ; column ++)
			eglib_SendDataByte(eglib, 0x00);
	}

	// Set display on
	display_on(eglib);

	eglib_CommEnd(eglib);
};

static void sleep_in(eglib_t *eglib) {
	// TODO
	(void)eglib;
};

static void sleep_out(eglib_t *eglib) {
	// TODO
	(void)eglib;
};

static void get_dimension(
	eglib_t *eglib,
	coordinate_t *width, coordinate_t*height
) {
	sh1106_config_t *config;

	config = eglib_GetDisplayConfig(eglib);

	*width = config->width;;
	*height = config->height;
};

static void get_pixel_format(eglib_t *eglib, enum pixel_format_t *pixel_format) {
	(void)eglib;

	*pixel_format = PIXEL_FORMAT_1BIT_BW_PAGED;
}

static void draw_pixel_color(
	eglib_t *eglib,
	coordinate_t x, coordinate_t y, color_t color
) {
	(void)eglib;
	(void)x;
	(void)y;
	(void)color;
};

static void send_buffer(
	eglib_t *eglib,
	void *buffer_ptr,
	coordinate_t x, coordinate_t y,
	coordinate_t width, coordinate_t height
) {
	uint8_t *buffer;
	coordinate_t display_width, display_height;

	buffer = (uint8_t *)buffer_ptr;

	eglib_GetDimension(eglib, &display_width, &display_height);

	eglib_CommBegin(eglib);
	for(uint8_t page=y/8 ; page < ((y+height)/8.0) ; page++) {
		eglib_SendCommandByte(eglib, SH1106_SET_PAGE_ADDRESS(page));
		set_column_address(eglib, x);
		eglib_SendData(eglib, (buffer + page * display_width + x), width);
	}
	eglib_CommEnd(eglib);
}

static bool refresh(eglib_t *eglib) {
	(void)eglib;
	return false;
}

// Custom Functions

void sh1106_SetStartLine(
	eglib_t *eglib,
	uint8_t line
) {
	eglib_CommBegin(eglib);
	eglib_SendCommandByte(eglib, SH1106_SET_DISPLAY_START_LINE(line));
	eglib_CommEnd(eglib);
}

void sh1106_SetContrast(
	eglib_t *eglib,
	uint8_t contrast
) {
	eglib_CommBegin(eglib);
	eglib_SendCommandByte(eglib, SH1106_SET_CONTRAST_CONTROL_REGISTER);
	eglib_SendCommandByte(eglib, contrast);
	eglib_CommEnd(eglib);
}

void sh1106_EntireDisplayOn(
	eglib_t *eglib,
	bool entire_display_on
) {
	eglib_CommBegin(eglib);
	if(entire_display_on)
		eglib_SendCommandByte(eglib, SH1106_SET_ENTIRE_DISPLAY_ON);
	else
		eglib_SendCommandByte(eglib, SH1106_SET_ENTIRE_DISPLAY_OFF);
	eglib_CommEnd(eglib);
}

void sh1106_Reverse(
	eglib_t *eglib,
	bool reverse
) {
	eglib_CommBegin(eglib);
	if(reverse)
		eglib_SendCommandByte(eglib, SH1106_SET_REVERSE_DISPLAY);
	else
		eglib_SendCommandByte(eglib, SH1106_SET_NORMAL_DISPLAY);
	eglib_CommEnd(eglib);
}

//
// display_t
//

static void i2c_send(
	eglib_t *eglib,
	void (*i2c_write)(eglib_t *eglib, uint8_t byte),
	enum hal_dc_t dc,
	uint8_t *bytes,
	uint32_t length
) {
	// For more than 2 bytes it is more efficient to use Co=0
	if(length > 2) {
		// Control byte
		i2c_write(eglib, (0<<SH1106_I2C_CO) | (dc<<SH1106_I2C_DC));
		for (uint16_t i = 0; i < length; i++){
			// Data byte
			i2c_write(eglib, bytes[i]);
		}
		// ReStart
		eglib_CommBegin(eglib);
	} else {
		for (uint16_t i = 0; i < length; i++){
			// Control byte
			i2c_write(eglib, (1<<SH1106_I2C_CO) | (dc<<SH1106_I2C_DC));
			// Data byte
			i2c_write(eglib, bytes[i]);
		}
	}
}

static hal_i2c_config_t hal_i2c_config = {
	.speed = HAL_I2C_400KHZ,
	.get_7bit_slave_addr = get_7bit_slave_addr,
	.send = i2c_send,
};

const display_t sh1106_vdd1_1_65_v = {
	.comm = {
		.four_wire_spi = &((hal_four_wire_spi_config_t){
			.mode = 0,
			.bit_numbering = HAL_MSB_FIRST,
			.cs_setup_ns = 240,
			.cs_hold_ns = 120,
			.cs_disable_ns = 0,
			.dc_setup_ns = 300,
			.dc_hold_ns = 300,
			.sck_cycle_ns = 500,
		}),
		.i2c = &hal_i2c_config,
	},
	.init = init,
	.sleep_in = sleep_in,
	.sleep_out = sleep_out,
	.get_dimension = get_dimension,
	.get_pixel_format = get_pixel_format,
	.draw_pixel_color = draw_pixel_color,
	.draw_line = display_default_draw_line,
	.send_buffer = send_buffer,
	.refresh = refresh,
};

const display_t sh1106_vdd1_2_4_v = {
	.comm = {
		.four_wire_spi = &((hal_four_wire_spi_config_t){
			.mode = 0,
			.bit_numbering = HAL_MSB_FIRST,
			.cs_setup_ns = 120,
			.cs_hold_ns = 60,
			.cs_disable_ns = 0,
			.dc_setup_ns = 150,
			.dc_hold_ns = 150,
			.sck_cycle_ns = 250,
		}),
		.i2c = &hal_i2c_config,
	},
	.init = init,
	.sleep_in = sleep_in,
	.sleep_out = sleep_out,
	.get_dimension = get_dimension,
	.get_pixel_format = get_pixel_format,
	.draw_pixel_color = draw_pixel_color,
	.draw_line = display_default_draw_line,
	.send_buffer = send_buffer,
	.refresh = refresh,
};

//
// sh1106_config_t
//

sh1106_config_t sh1106_config_sparkfun_micro_oled = {
	// Display physical construction
	.width = 64,
	.height = 48,
	.segment_remap = SH1106_SEGMENT_REMAP_REVERSE,
	.common_pads_hardware_configuration = SH1106_COMMON_PADS_HARDWARE_CONFIGURATION_ALTERNATIVE,
	.common_output_scan_direction = SH1106_COMMON_OUTPUT_SCAN_DIRECTION_ASC,
	.display_offset = 0,
	.column_offset = 0,

	// Change period
	.pre_charge_period = 1,
	.dis_charge_period = 15,

	// VCOM deselect
	.vcom_deselect_level = 0x40,

	// Internal display clocks
	.clock_divide = 0,
	.oscillator_frequency = SH1106_OSCILLATOR_FREQUENCY_PLUS_15_PCT,

	// Charge Pump Regulator
	.dc_dc_enable = true,
	.dc_dc_voltage = SHH1106_DC_DC_8_0_V,
};