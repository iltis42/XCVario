#include "ssd1675a.h"
#include "frame_buffer.h"

//
// Defines from Datasheet
//

#define SSD1675A_DRIVER_OUTPUT_CONTROL 0x01  // Configuration
#define SSD1675A_GATE_DRIVING_VOLTAGE_CONTROL 0x03  // Configuration
#define SSD1675A_SOURCE_DRIVING_VOLTAGE_CONTROL 0x04  // Configuration
#define SSD1675A_BOOSTER_SOFT_START_CONTROL 0x0c  // Configuration
#define SSD1675A_GATE_SCAN_START_POSITION 0x0f  // Configuration
#define SSD1675A_DEEP_SLEEP_MODE 0x10
#define SSD1675A_DEEP_SLEEP_MODE_1 0x01
#define SSD1675A_DEEP_SLEEP_MODE_2 0x03
#define SSD1675A_DATA_ENTRY_MODE_SETTING 0x11  // Memory
#define SSD1675A_SW_RESET 0x12
#define SSD1675A_HV_READY_DETECTION 0x14
#define SSD1675A_VCI_DETECTION 0x15
#define SSD1675A_TEMPERATURE_SENSOR_CONTROL 0x18  // Configuration
#define SSD1675A_TEMPERATURE_SENSOR_CONTROL_WRITE_TO_TEMPERATURE_REGISTER 0x1a
#define SSD1675A_TEMPERATURE_SENSOR_CONTROL_READ_TO_TEMPERATURE_REGISTER 0x1b
#define SSD1675A_TEMPERATURE_SENSOR_CONTROL_WRITE_COMMAND_TO_EXTERNAL_TEMPERATURE_SENSOR 0x1c
#define SSD1675A_MASTER_ACTIVATION 0x20
#define SSD1675A_DISPLAY_UPDATE_CONTROL_1 0x21  // Function
#define SSD1675A_DISPLAY_UPDATE_CONTROL_2 0x22  // Configuration
#define SSD1675A_WRITE_RAM_BW 0x24  // Memory
#define SSD1675A_WRITE_RAM_RED 0x26  // Memory
#define SSD1675A_READ_RAM 0x27
#define SSD1675A_VCOM_SENSE 0x28
#define SSD1675A_VCOM_SENSE_DURATION 0x29  // Configuration
#define SSD1675A_PROGRAM_VCOM_OTP 0x2a
#define SSD1675A_WRITE_VCOM_REGISTER 0x2c  // Configuration
#define SSD1675A_OTP_REGISTER_READ_FOR_DISPLAY_OPTION 0x2d
#define SSD1675A_USER_ID_READ 0x2e
#define SSD1675A_STATUS_BIT_READ 0x2f
#define SSD1675A_PROGRAM_WS_OTP 0x30
#define SSD1675A_LOAD_WS_OTP 0x31
#define SSD1675A_WRITE_LUT_REGISTER 0x32  // Configuration
#define SSD1675A_CRC_CALCULATION 0x34
#define SSD1675A_CRC_STATUS_READ 0x35
#define SSD1675A_PROGRAM_OTP_SELECTION 0x36
#define SSD1675A_WRITE_REGISTER_FOR_DISPLAY_OPTION_IN_OTP 0x37
#define SSD1675A_WRITE_REGISTER_FOR_USER_ID 0x38
#define SSD1675A_OTP_PROGRAM_MODE 0x39
#define SSD1675A_SET_DUMMY_LINE_PERIOD 0x3a  // Configuration
#define SSD1675A_SET_GATE_LINE_WIDTH 0x3b  // Configuration
#define SSD1675A_BORDER_WAVEFORM_CONTROL 0x3c  // Configuration
#define SSD1675A_READ_RAM_OPTION 0x41
#define SSD1675A_SET_RAM_X_ADDRESS_START_END_POSITION 0x44  // Memory
#define SSD1675A_SET_RAM_Y_ADDRESS_START_END_POSITION 0x45  // Memory
#define SSD1675A_AUTO_WRITE_RED_RAM_FOR_REGULAR_PATTERN 0x46  // Memory
#define SSD1675A_AUTO_WRITE_BW_RAM_FOR_REGULAR_PATTERN 0x47  // Memory
#define SSD1675A_SET_RAM_X_ADDRESS_COUNTER 0x4e  // Memory
#define SSD1675A_SET_RAM_Y_ADDRESS_COUNTER 0x4f  // Memory
#define SSD1675A_SET_ANALOG_BLOCK_CONTROL 0x74  // Configuration
#define SSD1675A_SET_DIGITAL_BLOCK_CONTROL 0x7e  // Configuration
#define SSD1675A_NOP 0x7f

