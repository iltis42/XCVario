#include "stream.h"
#include <stdlib.h>

static void init(eglib_t *eglib) {
	four_wire_spi_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	config->stream = open_memstream(&(config->buffer), &(config->sizeloc));

	if(config->stream == NULL)
		exit(1);

	fprintf(config->stream, "init\n");
}

static void sleep_in(eglib_t *eglib) {
	four_wire_spi_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	fprintf(config->stream, "sleep_in\n");
}

static void sleep_out(eglib_t *eglib) {
	four_wire_spi_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	fprintf(config->stream, "sleep_out\n");
}

static void delay_ns(eglib_t *eglib, uint32_t ns) {
	four_wire_spi_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	fprintf(config->stream, "delay_ns %lu\n", (unsigned long int)ns);
}

static void set_reset(eglib_t *eglib, bool state) {
	four_wire_spi_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	fprintf(config->stream, "set_reset %d\n", state);
}

static bool get_busy(eglib_t *eglib) {
	four_wire_spi_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	fprintf(config->stream, "get_busy\n");
	return false;
}

static void comm_begin(eglib_t *eglib) {
	four_wire_spi_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	fprintf(config->stream, "comm_begin\n");
}

static void send(
	eglib_t *eglib,
	enum hal_dc_t dc,
	uint8_t *bytes,
	uint32_t length
) {
	four_wire_spi_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	fprintf(config->stream, "send\n");
	fprintf(config->stream, "  dc %d\n", dc);
	fprintf(config->stream, "  bytes");
	for(uint32_t i=0 ; i < length ; i++)
		fprintf(config->stream, " %02x", bytes[i]);

	fprintf(config->stream, "\n");
}

static void comm_end(eglib_t *eglib) {
	four_wire_spi_stream_config_t *config;

	config = eglib_GetHalConfig(eglib);

	fprintf(config->stream, "comm_end\n");
}

const hal_t four_wire_spi_stream = {
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