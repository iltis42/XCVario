#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_system.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <cstring>

extern "C" {
#include "esp32_ili9341.h"

extern xSemaphoreHandle spiMutex;

static esp32_hal_config_t *config;
static spi_device_handle_t spi;
static spi_transaction_t ta = {
	.flags = 0,//SPI_TRANS_USE_RXDATA, //| SPI_TRANS_MODE_OCT | SPI_TRANS_CS_KEEP_ACTIVE,
	.cmd = 0,
	.addr = 0,
	.length = 0,
	.rxlength = 0,
	.user = nullptr,
	.tx_buffer = nullptr,
	.rx_buffer = nullptr,
};
static bool init = false;

// static void spi_pre_transfer_callback(spi_transaction_t *t);

static void einit(eglib_t *eglib)
{
	if ( init ) {
		return;
	}
	init = true;
	ESP_LOGI("ILI9341","einit() %p", eglib);
	config = (esp32_hal_config_t *)eglib_GetHalConfig(eglib);
	// hal_four_wire_spi_config_t * fwc = eglib_GetHalFourWireSpiConfigComm(eglib);
	ESP_LOGI("ILI9341","spi%d host%d, mode %d, freq %d", config->spi_num, SPI2_HOST, config->dataMode, config->freq);

	// SPI device configuration
	spi_device_interface_config_t devcfg = {
		.command_bits = 0,
		.address_bits = 0,
		.dummy_bits = 0,
		.mode = config->dataMode,
		.duty_cycle_pos = 0,
		.cs_ena_pretrans = 0,
		.cs_ena_posttrans = 0,
		.clock_speed_hz = (int)config->freq/2,
		.input_delay_ns = 0,
		.spics_io_num = config->gpio_cs,
		.flags = SPI_DEVICE_HALFDUPLEX,
		.queue_size = 1, // Transaction queue size
		.pre_cb = NULL, // spi_pre_transfer_callback, // Pre-transaction callback
		.post_cb = NULL  // Post-transaction callback
	};
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ESP_ERROR_CHECK(spi_bus_add_device((spi_host_device_t)config->spi_num, &devcfg, &spi));
	xSemaphoreGive(spiMutex);

	// init GPIO pins of 4 WIRE SPI bus
	gpio_reset_pin(config->gpio_cs);
	gpio_reset_pin(config->gpio_rs);
	gpio_reset_pin(config->gpio_dc);
	gpio_set_direction(config->gpio_cs, GPIO_MODE_OUTPUT);
	gpio_set_direction(config->gpio_rs, GPIO_MODE_OUTPUT);
	gpio_set_direction(config->gpio_dc, GPIO_MODE_OUTPUT);
	// set default state for all output pins
	gpio_set_level(config->gpio_rs, 1);
	gpio_set_level(config->gpio_dc, 1 );
	gpio_set_level(config->gpio_cs, 1 );
	ESP_LOGI("ILI9341","pins dc%d rs%d cs%d", config->gpio_dc, config->gpio_rs, config->gpio_cs);
}

static void esleep_in(eglib_t *_eglib) {
	ESP_LOGI("ILI9341","sleep in");
	vTaskDelay( 120 / portTICK_PERIOD_MS);
}

static void esleep_out(eglib_t *_eglib) {
	ESP_LOGI("ILI9341","sleep out");
	vTaskDelay( 120 / portTICK_PERIOD_MS);
}

static void edelay_ns(eglib_t *_eglib, uint32_t ns) {
	ESP_LOGI("ILI9341","delay %d ms", ns/1000000 );
	vTaskDelay( (ns/1000000) / portTICK_PERIOD_MS);
}

static void eset_reset(eglib_t *_eglib, bool state) {
	ESP_LOGI("ILI9341","reset IO:%d state=%d", config->gpio_rs, state );
	gpio_set_level(config->gpio_rs, (unsigned int)state );
}

static IRAM_ATTR bool eget_busy(eglib_t *_eglib) {
    return false;
}

static IRAM_ATTR void ecomm_begin(eglib_t *_eglib) {
	// ESP_LOGI("ILI9341", "comm begin(): eglib:%x config:%x CS-PIN:%d", (unsigned int)_eglib, (unsigned int)config, (unsigned int)config->gpio_cs );
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	gpio_set_level(config->gpio_cs, 0 );
	// spi_device_acquire_bus(spi, portMAX_DELAY);
}

// IRAM_ATTR void spi_pre_transfer_callback(spi_transaction_t *t) {
//     // Set DC based on user-defined flag in transaction
//     gpio_set_level(config->gpio_dc, (t->user == (void *)1) ? 1 : 0);
// }

// constexpr int NUM_READ_OPCODES = 14; // Adjust based on the number of known read opcodes

// static const uint8_t read_opcodes[NUM_READ_OPCODES] = {
// 	0x04, // Read Display Status
// 	0x09, // Get Device ID Liest die Geräte-ID des ILI9341 aus.
// 	0x0A, // Get Display Status Liest den Status des Displays aus.
// 	0x0B, // Get Power Mode Liest den aktuellen Power-Modus des Displays aus.
// 	0x0C, // Get MADCTL Liest die aktuelle Speicherzugriffssteuerung (Memory Access Control) aus.
// 	0x0D, // Get Pixel Format Liest das aktuelle Pixel-Format (Farbtiefe, z. B. 16-bit oder 18-bit) aus.
// 	0x0E, // Get Image Format Liest das aktuelle Bildformat aus.
// 	0x0F, // Get Signal Mode Liest den Signalmodus des Displays aus.

// 	0x2E, // Read Memory Liest Bilddaten aus dem Display-Speicher (RAM).
// 	0x4E, // Read ID1 Liest die erste Geräte-ID.
// 	0x4F, // Read ID2 Liest die zweite Geräte-ID.
// 	0xDA, // Read Manufacturer ID Liest die Hersteller-ID aus.
// 	0xDB, // Read Module/Driver Version Liest die Modul- oder Treiberversion aus.
// 	0xDC, // Read Module/Driver ID Liest die Modul- oder Treiber-ID aus.
// };
// bool IRAM_ATTR is_read_opcode(uint8_t opcode)
// {
// 	for (int i = 0; i < NUM_READ_OPCODES; i++)
// 	{
// 		if (read_opcodes[i] == opcode)
// 		{
// 			return true;
// 		}
// 	}
// 	return false;
// }

static IRAM_ATTR void esend(eglib_t *_eglib, enum hal_dc_t dc, uint8_t *bytes, uint32_t length)
{
	// ESP_LOGI("ILI9341", "esend() DC-IO:%d dc:%s len:%d\n",config->gpio_dc,  dc?"DAT":"CMD", length );
	// ESP_LOG_BUFFER_HEXDUMP("ILI9341", bytes, length, ESP_LOG_INFO);
	if (dc == HAL_DATA)
	{
		gpio_set_level(config->gpio_dc, 1);
		// ESP_LOGI("ILI9341", "esend() DAT %d", length );
		// ta.user = (void *)1;
		ta.flags =  0; // SPI_TRANS_MODE_OCT;
		ta.rx_buffer = nullptr;
		ta.tx_buffer = bytes;
		ta.length = length * 8;
	}
	else
	{
		// ESP_LOGI("ILI9341", "esend() CMD 0x%x", *bytes );
		// ta.user = (void *)0;
		ta.flags = SPI_TRANS_USE_TXDATA; // | SPI_TRANS_MODE_OCT;
		ta.rx_buffer = nullptr;
		ta.tx_data[0] = *bytes;
		ta.length = 8;
		gpio_set_level(config->gpio_dc, 0);
	}
	esp_err_t ret = spi_device_transmit(spi, &ta);
	if (ret != ESP_OK)
	{
		ESP_LOGE("SPI", "SPI transfer failed: %s", esp_err_to_name(ret));
	}
}

static IRAM_ATTR void ecomm_end(eglib_t *_eglib) {
	// ESP_LOGI("ILI9341","comm end()");
	gpio_set_level(config->gpio_cs, 1 );
	xSemaphoreGive(spiMutex);
	// spi_device_release_bus(spi);
}

hal_t esp32_ili9341 = {
	.init = einit,
	.sleep_in = esleep_in,
	.sleep_out = esleep_out,
	.delay_ns = edelay_ns,
	.set_reset = eset_reset,
	.get_busy = eget_busy,
	.comm_begin = ecomm_begin,
	.send = esend,
	.comm_end = ecomm_end,
};
}
