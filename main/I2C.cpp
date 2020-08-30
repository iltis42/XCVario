#include <inttypes.h>
#include <stdio.h>
#include "I2C.h"
#include <string.h>
#include "sdkconfig.h"
#include <logdef.h>

#define I2C_MASTER_TX_BUF_DISABLE    0   /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE    0   /*!< I2C master do not need buffer */

xSemaphoreHandle I2C::mutex = 0;

void I2C::init(gpio_num_t sda, gpio_num_t scl, uint32_t frequency, i2c_port_t num)
{
	ESP_LOGI(FNAME,"I2C(sda=%02x  scl=%02x)", sda, scl);
	bool error=false;
	_sda=sda;
	_scl=scl;
	_num=num;
	i2c_config_t conf;
	memset( &conf, 0, sizeof(conf) );
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = sda;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_io_num = scl;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = frequency;
	esp_err_t ret = i2c_param_config(_num, &conf);
	mutex = xSemaphoreCreateMutex();
	if( mutex == 0 )
		ESP_LOGE(FNAME,"Error creating I2C Semaphore");
	else
		ESP_LOGI(FNAME,"I2C Semaphore successfully created");
	if( ret == ESP_FAIL){
				ESP_LOGE(FNAME,"I2C ERROR param config: %x", ret);
				error=true;
			}
	ret = i2c_driver_install(_num, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
	if( ret == ESP_FAIL){
			ESP_LOGE(FNAME,"I2C ERROR driver install: %x", ret);
			error=true;
		}
	if(!error){
		ESP_LOGI(FNAME,"I2C init SUCCESS");
	}
}

esp_err_t I2C::write(uint8_t byte, int ack, i2c_rw_t rw)
{
	esp_err_t ret = i2c_master_write_byte(cmd, (byte<<1)| rw , ack);
	if( ret == ESP_FAIL){
		ESP_LOGE(FNAME,"I2C ERROR write byte: %x", ret);
	}
	return ret;
}

esp_err_t I2C::write_byte(uint8_t byte, int ack)
{
	esp_err_t ret = i2c_master_write_byte(cmd, byte , ack);
	if( ret == ESP_FAIL){
		ESP_LOGE(FNAME,"I2C ERROR write data byte: %x", ret);
	}
	return ret;
}

esp_err_t I2C::write16bit( uint8_t addr, uint16_t word )
{
	xSemaphoreTake(mutex,portMAX_DELAY );
	cmd = i2c_cmd_link_create();
	uint8_t datah=(uint8_t(word>>8));
	uint8_t datal=(uint8_t(word & 0xFF));
	start();
	write(addr, true, I2C_MASTER_WRITE);
    write_byte(datal, 0);
	write_byte(datah, 1);
	stop();
	esp_err_t ret = i2c_master_cmd_begin(_num, cmd, 100 / portTICK_RATE_MS);
    if( ret == ESP_FAIL){
		ESP_LOGE(FNAME,"I2C ERROR write 16 bit: %x", ret);
	}
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(mutex);
    return ret;
}

esp_err_t I2C::write8bit( uint8_t addr, uint16_t word )
{
	xSemaphoreTake(mutex,portMAX_DELAY );
	cmd = i2c_cmd_link_create();
	uint8_t datal=(uint8_t(word & 0xFF));
	start();
	write(addr, true, I2C_MASTER_WRITE);
    write_byte(datal, 0);
	stop();
	esp_err_t ret = i2c_master_cmd_begin(_num, cmd, 100 / portTICK_RATE_MS);
    if( ret == ESP_FAIL){
		ESP_LOGE(FNAME,"I2C ERROR write 8 bit: %x", ret);
		;
	}
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(mutex);
    return ret;
}


esp_err_t I2C::read16bit( uint8_t addr, uint16_t *word )
{
	xSemaphoreTake(mutex,portMAX_DELAY );
	cmd = i2c_cmd_link_create();
	uint8_t datah, datal;
	start();
	write(addr, true, I2C_MASTER_READ);
    read(&datah, 0);
	read(&datal, 1);
	stop();
	esp_err_t ret = i2c_master_cmd_begin(_num, cmd, 100 / portTICK_RATE_MS);
    if( ret == ESP_FAIL){
		ESP_LOGE(FNAME,"I2C ERROR read 16 bit: %x", ret);
	}
    *word = (datah << 8) | datal;
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(mutex);
    return ret;
}

esp_err_t I2C::read8bit( uint8_t addr, uint16_t *word )
{
	xSemaphoreTake(mutex,portMAX_DELAY );
	cmd = i2c_cmd_link_create();
	uint8_t datal;
	start();
	write(addr, true, I2C_MASTER_READ);
	read(&datal, 1);
	stop();
	esp_err_t ret = i2c_master_cmd_begin(_num, cmd, 100 / portTICK_RATE_MS);
    if( ret == ESP_FAIL){
		ESP_LOGE(FNAME,"I2C ERROR read 8 bit: %x", ret);
    	;
	}
    *word = (uint16_t)datal;
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(mutex);
    return ret;
}


esp_err_t I2C::scan()
{
	bool found=false;
	cmd = i2c_cmd_link_create();
	for( uint8_t addr=3; addr<0x78; addr++ )
	{
		ESP_LOGI(FNAME,"scan a %02x sca %02x scl %02x", addr, _sda, _scl );
		start();
		esp_err_t ret = write( addr, true );
		stop();
		if( ret == ESP_OK )
		{
			ESP_LOGI(FNAME,"Found I2C address: %02x", addr);
			found = true;
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}

	}
	i2c_cmd_link_delete(cmd);
	if( !found )
	{
		ESP_LOGI(FNAME,"I2C scan found nothing");
	}
	vTaskDelay(5000 / portTICK_PERIOD_MS);
	return ESP_OK;
}

esp_err_t I2C::read(uint8_t *byte, int ack) //(i2c_ack_type_t)
{
	esp_err_t ret = i2c_master_read_byte(cmd, byte, (i2c_ack_type_t)ack); //Read data back on B
	if (ret == ESP_FAIL) {
		ESP_LOGE(FNAME,"ERROR master read byte I2C link: %x", ret);
	}
	return ret;
}


void I2C::start()
{
	i2c_master_start(cmd);
}


void I2C::stop()
{
	i2c_master_stop(cmd);
}

