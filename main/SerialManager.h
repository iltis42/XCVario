#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include "driver/uart.h"
#include "driver/gpio.h"

typedef enum _e_baud { BAUD_OFF, BAUD_4800, BAUD_9600, BAUD_19200, BAUD_57600, BAUD_115200 } e_baud;
typedef enum _e_polarity { RS232_STANDARD, RS232_TTL } e_polarity;
typedef enum _e_pin { RJ45_3TX_4RX, RJ45_3RX_4TX } e_pin;
typedef enum _e_tx { SM_SLAVE, SM_MASTER } e_tx;
typedef enum _e_profile { SM_FLARM, SM_RADIO, SM_XCTNAV_S3, SM_OPENVARIO, SM_XCFLARMVIEW } e_profile;

typedef struct _s_serial_cfg {
	e_profile  prof:5;
	e_baud     baud:3;
	e_polarity pol:1;
	e_pin      pin:1;
	e_tx        tx:1;
} t_serial_cfg;

class SerialManager {
public:
    SerialManager(uart_port_t uart);                    // [0 = console], 1 = S1, 2 = S2
    void configure(e_profile profile);                  // SM_FLARM, SM_RADIO, etc
    void setBaud(e_baud baud);
    void setLineInverse(e_polarity apol);               // BAUD_9600, etc
    void setPinSwap( _e_pin pin );
    void setSlaveRole( e_tx tx );                       // Slave role or TX disable e.g. readonly FLARM CLIENT, e.g. we are parallel with another transmitting device with Master Role
    inline void restart() { stop(); start(); };         // recycle RS232 interface

private:
    t_serial_cfg cfg;
    uart_port_t uart_nr;
    gpio_num_t tx_gpio;
    gpio_num_t rx_gpio;
    void start();                                             // starts RS232 interface
    void stop();                                              // stops RS232 interface
};

#endif // SERIALMANAGER_H
