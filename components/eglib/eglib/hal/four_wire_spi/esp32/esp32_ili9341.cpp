#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "SPI.h"
#include "driver/gpio.h"
#include <esp_log.h>

extern "C" {
#include "esp32_ili9341.h"

static esp32_hal_config_t *config;

static void einit(eglib_t *eglib) {
	ESP_LOGI("ILI9341","init()");
	config = (esp32_hal_config_t *)eglib_GetHalConfig(eglib);
	// SPI.begin( config->gpio_scl, config->gpio_sdi, config->gpio_sda, config->gpio_cs );  // done already by sensors and CS handled in here
	// init GPIO pins of 4 WIRE SPI bus
	gpio_set_direction(config->gpio_sdi, GPIO_MODE_INPUT);
	gpio_set_direction(config->gpio_sda, GPIO_MODE_OUTPUT);
	gpio_set_direction(config->gpio_scl, GPIO_MODE_OUTPUT);
	gpio_set_direction(config->gpio_cs, GPIO_MODE_OUTPUT);
	gpio_set_direction(config->gpio_rs, GPIO_MODE_OUTPUT);
	gpio_set_direction(config->gpio_dc, GPIO_MODE_OUTPUT);

	gpio_set_level(config->gpio_rs, 1);
	gpio_set_level(config->gpio_dc, 1 );
	gpio_set_level(config->gpio_cs, 1 );
	gpio_set_level(config->gpio_scl, 0 );
	gpio_set_level(config->gpio_sda, 1 );

	// hal_four_wire_spi_config_t * fwc = eglib_GetHalFourWireSpiConfigComm(eglib);
	SPI.setClockDivider( SPI_CLOCK_DIV8 );
}

static void esleep_in(eglib_t *_eglib) {
	ESP_LOGI("ILI9341","sleep in");
	// we are not using this, ESP32 SPI sleep is not helpful for XCVario
}

static void esleep_out(eglib_t *_eglib) {
	ESP_LOGI("ILI9341","sleep out");
	// dito
}

static void edelay_ns(eglib_t *_eglib, uint32_t ns) {
	ESP_LOGI("ILI9341","delay %d ms", ns/1000000 );
	vTaskDelay( (ns/1000000) / portTICK_PERIOD_MS);
}

static void eset_reset(eglib_t *_eglib, bool state) {
	ESP_LOGI("ILI9341","reset state=%d", state );
	gpio_set_level(config->gpio_rs, (unsigned int)state );
}

static bool eget_busy(eglib_t *_eglib) {
    return false;
}

static void ecomm_begin(eglib_t *_eglib) {
	ESP_LOGI("ILI9341","comm begin()");
	gpio_set_level(config->gpio_cs, 0 );
	SPI.beginTransaction(SPISettings( config->freq, config->bitOrder, config->dataMode ));  // *3
}

void esend(
	eglib_t *_eglib,
	enum hal_dc_t dc,
	uint8_t *bytes,
	uint32_t length )
{
	ESP_LOGI("ILI9341", "esend() dc:%d len:%d\n", dc, length );
    ESP_LOG_BUFFER_HEXDUMP("ILI9341", bytes, length, ESP_LOG_INFO);
	gpio_set_level(config->gpio_dc, dc );
	SPI.transfer( bytes, length );
}

static void ecomm_end(eglib_t *_eglib) {
	ESP_LOGI("ILI9341","comm end()");
	SPI.endTransaction();
	gpio_set_level(config->gpio_cs, 1 );
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