//
// Functions
//

// Helpers

enum data_entry_mode {
	DATA_ENTRY_MODE_RIGHT_TO_LEFT_THEN_UP = 0x00,  // Rotate 180
	DATA_ENTRY_MODE_LEFT_TO_RIGHT_THEN_UP = 0x01,  // Mirror Vertically
	DATA_ENTRY_MODE_RIGHT_TO_LEFT_THEN_DOWN = 0x02,  // Mirror Horizontally
	DATA_ENTRY_MODE_LEFT_TO_RIGHT_THEN_DOWN = 0x03,  // Regular
	DATA_ENTRY_MODE_BOTTOM_TO_TOP_THEN_LEFT = 0x04,  // Rotate 90 & Mirror Vertically
	DATA_ENTRY_MODE_BOTTOM_TO_TOP_THEN_RIGHT = 0x05,  // Rotate 270
	DATA_ENTRY_MODE_TOP_TO_BOTTOM_THEN_LEFT = 0x06,  // Rotate 90
	DATA_ENTRY_MODE_TOP_TO_BOTTOM_THEN_RIGHT = 0x07,  // Rotate 270 & Mirror Vertically
};

static void set_data_entry_mode(eglib_t *eglib, enum data_entry_mode value);

static void set_data_entry_mode(eglib_t *eglib, enum data_entry_mode value) {
	eglib_SendCommandByte(eglib, SSD1675A_DATA_ENTRY_MODE_SETTING);
	eglib_SendDataByte(eglib, value);
}

static void set_ram_address_window(eglib_t *eglib, uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end) {
	eglib_SendCommandByte(eglib, SSD1675A_SET_RAM_X_ADDRESS_START_END_POSITION);
	eglib_SendDataByte(eglib, x_start >> 3);
	eglib_SendDataByte(eglib, x_end >> 3);
	eglib_SendCommandByte(eglib, SSD1675A_SET_RAM_Y_ADDRESS_START_END_POSITION);
	eglib_SendDataByte(eglib, y_start & 0xFF);
	eglib_SendDataByte(eglib, (y_start >> 8) & 0xFF);
	eglib_SendDataByte(eglib, y_end & 0xFF);
	eglib_SendDataByte(eglib, (y_end >> 8) & 0xFF);
}

static void set_ram_address_counter(eglib_t *eglib, uint8_t x, uint16_t y);

static void set_ram_address_counter(eglib_t *eglib, uint8_t x, uint16_t y) {
	eglib_SendCommandByte(eglib, SSD1675A_SET_RAM_X_ADDRESS_COUNTER);
	eglib_SendDataByte(eglib, (x >> 3) & 0xFF);
	eglib_SendCommandByte(eglib, SSD1675A_SET_RAM_Y_ADDRESS_COUNTER);
    eglib_SendDataByte(eglib, y & 0xFF);
    eglib_SendDataByte(eglib, (y >> 8) & 0xFF);
}

// display_t

