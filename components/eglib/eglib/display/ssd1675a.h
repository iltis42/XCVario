#ifndef EGLIB_DISPLAY_SSD1675A_H
#define EGLIB_DISPLAY_SSD1675A_H

#include "../../eglib.h"
#include <stdbool.h>

/**
 * Types
 * =====
 */

/** First output Gate. */
enum ssd1675a_first_output_gate {
	/** G0, G1, G2, G3... (POR) */
	SSD1675A_FIRST_OUTPUT_GATE_G0_G1,
	/** G1, G0, G3, G2... */
	SSD1675A_FIRST_OUTPUT_GATE_G1_G0,
};

/** Scanning order of gate driver. */
enum ssd1675a_scanning_order_of_gate_driver {
	/** G0, G1, G2, G3...295 (POR) */
	SSD1675A_SCANNING_ORDER_OF_GATE_DRIVER_G0_G1,
	/** G0, G2, G4...G294, G1, G3,...G295 */
	SSD1675A_SCANNING_ORDER_OF_GATE_DRIVER_G0_G2,
};

/** TB. */
enum ssd1675a_tb {
	/** Scan from G0 to G295 (POR) */
	SSD1675A_TB_SCAN_FROM_G0_TO_G295,
	/** Scan from G295 to G0 */
	SSD1675A_TB_SCAN_FROM_G295_TO_G0,
};

/** Gate driving voltage */
enum ssd1675a_gate_driving_voltage {
	/** 10V */
	SSD1675A_GATE_DRIVING_VOLTAGE_10V = 0x03,
	/** 10.5V */
	SSD1675A_GATE_DRIVING_VOLTAGE_10_5V = 0x04,
	/** 11V */
	SSD1675A_GATE_DRIVING_VOLTAGE_11V = 0x05,
	/** 11.5V */
	SSD1675A_GATE_DRIVING_VOLTAGE_11_5V = 0x06,
	/** 12V */
	SSD1675A_GATE_DRIVING_VOLTAGE_12V = 0x07,
	/** 12.5V */
	SSD1675A_GATE_DRIVING_VOLTAGE_12_5V = 0x08,
	/** 13V */
	SSD1675A_GATE_DRIVING_VOLTAGE_13V = 0x09,
	/** 13.5V */
	SSD1675A_GATE_DRIVING_VOLTAGE_13_5V = 0x0a,
	/** 14V */
	SSD1675A_GATE_DRIVING_VOLTAGE_14V = 0x0b,
	/** 14.5V */
	SSD1675A_GATE_DRIVING_VOLTAGE_14_5V = 0x0c,
	/** 15V */
	SSD1675A_GATE_DRIVING_VOLTAGE_15V = 0x0d,
	/** 15.5V */
	SSD1675A_GATE_DRIVING_VOLTAGE_15_5V = 0x0e,
	/** 16V */
	SSD1675A_GATE_DRIVING_VOLTAGE_16V = 0x0f,
	/** 16.5V */
	SSD1675A_GATE_DRIVING_VOLTAGE_16_5V = 0x10,
	/** 17V */
	SSD1675A_GATE_DRIVING_VOLTAGE_17V = 0x11,
	/** 17.5V */
	SSD1675A_GATE_DRIVING_VOLTAGE_17_5V = 0x12,
	/** 18V */
	SSD1675A_GATE_DRIVING_VOLTAGE_18V = 0x13,
	/** 18.5V */
	SSD1675A_GATE_DRIVING_VOLTAGE_18_5V = 0x14,
	/** 19V (POR) */
	SSD1675A_GATE_DRIVING_VOLTAGE_19V = 0x15,
	/** 19.5V */
	SSD1675A_GATE_DRIVING_VOLTAGE_19_5V = 0x16,
	/** 20V */
	SSD1675A_GATE_DRIVING_VOLTAGE_20V = 0x17,
};

