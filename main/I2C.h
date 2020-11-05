#include <inttypes.h>
#include "esp_system.h"
#include <driver/i2c.h>

#ifndef I2C_h
#define I2C_h

#define I2C_MASTER_FREQ_HZ         100000   /*!< I2C master clock frequency 100000 */
#define I2C_MASTER_TX_BUF_DISABLE    0      /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE    0      /*!< I2C master do not need buffer */

class I2C
{
  public:
	I2C() { _scl = GPIO_NUM_0; _sda = GPIO_NUM_0; cmd = 0; _num=I2C_NUM_0; };
    void init(gpio_num_t sda, gpio_num_t scl, uint32_t frequeny=I2C_MASTER_FREQ_HZ, i2c_port_t num=I2C_NUM_0);
    esp_err_t write( uint8_t address, int ack=0, i2c_rw_t rw=I2C_MASTER_READ);
    esp_err_t write_byte( uint8_t address, int ack=0);
    esp_err_t read( uint8_t *byte, int ack=1);
    esp_err_t read16bit( uint8_t addr, uint16_t *word );
    esp_err_t read8bit( uint8_t addr, uint16_t *word );
    esp_err_t write16bit( uint8_t addr, uint16_t word );
    esp_err_t read32bit( uint8_t addr, uint8_t *data );
    esp_err_t write8bit( uint8_t addr, uint16_t word );  // write only lower byte from word
    void start();
    void stop();
    esp_err_t scan( uint8_t addr = 0 );
  private:
    // i2c_cmd_handle_t cmd;
    gpio_num_t _sda;
    gpio_num_t _scl;
    i2c_port_t _num;
    i2c_cmd_handle_t cmd;
    static xSemaphoreHandle mutex;
};


#endif