static void init(eglib_t *eglib) {
	ssd1675a_config_t *config;

	config = eglib_GetDisplayConfig(eglib);
	(void)config;

	// HW Reset
	// Datasheet does not specify delays, using values from BuyDisplay examples
	eglib_SetReset(eglib, 1);
	eglib_SetReset(eglib, 0);
	eglib_DelayMs(eglib, 200);
	eglib_SetReset(eglib, 1);
	eglib_DelayMs(eglib, 200);

	eglib_CommBegin(eglib);
	eglib_SendCommandByte(eglib, SSD1675A_SW_RESET);
	// Busy line takes a while to go HIGH, so we end comm here to allow time
	// for it to raise
	eglib_CommEnd(eglib);
	while(eglib_GetBusy(eglib));

	eglib_CommBegin(eglib);

	eglib_SendCommandByte(eglib, SSD1675A_SET_ANALOG_BLOCK_CONTROL);
	eglib_SendDataByte(eglib, config->analog_block_control);

	eglib_SendCommandByte(eglib, SSD1675A_SET_DIGITAL_BLOCK_CONTROL);
	eglib_SendDataByte(eglib, config->digital_block_control);

	eglib_SendCommandByte(eglib, SSD1675A_DRIVER_OUTPUT_CONTROL);
	eglib_SendDataByte(eglib, (config->mux_gate_lines) & 0xFF);
	eglib_SendDataByte(eglib, (config->mux_gate_lines) >> 8);
	eglib_SendDataByte(
		eglib,
		((config->first_output_gate) << 2) |
		((config->scanning_order_of_gate_driver) << 1) |
		((config->tb) << 0)
	);

	eglib_SendCommandByte(eglib, SSD1675A_GATE_DRIVING_VOLTAGE_CONTROL);
	eglib_SendDataByte(eglib, config->gate_driving_voltage);

	eglib_SendCommandByte(eglib, SSD1675A_SET_DUMMY_LINE_PERIOD);
	eglib_SendDataByte(eglib, config->dummy_line_period);

	eglib_SendCommandByte(eglib, SSD1675A_SET_GATE_LINE_WIDTH);
	eglib_SendDataByte(eglib, config->gate_line_width);

	eglib_SendCommandByte(eglib, SSD1675A_SOURCE_DRIVING_VOLTAGE_CONTROL);
	eglib_SendDataByte(eglib, config->source_driving_voltage_vsh1);
	eglib_SendDataByte(eglib, config->source_driving_voltage_vsh2);
	eglib_SendDataByte(eglib, config->source_driving_voltage_vsl);

	eglib_SendCommandByte(eglib, SSD1675A_WRITE_VCOM_REGISTER);
	eglib_SendDataByte(eglib, config->vcom_register);

	eglib_SendCommandByte(eglib, SSD1675A_BORDER_WAVEFORM_CONTROL);
	eglib_SendDataByte(
		eglib,
		(config->border_waveform_vdb_option) << 6 |
		(config->border_waveform_vdb_fix_level) << 4 |
		(config->border_waveform_vdb_fix_level) << 0
	);

	// TODO implement support for these parameters

	// SSD1675A_BOOSTER_SOFT_START_CONTROL 0x0c
	/** Booster Soft Start Phase 1 Driving Strength (0 = Weakest, 7 = Strongest) */
	// uint8_t booster_soft_start_phase1_driving_strength : 3;
	/** Booster Soft Start Phase 1 Min Off Time Setting of GDR */
	// enum ssd1675a_booster_soft_start_phase_gdr_min_off_time booster_soft_start_phase1_min_off_time_setting_of_gdr : 4;
	/** Booster Soft Start Phase 1 Duration */
	// enum ssd1675a_booster_soft_start_phase_duration booster_soft_start_phase1_duration : 2;
	/** Booster Soft Start Phase 2 Driving Strength (0 = Weakest, 7 = Strongest) */
	// uint8_t booster_soft_start_phase2_driving_strength : 3;
	/** Booster Soft Start Phase 2 Min Off Time Setting of GDR */
	// enum ssd1675a_booster_soft_start_phase_gdr_min_off_time booster_soft_start_phase2_min_off_time_setting_of_gdr : 4;
	/** Booster Soft Start Phase 2 Duration */
	// enum ssd1675a_booster_soft_start_phase_duration booster_soft_start_phase2_duration : 2;
	/** Booster Soft Start Phase 3 Driving Strength (0 = Weakest, 7 = Strongest) */
	// uint8_t booster_soft_start_phase3_driving_strength : 3;
	/** Booster Soft Start Phase 3 Min Off Time Setting of GDR */
	// enum ssd1675a_booster_soft_start_phase_gdr_min_off_time booster_soft_start_phase3_min_off_time_setting_of_gdr : 4;
	/** Booster Soft Start Phase 3 Duration */
	// enum ssd1675a_booster_soft_start_phase_duration booster_soft_start_phase3_duration : 2;
	
	// SSD1675A_GATE_SCAN_START_POSITION 0x0f
	/** Gate scan start position from 0 to 295 (POR=0) */
	// uint16_t gate_scan_start_position : 9;

	// SSD1675A_TEMPERATURE_SENSOR_CONTROL 0x18
	/** Temperature Sensor Control */
	// enum ssd1675a_temperature_sensor temperature_sensor;

	eglib_CommEnd(eglib);

	ssd1675a_writeLut(eglib, config->lut_register);

	eglib_CommBegin(eglib);

	set_data_entry_mode(eglib, DATA_ENTRY_MODE_LEFT_TO_RIGHT_THEN_DOWN);
	// FIXME Account for SSD1675A_GATE_SCAN_START_POSITION
	set_ram_address_window(eglib, 0, config->width - 1, 0, config->height -1);
	eglib_SendCommandByte(eglib, SSD1675A_WRITE_RAM_BW);
	for(uint16_t i=0 ; i < (config->height * config->width / 8) ; i++)
		eglib_SendDataByte(eglib, 0xff);
	eglib_SendCommandByte(eglib, SSD1675A_WRITE_RAM_RED);
	for(uint16_t i=0 ; i < (config->height * config->width / 8) ; i++)
		eglib_SendDataByte(eglib, 0x00);

	eglib_CommEnd(eglib);
};