/** Source driving voltage VSH1 / VSH2 */
enum ssd1675a_source_driving_voltage_vsh {
	/** 2.4V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_2_4V = 0x8e,
	/** 2.5V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_2_5V = 0x8f,
	/** 2.6V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_2_6V = 0x90,
	/** 2.7V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_2_7V = 0x91,
	/** 2.8V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_2_8V = 0x92,
	/** 2.9V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_2_9V = 0x93,
	/** 3V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_3V = 0x94,
	/** 3.1V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_3_1V = 0x95,
	/** 3.2V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_3_2V = 0x96,
	/** 3.3V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_3_3V = 0x97,
	/** 3.4V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_3_4V = 0x98,
	/** 3.5V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_3_5V = 0x99,
	/** 3.6V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_3_6V = 0x9a,
	/** 3.7V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_3_7V = 0x9b,
	/** 3.8V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_3_8V = 0x9c,
	/** 3.9V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_3_9V = 0x9d,
	/** 4V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_4V = 0x9e,
	/** 4.1V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_4_1V = 0x9f,
	/** 4.2V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_4_2V = 0xa0,
	/** 4.3V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_4_3V = 0xa1,
	/** 4.4V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_4_4V = 0xa2,
	/** 4.5V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_4_5V = 0xa3,
	/** 4.6V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_4_6V = 0xa4,
	/** 4.7V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_4_7V = 0xa5,
	/** 4.8V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_4_8V = 0xa6,
	/** 4.9V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_4_9V = 0xa7,
	/** 5V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_5V = 0xa8,
	/** 5.1V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_5_1V = 0xa9,
	/** 5.2V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_5_2V = 0xaa,
	/** 5.3V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_5_3V = 0xab,
	/** 5.4V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_5_4V = 0xac,
	/** 5.5V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_5_5V = 0xad,
	/** 5.6V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_5_6V = 0xae,
	/** 5.7V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_5_7V = 0xaf,
	/** 5.8V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_5_8V = 0xb0,
	/** 5.9V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_5_9V = 0xb1,
	/** 6V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_6V = 0xb2,
	/** 6.1V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_6_1V = 0xb3,
	/** 6.2V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_6_2V = 0xb4,
	/** 6.3V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_6_3V = 0xb5,
	/** 6.4V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_6_4V = 0xb6,
	/** 6.5V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_6_5V = 0xb7,
	/** 6.6V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_6_6V = 0xb8,
	/** 6.7V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_6_7V = 0xb9,
	/** 6.8V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_6_8V = 0xba,
	/** 6.9V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_6_9V = 0xbb,
	/** 7V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_7V = 0xbc,
	/** 7.1V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_7_1V = 0xbd,
	/** 7.2V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_7_2V = 0xbe,
	/** 7.3V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_7_3V = 0xbf,
	/** 7.4V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_7_4V = 0xc0,
	/** 7.5V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_7_5V = 0xc1,
	/** 7.6V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_7_6V = 0xc2,
	/** 7.7V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_7_7V = 0xc3,
	/** 7.8V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_7_8V = 0xc4,
	/** 7.9V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_7_9V = 0xc5,
	/** 8V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_8V = 0xc6,
	/** 8.1V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_8_1V = 0xc7,
	/** 8.2V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_8_2V = 0xc8,
	/** 8.3V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_8_3V = 0xc9,
	/** 8.4V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_8_4V = 0xca,
	/** 8.5V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_8_5V = 0xcb,
	/** 8.6V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_8_6V = 0xcc,
	/** 8.7V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_8_7V = 0xcd,
	/** 8.8V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_8_8V = 0xce,
	/** 9V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_9V = 0x23,
	/** 9.2V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_9_2V = 0x24,
	/** 9.4V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_9_4V = 0x25,
	/** 9.6V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_9_6V = 0x26,
	/** 9.8V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_9_8V = 0x27,
	/** 10V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_10V = 0x28,
	/** 10.2V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_10_2V = 0x29,
	/** 10.4V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_10_4V = 0x2a,
	/** 10.6V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_10_6V = 0x2b,
	/** 10.8V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_10_8V = 0x2c,
	/** 11V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_11V = 0x2d,
	/** 11.2V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_11_2V = 0x2e,
	/** 11.4V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_11_4V = 0x2f,
	/** 11.6V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_11_6V = 0x30,
	/** 11.8V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_11_8V = 0x31,
	/** 12V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_12V = 0x32,
	/** 12.2V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_12_2V = 0x33,
	/** 12.4V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_12_4V = 0x34,
	/** 12.6V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_12_6V = 0x35,
	/** 12.8V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_12_8V = 0x36,
	/** 13V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_13V = 0x37,
	/** 13.2V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_13_2V = 0x38,
	/** 13.4V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_13_4V = 0x39,
	/** 13.6V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_13_6V = 0x3a,
	/** 13.8V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_13_8V = 0x3b,
	/** 14V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_14V = 0x3c,
	/** 14.2V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_14_2V = 0x3d,
	/** 14.4V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_14_4V = 0x3e,
	/** 14.6V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_14_6V = 0x3f,
	/** 14.8V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_14_8V = 0x40,
	/** 15V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_15V = 0x41,
	/** 15.2V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_15_2V = 0x42,
	/** 15.4V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_15_4V = 0x43,
	/** 15.6V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_15_6V = 0x44,
	/** 15.8V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_15_8V = 0x45,
	/** 16V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_16V = 0x46,
	/** 16.2V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_16_2V = 0x47,
	/** 16.4V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_16_4V = 0x48,
	/** 16.6V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_16_6V = 0x49,
	/** 16.8V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_16_8V = 0x4a,
	/** 17V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSH_17V = 0x4b,
};

/** Source driving voltage VSL */
enum ssd1675a_source_driving_voltage_vsl {
	/** -9V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_9V = 0x1a,
	/** -9.5V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_9_5V = 0x1c,
	/** -10V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_10V = 0x1e,
	/** -10.5V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_10_5V = 0x20,
	/** -11V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_11V = 0x22,
	/** -11.5V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_11_5V = 0x24,
	/** -12V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_12V = 0x26,
	/** -12.5V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_12_5V = 0x28,
	/** -13V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_13V = 0x2a,
	/** -13.5V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_13_5V = 0x2c,
	/** -14V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_14V = 0x2c,
	/** -14.5V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_14_5V = 0x30,
	/** -15V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_15V = 0x32,
	/** -15.5V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_15_5V = 0x34,
	/** -16V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_16V = 0x36,
	/** -16.5V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_16_5V = 0x38,
	/** -17V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_17V = 0x3a,
	/** -17.5V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_17_5V = 0x3c,
	/** -18V */
	SSD1675A_SOURCE_DRIVING_VOLTAGE_VSL_MINUS_18V = 0x3e,
};

/** Booster Soft Start Phase Min Off Time Setting of GDR */
enum ssd1675a_booster_soft_start_phase_gdr_min_off_time {
	/** 2.6 */
	SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_2_6 = 0x04,
	/** 3.2 */
	SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_3_2 = 0x05,
	/** 3.9 */
	SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_3_9 = 0x06,
	/** 4.6 */
	SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_4_6 = 0x07,
	/** 5.4 */
	SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_5_4 = 0x08,
	/** 6.3 */
	SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_6_3 = 0x09,
	/** 7.3 */
	SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_7_3 = 0x0a,
	/** 8.4 */
	SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_8_4 = 0x0b,
	/** 9.8 */
	SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_9_8 = 0x0c,
	/** 11.5 */
	SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_11_5 = 0x0d,
	/** 13.8 */
	SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_13_8 = 0x0e,
	/** 16.5 */
	SSD1675A_BOOSTER_SOFT_START_PHASE_GDR_MIN_OFF_TIME_16_5 = 0x0f,
};

/** Booster Soft Start Phase Duration */
enum ssd1675a_booster_soft_start_phase_duration {
	/** 10ms */
	SSD1675A_BOOSTER_SOFT_START_PHASE_DURATION_10MS,
	/** 20ms */
	SSD1675A_BOOSTER_SOFT_START_PHASE_DURATION_20MS,
	/** 30ms */
	SSD1675A_BOOSTER_SOFT_START_PHASE_DURATION_30MS,
	/** 40ms */
	SSD1675A_BOOSTER_SOFT_START_PHASE_DURATION_40MS,
};

// /** Temperature Sensor Control */
// enum ssd1675a_temperature_sensor {
// 	/** External temperature sensor (POR) */
// 	SSD1675A_TEMPERATURE_SENSOR_EXTERNAL = 0x48,
// 	/** Internal temperature sensor */
// 	SSD1675A_TEMPERATURE_SENSOR_INTERNAL = 0x80,
// };

/** VCOM Register */
enum ssd1675a_vcom_register {
	/** -0.2V */
	SSD1675A_VCOM_REGISTER_MINUS_0_2V = 0x08,
	/** -0.3V */
	SSD1675A_VCOM_REGISTER_MINUS_0_3V = 0x0c,
	/** -0.4V */
	SSD1675A_VCOM_REGISTER_MINUS_0_4V = 0x10,
	/** -0.5V */
	SSD1675A_VCOM_REGISTER_MINUS_0_5V = 0x14,
	/** -0.6V */
	SSD1675A_VCOM_REGISTER_MINUS_0_6V = 0x18,
	/** -0.7V */
	SSD1675A_VCOM_REGISTER_MINUS_0_7V = 0x1c,
	/** -0.8V */
	SSD1675A_VCOM_REGISTER_MINUS_0_8V = 0x20,
	/** -0.9V */
	SSD1675A_VCOM_REGISTER_MINUS_0_9V = 0x24,
	/** -1V */
	SSD1675A_VCOM_REGISTER_MINUS_1V = 0x28,
	/** -1.1V */
	SSD1675A_VCOM_REGISTER_MINUS_1_1V = 0x2c,
	/** -1.2V */
	SSD1675A_VCOM_REGISTER_MINUS_1_2V = 0x30,
	/** -1.3V */
	SSD1675A_VCOM_REGISTER_MINUS_1_3V = 0x34,
	/** -1.4V */
	SSD1675A_VCOM_REGISTER_MINUS_1_4V = 0x38,
	/** -1.5V */
	SSD1675A_VCOM_REGISTER_MINUS_1_5V = 0x3c,
	/** -1.6V */
	SSD1675A_VCOM_REGISTER_MINUS_1_6V = 0x40,
	/** -1.7V */
	SSD1675A_VCOM_REGISTER_MINUS_1_7V = 0x44,
	/** -1.8V */
	SSD1675A_VCOM_REGISTER_MINUS_1_8V = 0x48,
	/** -1.9V */
	SSD1675A_VCOM_REGISTER_MINUS_1_9V = 0x4c,
	/** -2V */
	SSD1675A_VCOM_REGISTER_MINUS_2V = 0x50,
	/** -2.1V */
	SSD1675A_VCOM_REGISTER_MINUS_2_1V = 0x54,
	/** -2.2V */
	SSD1675A_VCOM_REGISTER_MINUS_2_2V = 0x58,
	/** -2.3V */
	SSD1675A_VCOM_REGISTER_MINUS_2_3V = 0x5c,
	/** -2.4V */
	SSD1675A_VCOM_REGISTER_MINUS_2_4V = 0x60,
	/** -2.5V */
	SSD1675A_VCOM_REGISTER_MINUS_2_5V = 0x64,
	/** -2.6V */
	SSD1675A_VCOM_REGISTER_MINUS_2_6V = 0x68,
	/** -2.7V */
	SSD1675A_VCOM_REGISTER_MINUS_2_7V = 0x6c,
	/** -2.8V */
	SSD1675A_VCOM_REGISTER_MINUS_2_8V = 0x70,
	/** -2.9V */
	SSD1675A_VCOM_REGISTER_MINUS_2_9V = 0x74,
	/** -3V */
	SSD1675A_VCOM_REGISTER_MINUS_3V = 0x78,
};

/** LUT */
typedef const uint8_t ssd1675a_lut_t[70];

/** Border Waveform for VBD Option */
enum ssd1675a_border_waveform_vdb_option {
	/** GS Transition */
	SSD1675A_BORDER_WAVEFORM_VDB_OPTION_GS_TRANSITION,
	/** Fix Level */
	SSD1675A_BORDER_WAVEFORM_VDB_OPTION_FIX_LEVEL,
	/** VCOM */
	SSD1675A_BORDER_WAVEFORM_VDB_OPTION_VCOM,
	/** HiZ (POR) */
	SSD1675A_BORDER_WAVEFORM_VDB_OPTION_HiZ,
};

/** Border Waveform for VBD Fix Level */
enum ssd1675a_border_waveform_vdb_fix_level {
	/** VSS (POR) */
	SSD1675A_BORDER_WAVEFORM_VDB_FIX_LEVEL_VSS,
	/** VSH1 */
	SSD1675A_BORDER_WAVEFORM_VDB_FIX_LEVEL_VSH1,
	/** VSL */
	SSD1675A_BORDER_WAVEFORM_VDB_FIX_LEVEL_VSL,
	/** VSH2 */
	SSD1675A_BORDER_WAVEFORM_VDB_FIX_LEVEL_VSH2,
};

/** Border Waveform for VBD GS Transition */
enum ssd1675a_border_waveform_vdb_gs_transition {
	/** LUT0 (POR) */
	SSD1675A_BORDER_WAVEFORM_VDB_GS_TRANSITION_LUT0,
	/** LUT1 */
	SSD1675A_BORDER_WAVEFORM_VDB_GS_TRANSITION_LUT1,
	/** LUT2 */
	SSD1675A_BORDER_WAVEFORM_VDB_GS_TRANSITION_LUT2,
	/** LUT3 */
	SSD1675A_BORDER_WAVEFORM_VDB_GS_TRANSITION_LUT3,
};


/**
 * Configuration
 * =============
 */



/**
 * Configuration for :c:data:`ssd1675a`.
 *
 * :See also: :c:func:`eglib_Init`.
 */
typedef struct {
	/** Display width. */
	coordinate_t width;
	/** Display height. */
	coordinate_t height;

	/** MUX Gate lines setting (POR=296) */
	uint16_t mux_gate_lines : 9;
	/** First output Gate. */
	enum ssd1675a_first_output_gate first_output_gate : 1;
	/** Scanning order of gate driver. */
	enum ssd1675a_scanning_order_of_gate_driver scanning_order_of_gate_driver : 1;
	/** TB */
	enum ssd1675a_tb tb : 1;

	/** Gate driving voltage */
	enum ssd1675a_gate_driving_voltage gate_driving_voltage : 5;

	/** Source driving voltage VSH1 */
	enum ssd1675a_source_driving_voltage_vsh source_driving_voltage_vsh1 : 8;
	/** Source driving voltage VSH2 */
	enum ssd1675a_source_driving_voltage_vsh source_driving_voltage_vsh2 : 8;
	/** Source driving voltage VSL */
	enum ssd1675a_source_driving_voltage_vsl source_driving_voltage_vsl : 8;

	// /** Booster Soft Start Phase 1 Driving Strength (0 = Weakest, 7 = Strongest) */
	// uint8_t booster_soft_start_phase1_driving_strength : 3;
	// /** Booster Soft Start Phase 1 Min Off Time Setting of GDR */
	// enum ssd1675a_booster_soft_start_phase_gdr_min_off_time booster_soft_start_phase1_min_off_time_setting_of_gdr : 4;
	// /** Booster Soft Start Phase 1 Duration */
	// enum ssd1675a_booster_soft_start_phase_duration booster_soft_start_phase1_duration : 2;
	// /** Booster Soft Start Phase 2 Driving Strength (0 = Weakest, 7 = Strongest) */
	// uint8_t booster_soft_start_phase2_driving_strength : 3;
	// /** Booster Soft Start Phase 2 Min Off Time Setting of GDR */
	// enum ssd1675a_booster_soft_start_phase_gdr_min_off_time booster_soft_start_phase2_min_off_time_setting_of_gdr : 4;
	// /** Booster Soft Start Phase 2 Duration */
	// enum ssd1675a_booster_soft_start_phase_duration booster_soft_start_phase2_duration : 2;
	// /** Booster Soft Start Phase 3 Driving Strength (0 = Weakest, 7 = Strongest) */
	// uint8_t booster_soft_start_phase3_driving_strength : 3;
	// /** Booster Soft Start Phase 3 Min Off Time Setting of GDR */
	// enum ssd1675a_booster_soft_start_phase_gdr_min_off_time booster_soft_start_phase3_min_off_time_setting_of_gdr : 4;
	// /** Booster Soft Start Phase 3 Duration */
	// enum ssd1675a_booster_soft_start_phase_duration booster_soft_start_phase3_duration : 2;
	
	/** Gate scan start position from 0 to 295 (POR=0) */
	uint16_t gate_scan_start_position : 9;

	// /** Temperature Sensor Control */
	// enum ssd1675a_temperature_sensor temperature_sensor;

	/** Display update control 2 */
	uint8_t display_update_control2;

	/** VCOM Register */
	enum ssd1675a_vcom_register vcom_register : 8;

	/** LUT register (70 bytes) */
	ssd1675a_lut_t *lut_register;

	/** Dummy line period in terms of TGate */
	uint8_t dummy_line_period : 7;

	/** Gate line width (TGate) (POR=0x0a) */
	uint8_t gate_line_width : 4;

	/** Border Waveform for VBD Option */
	enum ssd1675a_border_waveform_vdb_option border_waveform_vdb_option : 2;
	/** Border Waveform for VBD Fix Level */
	enum ssd1675a_border_waveform_vdb_fix_level border_waveform_vdb_fix_level : 2;
	/** Border Waveform for VBD GS Transition */
	enum ssd1675a_border_waveform_vdb_gs_transition border_waveform_vdb_gs_transition : 2;

	/** Analog block control (POR=0x54) */
	uint8_t analog_block_control;

	/** Digital block control (POR=0x3b) */
	uint8_t digital_block_control;
} ssd1675a_config_t;

/**
 * Pre-baked configurations
 * ------------------------
 */

/**
 * Configuration for `BuyDisplay Red 2.9" inch e-Ink Display Module 296x128
 * <https://www.buydisplay.com/red-2-9-inch-e-ink-display-module-296x128-for-arduino-raspberry-pi>`_.
 * Should also work with
 * `Yellow 2.9" e-Paper Display Module 296x128 for Arduino,Raspberry Pi <https://www.buydisplay.com/yellow-2-9-inch-e-paper-display-module-296x128-for-arduino-raspberry-pi-1>_`.
 *
 * Values extracted from BuyDisplay's provided example code.
 */
extern ssd1675a_config_t ssd1675a_config_buydisplay_red_2_9_inch_e_ink_display_module;

/**
 * Variant of :c:data:`ssd1675a_config_buydisplay_red_2_9_inch_e_ink_display_module`
 * but with custom voltages and LUT for ~3x faster update (~6s instead of ~18s).
 */
extern ssd1675a_config_t ssd1675a_config_buydisplay_red_2_9_inch_e_ink_display_module_fast_update;

/**
 * Configuration for
 * `Black 2.9" inch e-Paper Display Module for Arduino,Raspberry Pi <https://www.buydisplay.com/black-2-9-inch-e-paper-display-module-for-arduino-raspberry-pi>`_.
 *
 * Values extracted from BuyDisplay's provided example code. Unfortunately,
 * BuyDisplay example code does not provide LUT for this BW display (only for
 * BWR/BWR), but :c:data:`ssd1675a_lut_bw_adafruit` seems to work just fine.
 *
 * :See also: :c:func:`ssd1675a_writeLut`,
 *   :c:data:`ssd1675a_lut_bw_adafruit` and
 *   :c:data:`ssd1675a_lut_bw_partial`.
 */
extern ssd1675a_config_t ssd1675a_config_buydisplay_black_2_9_inch_e_ink_display_module;

/**
 * Driver
 * ======
 */

/**
 * SSD1675A display driver.
 *
 * :See also: :c:func:`eglib_Init`.
 * :See also: `Datasheet <https://www.buydisplay.com/download/ic/SSD1675A.pdf>`_.
 */
extern const display_t ssd1675a;

/**
 * Functions
 * =========
 *
 * These functions can be used exclusively with :c:type:`eglib_t` initialized
 * with :c:data:`ssd1675a`.
 */

/**
 * Full update LUT for
 * `Adafruit_CircuitPython_SSD1675 <https://github.com/adafruit/Adafruit_CircuitPython_SSD1675>_`.
 *
 * This driver is for a different controller SSD1675, but the LUT should be
 * compatible with similar BW displays.
 */
extern ssd1675a_lut_t ssd1675a_lut_bw_adafruit;

/**
 * Partial update LUT for
 * :c:data:`ssd1675a_config_buydisplay_black_2_9_inch_e_ink_display_module`.
 *
 * This fast screen update causes no flickering, at the expense of image
 * ghosting.
 *
 * **Warning:** doing repeated partial updates without issuing full updates once
 * in a while will permanently polarize the display causing permanent damage as
 * image burn in!
 */
extern ssd1675a_lut_t ssd1675a_lut_bw_partial;

/**
 * Set LUT.
 *
 * :param lut: LUT register (70 bytes).
 */
void ssd1675a_writeLut(eglib_t *eglib, ssd1675a_lut_t *lut);

#endif