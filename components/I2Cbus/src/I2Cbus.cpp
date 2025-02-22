/* =========================================================================
I2Cbus library is placed under the MIT License
Copyright 2017 Natanael Josue Rabello. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
 ========================================================================= */

#include "I2Cbus.hpp"
#include <stdio.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
// #include "driver/i2c.h"
#include "i2c.h"
#include "driver/gpio.h"
#include <iostream>
#include <cstring>


// #define CONFIG_I2CBUS_LOG_RW_LEVEL_INFO
// #define CONFIG_I2CBUS_LOG_READWRITES
// #define CONFIG_I2CBUS_LOG_ERRORS

#define I2C_TIMEOUT 30

#if defined   CONFIG_I2CBUS_LOG_RW_LEVEL_INFO
#define I2CBUS_LOG_RW(format, ...) ESP_LOGI(TAG, format, ##__VA_ARGS__)
#elif defined CONFIG_I2CBUS_LOG_RW_LEVEL_DEBUG
#define I2CBUS_LOG_RW(format, ...) ESP_LOGD(TAG, format, ##__VA_ARGS__)
#elif defined CONFIG_I2CBUS_LOG_RW_LEVEL_VERBOSE
#define I2CBUS_LOG_RW(format, ...) ESP_LOGV(TAG, format, ##__VA_ARGS__)
#endif
#if defined CONFIG_I2CBUS_LOG_ERRORS
#define I2CBUS_LOGE(format, ...)   ESP_LOGE(TAG, format, ##__VA_ARGS__)
#endif

#define I2CBUS_LOGI(format, ...)   ESP_LOGI(TAG, format, ##__VA_ARGS__)

#define I2C_MASTER_ACK_EN   true    /*!< Enable ack check for master */
#define I2C_MASTER_ACK_DIS  false   /*!< Disable ack check for master */


static const char* TAG __attribute__((unused)) = "I2Cbus";

// Protect multithreading by semaphore
xSemaphoreHandle i2cbus_mutex_0 = 0;
xSemaphoreHandle i2cbus_mutex_1 = 0;

xSemaphoreHandle *i2cbus_mutex = 0;

/*******************************************************************************
 * OBJECTS
 ******************************************************************************/
I2C_t i2c0 = i2cbus::I2C(I2C_NUM_0);
I2C_t i2c1 = i2cbus::I2C(I2C_NUM_1);

/* ^^^^^^
 * I2Cbus
 * ^^^^^^ */
namespace i2cbus {

bool I2C::initDone[2] = { false, false };

/*******************************************************************************
 * SETUP
 ******************************************************************************/
I2C::I2C(i2c_port_t port) : port{port}, ticksToWait{pdMS_TO_TICKS(kDefaultTimeout)} {
}

I2C::~I2C() {
    close();
}

esp_err_t I2C::begin(gpio_num_t sda_io_num, gpio_num_t scl_io_num, uint32_t clk_speed) {
    return begin(sda_io_num, scl_io_num, GPIO_PULLUP_ENABLE, GPIO_PULLUP_ENABLE, clk_speed);
}

esp_err_t I2C::begin(gpio_num_t sda_io_num, gpio_num_t scl_io_num, gpio_pullup_t sda_pullup_en, gpio_pullup_t scl_pullup_en, uint32_t clk_speed) {
	if( initDone[port] )
		i2c_driver_delete(port);

	i2c_set_alive_interval(port, 20 );

	i2c_config_t conf;
	memset( &conf, 0, sizeof(conf) );
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = sda_io_num;
    conf.sda_pullup_en = sda_pullup_en;
    conf.scl_io_num = scl_io_num;
    conf.scl_pullup_en = scl_pullup_en;
    conf.master.clk_speed = clk_speed;
    if( port == 0 ){
    	I2CBUS_LOGI("Port Sempaphore: %d init", port);
    	if( i2cbus_mutex_0 == 0 )
    		i2cbus_mutex_0 = xSemaphoreCreateMutex();
    	i2cbus_mutex = &i2cbus_mutex_0;
    }
    else{
    	I2CBUS_LOGI("Port Sempaphore: %d init", port);
    	if( i2cbus_mutex_1 == 0 )
    		i2cbus_mutex_1 = xSemaphoreCreateMutex();
    	i2cbus_mutex = &i2cbus_mutex_1;
    }
    esp_err_t err = i2c_param_config(port, &conf);
    // i2c_set_stop_timing( port, 200, 200 );

    if (!err) err = i2c_driver_install(port, conf.mode, 0, 0, 0);
    initDone[port] = true;
    i2c_filter_enable(port, 7 );
    int setup, hold;
    i2c_get_stop_timing( port, &setup, &hold );
    I2CBUS_LOGI("stop timing setup %d, hold %d",setup, hold );
    i2c_set_stop_timing( port, setup, hold+1500 );
    i2c_get_stop_timing( port, &setup, &hold );
    I2CBUS_LOGI("new stop timing: setup %d, hold %d",setup, hold );
    int sample,hold_d;
    i2c_get_data_timing( port, &sample, &hold_d );
    I2CBUS_LOGI("data timing sample %d, hold %d",sample, hold_d );
    return err;
}

esp_err_t I2C::close() {
    return i2c_driver_delete(port);
}

void I2C::setTimeout(uint32_t ms) {
    ticksToWait = pdMS_TO_TICKS(ms);
}

esp_err_t I2C::write8bit( uint8_t addr, uint16_t word )
{
	xSemaphoreTake(*i2cbus_mutex,portMAX_DELAY );
	// I2CBUS_LOGI("write8bit NS");
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	uint8_t datal=(uint8_t(word & 0xFF));
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (addr<<1)| I2C_MASTER_WRITE , I2C_MASTER_ACK);
	esp_err_t ret = i2c_master_write_byte(cmd, datal , I2C_MASTER_NACK);  // ACK = 0, NACK = 1
	i2c_master_stop(cmd);
	ret |= i2c_master_cmd_begin(port, cmd, pdMS_TO_TICKS(I2C_TIMEOUT) );
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(*i2cbus_mutex);
    return ret;
}

