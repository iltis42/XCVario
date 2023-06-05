#include "stream.h"
#include <stdlib.h>

//
// Helpers
//

static void i2c_write(eglib_t *eglib, uint8_t byte) {
	i2c_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	fprintf(config->stream, "  byte %d\n", byte);
}

//
// Driver
//

static void init(eglib_t *eglib) {
	i2c_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	config->stream = open_memstream(&(config->buffer), &(config->sizeloc));

	if(config->stream == NULL)
		exit(1);

	fprintf(config->stream, "init\n");
}

static void sleep_in(eglib_t *eglib) {
	i2c_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	fprintf(config->stream, "sleep_in\n");
}

static void sleep_out(eglib_t *eglib) {
	i2c_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	fprintf(config->stream, "sleep_out\n");
}

static void delay_ns(eglib_t *eglib, uint32_t ns) {
	i2c_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	fprintf(config->stream, "delay_ns %lu\n", (unsigned long int)ns);
}

static void set_reset(eglib_t *eglib, bool state) {
	i2c_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	fprintf(config->stream, "set_reset %d\n", state);
}

static bool get_busy(eglib_t *eglib) {
	i2c_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	fprintf(config->stream, "get_busy\n");
	return false;
}

static void comm_begin(eglib_t *eglib) {
	i2c_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	fprintf(config->stream, "comm_begin\n");
}

static void send(
	eglib_t *eglib,
	enum hal_dc_t dc,
	uint8_t *bytes,
	uint32_t length
) {
	i2c_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	fprintf(config->stream, "send\n");

	if(eglib_ShouldSendI2cSlaveAddr(eglib))
		fprintf(
			config->stream,
			"  slave address %d\n",
			eglib_GetI2c7bitSlaveAddr(eglib, dc)
		);

	eglib_I2cSend(eglib, i2c_write, dc, bytes, length);
}

static void comm_end(eglib_t *eglib) {
	i2c_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	fprintf(config->stream, "comm_end\n");
}

const hal_t i2c_stream = {
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