static void sleep_in(eglib_t *eglib) {
	eglib_CommBegin(eglib);
	eglib_SendCommandByte(eglib, SSD1675A_DEEP_SLEEP_MODE);
	eglib_SendDataByte(eglib, SSD1675A_DEEP_SLEEP_MODE_1);
	eglib_CommEnd(eglib);
};

static void sleep_out(eglib_t *eglib) {
	init(eglib);
};

static void get_dimension(
	eglib_t *eglib,
	coordinate_t *width, coordinate_t*height
) {
	ssd1675a_config_t *config;

	config = eglib_GetDisplayConfig(eglib);

	*width = config->width;;
	*height = config->height;
};

static void get_pixel_format(eglib_t *eglib, enum pixel_format_t *pixel_format) {
	(void)eglib;

	*pixel_format = PIXEL_FORMAT_2BIT_EPD;
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
	uint8_t rem;
	uint8_t *buffer;
	coordinate_t display_width, display_height;

	eglib_GetDimension(eglib, &display_width, &display_height);

	rem = (x % 8);
	if(rem) {
		x = (x / 8) * 8;
		width += rem;
	}

	buffer = (uint8_t *)buffer_ptr;

	eglib_CommBegin(eglib);

	set_ram_address_window(eglib, x, x + width, y, y + height);

	for(uint16_t j=y ; j < height ; j++) {
		set_ram_address_counter(eglib, x, j);
		eglib_SendCommandByte(eglib, SSD1675A_WRITE_RAM_BW);
		for(uint16_t i=x ; i < width / 8 ; i++) {
			eglib_SendDataByte(
				eglib,
				*(buffer + (j * display_width / 8) + i)
			);
		}
	}

	buffer += display_width / 8 * display_height;
	for(uint16_t j=y ; j < height ; j++) {
		set_ram_address_counter(eglib, x, j);
		eglib_SendCommandByte(eglib, SSD1675A_WRITE_RAM_RED);
		for(uint16_t i=x ; i < width / 8 ; i++) {
			eglib_SendDataByte(
				eglib,
				*(buffer + (j * display_width / 8) + i)
			);
		}
	}

	eglib_CommEnd(eglib);
}

static bool refresh(eglib_t *eglib) {
	 if(eglib_IsRefreshing(eglib)) {
		return eglib_GetBusy(eglib);
	 } else {
		ssd1675a_config_t *config;

		config = eglib_GetDisplayConfig(eglib);

		eglib_CommBegin(eglib);
		eglib_SendCommandByte(eglib, SSD1675A_DISPLAY_UPDATE_CONTROL_2);
		eglib_SendDataByte(eglib, config->display_update_control2);
		eglib_SendCommandByte(eglib, SSD1675A_MASTER_ACTIVATION);
		eglib_CommEnd(eglib);

		return true;
	 }
}

// Custom Functions

// SSD1675A_DISPLAY_UPDATE_CONTROL_1
//   Inverse RED
//   Inverse BW

ssd1675a_lut_t ssd1675a_lut_bw_adafruit = {
	// Voltage VS[nX-LUTn]
	// g0    g1    g2    g3    g4    g5    g6
	// ABCD  ABCD  ABCD  ABCD  ABCD  ABCD  ABCD
	0x80, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00,  // LUT0
	0x10, 0x60, 0x20, 0x00, 0x00, 0x00, 0x00,  // LUT1
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // LUT2
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // LUT3
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // LUT4
	// Phase Length TP[nX]
	// A     B     C     D     Repeat RP[n]
	0x03, 0x03, 0x00, 0x00, 0x02,  // g0
	0x09, 0x09, 0x00, 0x00, 0x02,  // g1
	0x03, 0x03, 0x00, 0x00, 0x02,  // g2
	0x00, 0x00, 0x00, 0x00, 0x00,  // g3
	0x00, 0x00, 0x00, 0x00, 0x00,  // g4
	0x00, 0x00, 0x00, 0x00, 0x00,  // g5
	0x00, 0x00, 0x00, 0x00, 0x00   // g6
};

