#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include "driver/uart.h"
#include "driver/gpio.h"
#include "comm/InterfaceCtrl.h"

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

class SerialLine: public InterfaceCtrl {
public:
    SerialLine(uart_port_t uart);                    // [0 = console], 1 = S1, 2 = S2
    virtual ~SerialLine() {};
    void loadProfile(e_profile profile);                // load defaults according to profile given
    void configure();                                   // SM_FLARM, SM_RADIO, etc
    void setBaud(e_baud baud, bool coldstart=false);	// BAUD_9600, etc
    void setLineInverse(e_polarity apol);               // 0: Normal, 1: Invert
    void setPinSwap( _e_pin pin );                      // 0: Standard 1: Swapped
    void setSlaveRole( e_tx tx );                       // Slave role or TX disable e.g. readonly FLARM CLIENT, e.g. we are parallel with another transmitting device with Master Role
    inline void restart() { stop(); start(); };         // recycle RS232 interface
    void uartBegin();
    virtual void ConfigureIntf(int cfg);
    virtual bool Send(const char *msg, int len, int port=0);

private:
    void getGPIOPins();
    t_serial_cfg cfg;
    uart_port_t uart_nr;
    gpio_num_t tx_gpio;
    gpio_num_t rx_gpio;
    static gpio_num_t prior_S1_tx_gpio;
    static gpio_num_t prior_S1_rx_gpio;
    static gpio_num_t prior_S2_tx_gpio;
    static gpio_num_t prior_S2_rx_gpio;
    void start();                                             // starts RS232 interface
    void stop();                                              // stops RS232 interface
};

#endif // SERIALMANAGER_H
