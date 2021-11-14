#include "esp32_ili9341.h"
#include "SPI.h"
#include "driver/gpio.h"

static esp32_hal_config_t *config;

/*
.spi_num = 2;
	.freq = 10000000;  // 10 MHz
	.dataMode = SPI_MODE3;
	.bitOrder = MSBFIRST;
	.gpio_scl = SPI_SCLK;
	.gpio_sda = SPI_MOSI;
	.gpio_sdi = SPI_MISO;
	.gpio_cs  = CS_Display;
	.gpio_rs  = RESET_Display

*/

static void init(eglib_t *eglib) {
	SPI.begin( config->gpio_scl, config->gpio_sdi, config->gpio_sda, config->gpio_cs );
	(void)eglib;
	config = (esp32_hal_config_t *)eglib_GetHalConfig(eglib);
	// init GPIO pins of 4 WIRE SPI bus
	gpio_set_direction(config->gpio_sdi, GPIO_MODE_INPUT);
	gpio_set_direction(config->gpio_sda, GPIO_MODE_OUTPUT);
	gpio_set_direction(config->gpio_scl, GPIO_MODE_OUTPUT);
	gpio_set_direction(config->gpio_cs, GPIO_MODE_OUTPUT);
	gpio_set_direction(config->gpio_rs, GPIO_MODE_OUTPUT);
	gpio_set_direction(config->gpio_dc, GPIO_MODE_OUTPUT);
}

static void sleep_in(eglib_t *_eglib) {
	// we are not using this, ESP32 SPI sleep is not helpful for XCVario
}

static void sleep_out(eglib_t *_eglib) {
	// dito
}

static void delay_ns(eglib_t *_eglib, uint32_t ns) {
	while( ns ){
		ns--;
	}
}

static void set_reset(eglib_t *_eglib, bool state) {
	gpio_set_level(config->gpio_rs, (unsigned int)state );
}

static bool get_busy(eglib_t *_eglib) {
  return false;
}

static void comm_begin(eglib_t *_eglib) {
	gpio_set_level(config->gpio_cs, 0 );
	SPI.beginTransaction(SPISettings( config->freq, config->bitOrder, config->dataMode ));  // *3
}

static void send(
	eglib_t *_eglib,
	enum hal_dc_t dc,
	uint8_t *bytes,
	uint32_t length )
{
	gpio_set_level(config->gpio_dc, dc );
	SPI.transfer( bytes, length );
}

static void comm_end(eglib_t *_eglib) {
	SPI.endTransaction();
	gpio_set_level(config->gpio_cs, 1 );
}

const hal_t esp32_ili9341 = {
	.init = init,
	.sleep_in = sleep_in,
	.sleep_out = sleep_out,
	.delay_ns = delay_ns,
	.set_reset = set_reset,
	.get_busy = get_busy,
	.comm_begin = comm_begin,
	.send = send,
	.comm_end = comm_end,
};