ssd1675a_lut_t ssd1675a_lut_bw_partial = {
	// Voltage VS[nX-LUTn]
	// g0    g1    g2    g3    g4    g5    g6
	// ABCD  ABCD  ABCD  ABCD  ABCD  ABCD  ABCD
	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // LUT0: BB: VS 0 ~7
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // LUT1: BW: VS 0 ~7
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // LUT2: WB: VS 0 ~7
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // LUT3: WW: VS 0 ~7
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // LUT4: VCOM: VS 0 ~7
	// Phase Length TP[nX]
	// A     B     C     D     Repeat RP[n]
	0x0a, 0x00, 0x00, 0x00, 0x01,  // g0
	0x00, 0x00, 0x00, 0x00, 0x00,  // g1
	0x00, 0x00, 0x00, 0x00, 0x00,  // g2
	0x00, 0x00, 0x00, 0x00, 0x00,  // g3
	0x00, 0x00, 0x00, 0x00, 0x00,  // g4
	0x00, 0x00, 0x00, 0x00, 0x00,  // g5
	0x00, 0x00, 0x00, 0x00, 0x00   // g6
};

void ssd1675a_writeLut(eglib_t *eglib, ssd1675a_lut_t *lut) {
	eglib_CommBegin(eglib);
	eglib_SendCommandByte(eglib, SSD1675A_WRITE_LUT_REGISTER);
	eglib_SendData(eglib, (uint8_t *)lut, 70);
	eglib_CommEnd(eglib);
}

//
// display_t
//

