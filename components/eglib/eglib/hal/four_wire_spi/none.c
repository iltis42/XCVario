#include "none.h"

static void init(eglib_t *eglib) {
	(void)eglib;
}

static void sleep_in(eglib_t *eglib) {
	(void)eglib;
}

static void sleep_out(eglib_t *eglib) {
	(void)eglib;
}

static void delay_ns(eglib_t *eglib, uint32_t ns) {
	(void)eglib;
	(void)ns;
}

static void set_reset(eglib_t *eglib, bool state) {
	(void)eglib;
	(void)state;
}

static bool get_busy(eglib_t *eglib) {
  (void)eglib;
  return false;
}

static void comm_begin(eglib_t *eglib) {
	(void)eglib;
}

static void send(
	eglib_t *eglib,
	enum hal_dc_t dc,
	uint8_t *bytes,
	uint32_t length
) {
	(void)eglib;
	(void)dc;
	(void)bytes;
	(void)length;
}

static void comm_end(eglib_t *eglib) {
	(void)eglib;
}

const hal_t four_wire_spi_none = {
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