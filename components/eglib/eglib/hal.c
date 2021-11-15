#include "hal.h"
#include <stdio.h>

void eglib_CommBegin(eglib_t *eglib) {
	eglib->hal.comm_active = true;
	eglib->hal.i2c_send_slave_addr++;
	eglib->hal.driver->comm_begin(eglib);
}

void eglib_Send(
	eglib_t *eglib,
	enum hal_dc_t dc,
	uint8_t *bytes,
	uint32_t length
) {
	printf("eglib_Send() &eglib:%x &bytes:%x len:%d  dc:%d hal-driv:%x hds:%x\n", (unsigned int)eglib, (unsigned int)bytes, length, dc, (unsigned int)eglib->hal.driver, (unsigned int)eglib->hal.driver->send );
	uint8_t prev_hal_i2c_send_slave_addr;

	prev_hal_i2c_send_slave_addr = eglib->hal.i2c_send_slave_addr;

	eglib->hal.driver->send(eglib, (uint32_t)dc, bytes, length);

	if(eglib->hal.i2c_send_slave_addr == prev_hal_i2c_send_slave_addr) {
		if(eglib->hal.i2c_send_slave_addr)
			eglib->hal.i2c_send_slave_addr--;
	} else {
		if(eglib->hal.i2c_send_slave_addr)
			eglib->hal.i2c_send_slave_addr--;
		// I2C ReStart
		if(!eglib->hal.i2c_send_slave_addr)
			eglib->hal.i2c_send_slave_addr++;
	}
}

void eglib_CommEnd(eglib_t *eglib) {
	// Send pending I2C Slave address
	if(eglib->hal.i2c_send_slave_addr)
		eglib->hal.driver->send(eglib, 0, NULL, 0);
	eglib->hal.driver->comm_end(eglib);
	eglib->hal.comm_active = false;
	eglib->hal.i2c_send_slave_addr = 0;
}

void eglib_DelayS(eglib_t *eglib, uint32_t s) {
	for(uint32_t i=0 ; i < s/4 ; i++)
		eglib_DelayNs(eglib, 4 * 1000000000UL);
	eglib_DelayNs(eglib, (s%4) * 1000000000UL);
}