const display_t ssd1675a = {
	.comm = {
		.four_wire_spi = &((hal_four_wire_spi_config_t){
			.mode = 0,
			.bit_numbering = HAL_MSB_FIRST,
			.cs_setup_ns = 20,
			.cs_hold_ns = 20,
			.cs_disable_ns = 100,
			.dc_setup_ns = 0,
			.dc_hold_ns = 0,
			.sck_cycle_ns = 50,  // 20MHz
		}),
		.i2c = NULL,
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
// ssd1675a_config_t
//

ssd1675a_config_t ssd1675a_config_buydisplay_red_2_9_inch_e_ink_display_module = {
	.width = 128,
	.height = 296,
	// SSD1675A_DRIVER_OUTPUT_CONTROL 0x01
	.mux_gate_lines = 295,
	.first_output_gate = SSD1675A_FIRST_OUTPUT_GATE_G0_G1,
	.scanning_order_of_gate_driver = SSD1675A_SCANNING_ORDER_OF_GATE_DRIVER_G0_G1,
	.tb = SSD1675A_TB_SCAN_FROM_G0_TO_G295,
	// SSD1675A_GATE_DRIVING_VOLTAGE_CONTROL 0x03
	.gate_driving_voltage = SSD1675A_GATE_DRIVING_VOLTAGE_19V,
	// SSD1675A_SOURCE_DRIVING_VOLTAGE_CONTROL 0x04
	.source_driving_voltage_vsh1 = SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_15V,  // Black
	.source_driving_voltage_vsh2 = SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_5V,  // Red
	.source_driving_voltage_vsl = SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_15V, // White
	// SSD1675A_BOOSTER_SOFT_START_CONTROL 0x0c
	// .booster_soft_start_phase1_driving_strength = 0,
	// .booster_soft_start_phase1_min_off_time_setting_of_gdr = SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_8_4,
	// .booster_soft_start_phase1_duration = SSD1675A_BOOSTER_SOFT_START_PHASE_DURATION_40MS,
	// .booster_soft_start_phase2_driving_strength = 1,
	// .booster_soft_start_phase2_min_off_time_setting_of_gdr = SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_9_8,
	// .booster_soft_start_phase2_duration = SSD1675A_BOOSTER_SOFT_START_PHASE_DURATION_40MS,
	// .booster_soft_start_phase3_driving_strength = 1,
	// .booster_soft_start_phase3_min_off_time_setting_of_gdr = SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_3_9,
	// .booster_soft_start_phase3_duration = SSD1675A_BOOSTER_SOFT_START_PHASE_DURATION_40MS,
	// SSD1675A_GATE_SCAN_START_POSITION 0x0f
	.gate_scan_start_position = 0,
	// SSD1675A_TEMPERATURE_SENSOR_CONTROL 0x18
	// .temperature_sensor = SSD1675A_TEMPERATURE_SENSOR_EXTERNAL,
	// SSD1675A_DISPLAY_UPDATE_CONTROL_2 0x22
	.display_update_control2 = 0xc7,
	// SSD1675A_VCOM_SENSE_DURATION 0x29
	// SSD1675A_WRITE_VCOM_REGISTER 0x2c
	.vcom_register = SSD1675A_VCOM_REGISTER_MINUS_2_6V,
	// SSD1675A_WRITE_LUT_REGISTER 0x32
	.lut_register = &(ssd1675a_lut_t){
		// Voltage VS[nX-LUTn]
		// g0    g1    g2    g3    g4    g5    g6
		// ABCD  ABCD  ABCD  ABCD  ABCD  ABCD  ABCD
		0xAA, 0x99, 0x10, 0x00, 0x00, 0x00, 0x00,  // LUT0
		0x55, 0x99, 0x80, 0x00, 0x00, 0x00, 0x00,  // LUT1
		0x8A, 0xA8, 0x9B, 0x00, 0x00, 0x00, 0x00,  // LUT2
		0x8A, 0xA8, 0x9B, 0x00, 0x00, 0x00, 0x00,  // LUT3
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // LUT4
		// Phase Length TP[nX]
		// A     B     C     D     Repeat RP[n]
		0x0F, 0x0F, 0x0F, 0x0F, 0x02,  // g0
		0x14, 0x14, 0x14, 0x14, 0x06,  // g1
		0x14, 0x14, 0x0C, 0x82, 0x08,  // g2
		0x00, 0x00, 0x00, 0x00, 0x00,  // g3
		0x00, 0x00, 0x00, 0x00, 0x00,  // g4
		0x00, 0x00, 0x00, 0x00, 0x00,  // g5
		0x00, 0x00, 0x00, 0x00, 0x00   // g6
	},
	// SSD1675A_SET_DUMMY_LINE_PERIOD 0x3a
	.dummy_line_period = 0x35,
	// SSD1675A_SET_GATE_LINE_WIDTH 0x3b
	.gate_line_width = 0x04,
	// SSD1675A_BORDER_WAVEFORM_CONTROL 0x3c
	.border_waveform_vdb_option = SSD1675A_BORDER_WAVEFORM_VDB_OPTION_GS_TRANSITION,
	.border_waveform_vdb_fix_level = SSD1675A_BORDER_WAVEFORM_VDB_FIX_LEVEL_VSH2,
	.border_waveform_vdb_gs_transition = SSD1675A_BORDER_WAVEFORM_VDB_GS_TRANSITION_LUT3,
	// SSD1675A_SET_ANALOG_BLOCK_CONTROL 0x74
	.analog_block_control = 0x54,
	// SSD1675A_SET_DIGITAL_BLOCK_CONTROL 0x7e
	.digital_block_control = 0x3b,
};

ssd1675a_config_t ssd1675a_config_buydisplay_red_2_9_inch_e_ink_display_module_fast_update = {
	.width = 128,
	.height = 296,
	// SSD1675A_DRIVER_OUTPUT_CONTROL 0x01
	.mux_gate_lines = 295,
	.first_output_gate = SSD1675A_FIRST_OUTPUT_GATE_G0_G1,
	.scanning_order_of_gate_driver = SSD1675A_SCANNING_ORDER_OF_GATE_DRIVER_G0_G1,
	.tb = SSD1675A_TB_SCAN_FROM_G0_TO_G295,
	// SSD1675A_GATE_DRIVING_VOLTAGE_CONTROL 0x03
	.gate_driving_voltage = SSD1675A_GATE_DRIVING_VOLTAGE_19V,
	// SSD1675A_SOURCE_DRIVING_VOLTAGE_CONTROL 0x04
	.source_driving_voltage_vsh1 = SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_17V,  // Black
	.source_driving_voltage_vsh2 = SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_7V,  // Red
	.source_driving_voltage_vsl = SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_13V, // White
	// SSD1675A_BOOSTER_SOFT_START_CONTROL 0x0c
	// .booster_soft_start_phase1_driving_strength = 0,
	// .booster_soft_start_phase1_min_off_time_setting_of_gdr = SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_8_4,
	// .booster_soft_start_phase1_duration = SSD1675A_BOOSTER_SOFT_START_PHASE_DURATION_40MS,
	// .booster_soft_start_phase2_driving_strength = 1,
	// .booster_soft_start_phase2_min_off_time_setting_of_gdr = SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_9_8,
	// .booster_soft_start_phase2_duration = SSD1675A_BOOSTER_SOFT_START_PHASE_DURATION_40MS,
	// .booster_soft_start_phase3_driving_strength = 1,
	// .booster_soft_start_phase3_min_off_time_setting_of_gdr = SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_3_9,
	// .booster_soft_start_phase3_duration = SSD1675A_BOOSTER_SOFT_START_PHASE_DURATION_40MS,
	// SSD1675A_GATE_SCAN_START_POSITION 0x0f
	.gate_scan_start_position = 0,
	// SSD1675A_TEMPERATURE_SENSOR_CONTROL 0x18
	// .temperature_sensor = SSD1675A_TEMPERATURE_SENSOR_EXTERNAL,
	// SSD1675A_DISPLAY_UPDATE_CONTROL_2 0x22
	.display_update_control2 = 0xc7,
	// SSD1675A_VCOM_SENSE_DURATION 0x29
	// SSD1675A_WRITE_VCOM_REGISTER 0x2c
	.vcom_register = SSD1675A_VCOM_REGISTER_MINUS_2_6V,
	// SSD1675A_WRITE_LUT_REGISTER 0x32
	.lut_register = &(ssd1675a_lut_t){
		// Voltage VS[nX-LUTn]
		// g0    g1    g2    g3    g4    g5    g6
		// ABCD  ABCD  ABCD  ABCD  ABCD  ABCD  ABCD
		0x66, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,  // LUT0
		0x66, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,  // LUT1
		0x66, 0x9B, 0x00, 0x00, 0x00, 0x00, 0x00,  // LUT2
		0x66, 0x9B, 0x00, 0x00, 0x00, 0x00, 0x00,  // LUT3
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // LUT4
		// Phase Length TP[nX]
		// A     B     C     D     Repeat RP[n]
		0x15, 0x15, 0x15, 0x15, 0x05,  // g0
		0x02, 0x08, 0x08, 0x35, 0x03,  // g1
		0x00, 0x00, 0x00, 0x00, 0x00,  // g2
		0x00, 0x00, 0x00, 0x00, 0x00,  // g3
		0x00, 0x00, 0x00, 0x00, 0x00,  // g4
		0x00, 0x00, 0x00, 0x00, 0x00,  // g5
		0x00, 0x00, 0x00, 0x00, 0x00   // g6
	},
	// SSD1675A_SET_DUMMY_LINE_PERIOD 0x3a
	.dummy_line_period = 0x35,
	// SSD1675A_SET_GATE_LINE_WIDTH 0x3b
	.gate_line_width = 0x04,
	// SSD1675A_BORDER_WAVEFORM_CONTROL 0x3c
	.border_waveform_vdb_option = SSD1675A_BORDER_WAVEFORM_VDB_OPTION_GS_TRANSITION,
	.border_waveform_vdb_fix_level = SSD1675A_BORDER_WAVEFORM_VDB_FIX_LEVEL_VSH2,
	.border_waveform_vdb_gs_transition = SSD1675A_BORDER_WAVEFORM_VDB_GS_TRANSITION_LUT3,
	// SSD1675A_SET_ANALOG_BLOCK_CONTROL 0x74
	.analog_block_control = 0x54,
	// SSD1675A_SET_DIGITAL_BLOCK_CONTROL 0x7e
	.digital_block_control = 0x3b,
};

// TODO BuyDisplay example code also setup these
// Reduce glitch under ACVCOM	
// eglib_SendCommandByte(eglib, 0x2B);
// eglib_SendDataByte(eglib, 0x04);
// eglib_SendDataByte(eglib, 0x63);

// eglib_SendCommandByte(eglib, SSD1675A_BOOSTER_SOFT_START_CONTROL);
// eglib_SendDataByte(eglib, 0x8B);
// eglib_SendDataByte(eglib, 0x9C);
// eglib_SendDataByte(eglib, 0x96);
// eglib_SendDataByte(eglib, 0x0F);

// eglib_SendCommandByte(eglib, SSD1675A_TEMPERATURE_SENSOR_CONTROL);
// eglib_SendDataByte(eglib, 0x80);  //Internal temperature sensor
ssd1675a_config_t ssd1675a_config_buydisplay_black_2_9_inch_e_ink_display_module = {
	.width = 128,
	.height = 296,
	// SSD1675A_DRIVER_OUTPUT_CONTROL 0x01
	.mux_gate_lines = 295,
	.first_output_gate = SSD1675A_FIRST_OUTPUT_GATE_G0_G1,
	.scanning_order_of_gate_driver = SSD1675A_SCANNING_ORDER_OF_GATE_DRIVER_G0_G1,
	.tb = SSD1675A_TB_SCAN_FROM_G0_TO_G295,
	// SSD1675A_GATE_DRIVING_VOLTAGE_CONTROL 0x03
	.gate_driving_voltage = SSD1675A_GATE_DRIVING_VOLTAGE_19V,
	// SSD1675A_SOURCE_DRIVING_VOLTAGE_CONTROL 0x04
	.source_driving_voltage_vsh1 = SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_15V,  // Black
	.source_driving_voltage_vsh2 = SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_5V,  // Red
	.source_driving_voltage_vsl = SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_15V, // White
	// SSD1675A_BOOSTER_SOFT_START_CONTROL 0x0c
	// .booster_soft_start_phase1_driving_strength = 0,
	// .booster_soft_start_phase1_min_off_time_setting_of_gdr = SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_8_4,
	// .booster_soft_start_phase1_duration = SSD1675A_BOOSTER_SOFT_START_PHASE_DURATION_40MS,
	// .booster_soft_start_phase2_driving_strength = 1,
	// .booster_soft_start_phase2_min_off_time_setting_of_gdr = SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_9_8,
	// .booster_soft_start_phase2_duration = SSD1675A_BOOSTER_SOFT_START_PHASE_DURATION_40MS,
	// .booster_soft_start_phase3_driving_strength = 1,
	// .booster_soft_start_phase3_min_off_time_setting_of_gdr = SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_3_9,
	// .booster_soft_start_phase3_duration = SSD1675A_BOOSTER_SOFT_START_PHASE_DURATION_40MS,
	// SSD1675A_GATE_SCAN_START_POSITION 0x0f
	.gate_scan_start_position = 0,
	// SSD1675A_TEMPERATURE_SENSOR_CONTROL 0x18
	// .temperature_sensor = SSD1675A_TEMPERATURE_SENSOR_EXTERNAL,
	// SSD1675A_DISPLAY_UPDATE_CONTROL_2 0x22
	.display_update_control2 = 0xc7,
	// SSD1675A_VCOM_SENSE_DURATION 0x29
	// SSD1675A_WRITE_VCOM_REGISTER 0x2c
	.vcom_register = SSD1675A_VCOM_REGISTER_MINUS_2_8V,
	// .vcom_register = SSD1675A_VCOM_REGISTER_MINUS_2_6V,
	// SSD1675A_WRITE_LUT_REGISTER 0x32
	.lut_register = &ssd1675a_lut_bw_adafruit,
	// SSD1675A_SET_DUMMY_LINE_PERIOD 0x3a
	.dummy_line_period = 0x30,
	// .dummy_line_period = 0x35,
	// SSD1675A_SET_GATE_LINE_WIDTH 0x3b
	.gate_line_width = 0x0a,
	// .gate_line_width = 0x04,
	// SSD1675A_BORDER_WAVEFORM_CONTROL 0x3c
	.border_waveform_vdb_option = SSD1675A_BORDER_WAVEFORM_VDB_OPTION_GS_TRANSITION,
	.border_waveform_vdb_fix_level = SSD1675A_BORDER_WAVEFORM_VDB_FIX_LEVEL_VSS,
	// .border_waveform_vdb_fix_level = SSD1675A_BORDER_WAVEFORM_VDB_FIX_LEVEL_VSH2,
	.border_waveform_vdb_gs_transition = SSD1675A_BORDER_WAVEFORM_VDB_GS_TRANSITION_LUT3,
	// SSD1675A_SET_ANALOG_BLOCK_CONTROL 0x74
	.analog_block_control = 0x54,
	// SSD1675A_SET_DIGITAL_BLOCK_CONTROL 0x7e
	.digital_block_control = 0x3b,
};