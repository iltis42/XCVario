/*
      SPI.cpp Bosch BME280 for ESP32

 */


#define HIGH 1
#define LOW  0

#include "SPI.h"
#include "BMPVario.h"  // for sleep
#include "freertos/FreeRTOS.h"


bool SPI::_disable = false;

// Wrapper class for SPI interfacing on ESP32

void SPI::begin(uint8_t sclk, uint8_t mosi, uint8_t miso) {
	printf("++SPI::begin( sclk=%d, mosi=%d, miso=%d)\n", sclk, mosi, miso);

	if( !bus_conf && !_disable ){
		spi_bus_config_t bus_config;
		memset( &bus_config,0,sizeof(spi_bus_config_t));
		bus_config.sclk_io_num = sclk; // CLK
		bus_config.mosi_io_num = mosi; // MOSI
		bus_config.miso_io_num = miso; // MISO
		bus_config.quadwp_io_num = -1; // Not used
		bus_config.quadhd_io_num = -1; // Not used

		ESP_ERROR_CHECK( spi_bus_initialize(HSPI_HOST, &bus_config, 1) ); // 0 no DMA, 1 DMA ch 1
		bus_conf = true;
	}
	printf("--SPI::begin()\n");
};


void SPI::setDataMode(int mode, uint8_t cs, uint32_t freq, int queue, bool use_cs, int pretrans ) {
	if( !_handle[cs] && !_disable ){
		printf("SPI::setDataMode mode:%d cs:%d freq:%d q:%d use_cs:%d pretrans:%d\n", mode, cs, freq, queue, use_cs, pretrans );
		spi_device_interface_config_t dev_config;
		memset( &dev_config, 0, sizeof(dev_config));
		dev_config.clock_speed_hz	= freq;
		if( use_cs )
			dev_config.spics_io_num     = cs;
		else
			dev_config.spics_io_num     = -1; // -1 of not used
		dev_config.queue_size		=  queue;  // 200 for display, else 0
		dev_config.mode = mode;
		dev_config.flags = 0;
		ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &dev_config, &_handle[cs]));
		printf("got handle=%04x\n", (uint32_t)_handle[cs] );
	}
};



uint8_t SPI::send( gpio_num_t cs, uint8_t *txdata, uint8_t txlen, uint8_t *rxdata ){
	if( _disable )
		return 0;
	// printf("SPI::send, len %d, cs=%d\n", txlen, cs);
	spi_transaction_t trans_desc;
	trans_desc.addr		= 0;
	trans_desc.cmd		= 0;
	trans_desc.flags	= 0;
	trans_desc.length	= 8*txlen;
	trans_desc.rxlength = 0;
	trans_desc.tx_buffer = txdata;
	trans_desc.rx_buffer = rxdata;
	ESP_ERROR_CHECK( spi_device_transmit(_handle[cs], &trans_desc));
	/*
	if( rxdata != 0 ){
		printf("SPI::send (rx=!0), TX:%02x, txl: %d, RX:%02x RXlen:%d\n", txdata[0], txlen, rxdata[0], trans_desc.rxlength);
	}
	*/

	// 	printf("TX:%02x TXlen:%d\n", txdata[0], txlen);
	if( trans_desc.rxlength == 0 ) {
		printf( "SPI send overrun waiter\n" );
		vTaskDelay( 1000 / portTICK_PERIOD_MS);
	}
	return trans_desc.rxlength;
};


// send and receive one byte over SPI, received byte is returned by this method
uint8_t SPI::transfer( gpio_num_t cs, uint8_t txbyte, uint8_t rxlen ){
	if( _disable )
			return 0;
	uint8_t txdata[4];
	uint8_t rxdata[4];
	memset( &rxdata,0,sizeof(rxdata));
	memset( &txdata,0,sizeof(txdata));
	txdata[0] = txbyte;
	send( cs, txdata, 1, rxdata );
	// printf("SPI::transfer byte tx:%02x rx:%02x, cs%d\n", txbyte, rxdata[0], cs);
	return rxdata[0];
};

SPI _Spi;