esp_err_t I2C::write2bytes( uint8_t addr, uint8_t byte1, uint8_t byte2 )
{
	xSemaphoreTake(*i2cbus_mutex,portMAX_DELAY );
	// I2CBUS_LOGI("write8bit NS");
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (addr<<1)| I2C_MASTER_WRITE , I2C_MASTER_ACK);
	esp_err_t ret = i2c_master_write_byte(cmd, byte1 , I2C_MASTER_ACK);  // ACK = 0, NACK = 1
	ret |= i2c_master_write_byte(cmd, byte2 , I2C_MASTER_ACK);  // ACK = 0, NACK = 1
	i2c_master_stop(cmd);
	ret |= i2c_master_cmd_begin(port, cmd, pdMS_TO_TICKS(I2C_TIMEOUT) );
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(*i2cbus_mutex);
    return ret;
}

esp_err_t I2C::read8bit( uint8_t addr, uint16_t *word )
{
	xSemaphoreTake(*i2cbus_mutex,portMAX_DELAY );
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	uint8_t datal;
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (addr<<1)| I2C_MASTER_READ , I2C_MASTER_ACK);
	esp_err_t ret = i2c_master_read_byte(cmd, &datal, I2C_MASTER_NACK); //Read data back on B
	i2c_master_stop(cmd);
	ret |= i2c_master_cmd_begin(port, cmd, pdMS_TO_TICKS(I2C_TIMEOUT) );
    *word = (uint16_t)datal;
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(*i2cbus_mutex);
    return ret;
}

esp_err_t I2C::read16bit( uint8_t addr, uint16_t *word )
{

	xSemaphoreTake(*i2cbus_mutex,portMAX_DELAY);
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	uint8_t datah, datal;
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (addr<<1)| I2C_MASTER_READ , I2C_MASTER_ACK);
	i2c_master_read_byte(cmd, &datah, I2C_MASTER_ACK); //Read data back on B
	i2c_master_read_byte(cmd, &datal, I2C_MASTER_NACK); //Read data back on B
	i2c_master_stop(cmd);
	esp_err_t ret = i2c_master_cmd_begin(port, cmd, pdMS_TO_TICKS(I2C_TIMEOUT) );
    *word = (datah << 8) | datal;
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(*i2cbus_mutex);
    return ret;
}


/*******************************************************************************
 * WRITING
 ******************************************************************************/
esp_err_t I2C::writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data, int32_t timeout) {
    uint8_t buffer;
    esp_err_t err = readByte(devAddr, regAddr, &buffer, timeout);
    if (err) return err;
    buffer = data ? (buffer | (1 << bitNum)) : (buffer & ~(1 << bitNum));
    return writeByte(devAddr, regAddr, buffer, timeout);
}

esp_err_t I2C::writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data, int32_t timeout) {

    uint8_t buffer;
    esp_err_t err = readByte(devAddr, regAddr, &buffer, timeout);
    if (err) return err;
    uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    data <<= (bitStart - length + 1);
    data &= mask;
    buffer &= ~mask;
    buffer |= data;
    return writeByte(devAddr, regAddr, buffer, timeout);
}

esp_err_t I2C::writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data, int32_t timeout) {
	return writeBytes(devAddr, regAddr, 1, &data, timeout);
}



