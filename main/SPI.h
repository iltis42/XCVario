/*
  BME280_ESP32_SPI.h  Bosch BMP/BME280 for ESP32
  Version 1.1

Reference library:
https://github.com/BoschSensortec/BMP280_driver

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.   If not, see <http://www.gnu.org/licenses/>.

Written: Dec 7 2017.
Last Updated: Dec 07 2017.
This header must be included in any derived code or copies of the code.
Based on the data sheet provided by Bosch for the BMP280 environmental sensor,
calibration code based on algorithms provided by referenced Bosch library,
altitude calculation by open source community on github.

*/

#ifndef ESP32_SPI_h_
#define ESP32_SPI_h_
#include <esp_system.h>
#include "driver/gpio.h"
#include <esp_system.h>
#include "driver/spi_master.h"
#include <string.h>
#include "sdkconfig.h"
#include "freertos/task.h"


class SPI
{
public:
	SPI() {	 bus_conf=false;
			 memset( _handle, 0, sizeof( _handle ) );
		   };
	spi_device_handle_t _handle[GPIO_NUM_MAX];
	bool bus_conf;
	static bool _disable;
	void begin(uint8_t sclk, uint8_t mosi, uint8_t miso );
	void setDataMode(int mode, uint8_t cs, uint32_t freq, int queue=0, bool use_cs=true, int pretans=0);
	uint8_t transfer( gpio_num_t cs, uint8_t txbyte, uint8_t rxlen=1 );
	uint8_t send( gpio_num_t cs, uint8_t *txdata, uint8_t txlen, uint8_t *rxdata );
	static inline void disable( bool flag ) { _disable = flag; };

};

extern SPI _Spi;

#endif