esp_err_t I2C::writeBytes(uint8_t devAddr, uint8_t regAddr, size_t length, const uint8_t *data, int32_t timeout) {
	xSemaphoreTake(*i2cbus_mutex,portMAX_DELAY );
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (devAddr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK_EN);
    i2c_master_write_byte(cmd, regAddr, I2C_MASTER_ACK_EN);
    i2c_master_write(cmd, (uint8_t*) data, length, I2C_MASTER_ACK_EN);
    i2c_master_stop(cmd);
    // i2c_set_timeout( port, pdMS_TO_TICKS(I2C_TIMEOUT) );
    esp_err_t err = i2c_master_cmd_begin(port, cmd, pdMS_TO_TICKS(I2C_TIMEOUT) );
    i2c_cmd_link_delete(cmd);
#if defined CONFIG_I2CBUS_LOG_READWRITES
    if (!err) {
        char str[length*5+1];
        for (size_t i = 0; i < length; i++)
            sprintf(str+i*5, "0x%s%X ", (data[i] < 0x10 ? "0" : ""), data[i]);
        I2CBUS_LOG_RW("[port:%d, slave:0x%X] Write %d bytes to register 0x%X, data: %s",
            port, devAddr, length, regAddr, str);
    }
#endif
#if defined CONFIG_I2CBUS_LOG_ERRORS
#if defined CONFIG_I2CBUS_LOG_READWRITES
    else {
#else
    if (err) {
#endif
        // I2CBUS_LOGE("[port:%d, slave:0x%X] Failed to write %d bytes to__ register 0x%X, error: 0x%X", port, devAddr, length, regAddr, err);
    }
#endif
    xSemaphoreGive(*i2cbus_mutex);
    /*
    if (err) {
            I2CBUS_LOGE("[port:%d, slave:0x%X] Failed to write %d bytes to__ register 0x%X, error: 0x%X",
                port, devAddr, length, regAddr, err);
    }
    */
    return err;
}


/*******************************************************************************
 * READING
 ******************************************************************************/
esp_err_t I2C::readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, int32_t timeout) {
    return readBits(devAddr, regAddr, bitNum, 1, data, timeout);
}

esp_err_t I2C::readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, int32_t timeout) {
    uint8_t buffer;
    esp_err_t err = readByte(devAddr, regAddr, &buffer, timeout);
    if (!err) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        buffer &= mask;
        buffer >>= (bitStart - length + 1);
        *data = buffer;
    }
    return err;
}

esp_err_t I2C::readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, int32_t timeout) {
    return readBytes(devAddr, regAddr, 1, data, timeout);
}

esp_err_t I2C::readBytes(uint8_t devAddr, uint8_t regAddr, size_t length, uint8_t *data, int32_t timeout) {
	xSemaphoreTake(*i2cbus_mutex,portMAX_DELAY );
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (devAddr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK_EN);
    i2c_master_write_byte(cmd, regAddr, I2C_MASTER_ACK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (devAddr << 1) | I2C_MASTER_READ, I2C_MASTER_ACK_EN);
    i2c_master_read(cmd, data, length, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    // esp_err_t err = i2c_master_cmd_begin(port, cmd, (timeout < 0 ? ticksToWait : pdMS_TO_TICKS(timeout)));
    // i2c_set_timeout( port, pdMS_TO_TICKS(I2C_TIMEOUT) );
    esp_err_t err = i2c_master_cmd_begin(port, cmd,  pdMS_TO_TICKS(I2C_TIMEOUT) );
    i2c_cmd_link_delete(cmd);
#if defined CONFIG_I2CBUS_LOG_READWRITES
    if (!err ) {
    	if( 1 /* port == 0  */) {
    		char str[length*5+1];
    		for (size_t i = 0; i < length; i++)
    			sprintf(str+i*5, "0x%s%X ", (data[i] < 0x10 ? "0" : ""), data[i]);
    		I2CBUS_LOG_RW("[port:%d, slave:0x%X] Read_ %d bytes from register 0x%X, data: %s", port, devAddr, length, regAddr, str);
    	}
    }
#endif
#if defined CONFIG_I2CBUS_LOG_ERRORS
#if defined CONFIG_I2CBUS_LOG_READWRITES
    else {
#else
    if (err) {
#endif
        // I2CBUS_LOGE("[port:%d, slave:0x%X] Failed to read %d bytes from register 0x%X, error: 0x%X", port, devAddr, length, regAddr, err);
    }
#endif
    xSemaphoreGive(*i2cbus_mutex);
    /*
    if (err) {
         I2CBUS_LOGE("[port:%d, slave:0x%X] Failed to read %d bytes from register 0x%X, error: 0x%X",
             port, devAddr, length, regAddr, err);
     }
     */
    return err;
}


/*******************************************************************************
 * UTILS
 ******************************************************************************/
esp_err_t I2C::testConnection(uint8_t devAddr, int32_t timeout) {
	xSemaphoreTake(*i2cbus_mutex,portMAX_DELAY );
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (devAddr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK_EN);
    i2c_master_stop(cmd);
    esp_err_t err = i2c_master_cmd_begin(port, cmd, pdMS_TO_TICKS(I2C_TIMEOUT) );
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(*i2cbus_mutex);
    return err;
}


void I2C::scanner() {
    constexpr int32_t scanTimeout = 20;
    printf(LOG_COLOR_W "\n>> I2C scanning ..." LOG_RESET_COLOR "\n");
    uint8_t count = 0;
    for (size_t i = 0x3; i < 0x78; i++) {
        if (testConnection(i, scanTimeout) == ESP_OK) {
            printf(LOG_COLOR_W "- Device found at address 0x%X%s", i, LOG_RESET_COLOR "\n");
            count++;
        }
    }
    if (count == 0)
        printf(LOG_COLOR_E "- No I2C devices found!" LOG_RESET_COLOR "\n");
    printf("\n");
}

}  // namespace i2cbus

