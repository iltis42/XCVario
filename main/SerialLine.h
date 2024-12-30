#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include "driver/uart.h"
#include "driver/gpio.h"
#include "comm/DataLink.h"
#include "comm/InterfaceCtrl.h"
#include "HardwareSerial.h"
#include "RingBufCPP.h"
#include "SString.h"

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

class SerialLine final : public InterfaceCtrl {
public:
    SerialLine(uart_port_t uart);                       // [0 = console], 1 = S1, 2 = S2
    virtual ~SerialLine() {};
    void loadProfile(e_profile profile);                // load defaults according to profile given
    void configure();                                   // SM_FLARM, SM_RADIO, etc
    void setBaud(e_baud baud, bool coldstart=false);	// BAUD_9600, etc
    void setLineInverse(e_polarity apol);               // 0: Normal, 1: Invert
    void setPinSwap( _e_pin pin );                      // 0: Standard 1: Swapped
    void setSlaveRole( e_tx tx );                       // Slave role or TX disable e.g. readonly FLARM CLIENT, e.g. we are parallel with another transmitting device with Master Role
    void uartBegin();
    // Ctrl
    InterfaceId getId() const override { return _intfid; }
    const char* getStringId() const override { return _id_memo; }
    virtual void ConfigureIntf(int cfg);
    virtual int Send(const char *msg, int len, int port=0);  // #fixme: port obsoleted, to be removed

    // integrated vom ex HardwareSerial
    void receive( const char *msg, int len, int port=0 );
    size_t read(uint8_t *buffer, size_t size);
    void flush();
    void enableRxInterrupt();
    int number() const;  // returns UART number
    int available();     //
    int availableForWrite();
    size_t write(const char *buffer, size_t size);
    uint16_t readBufFromQueue( uint8_t* buffer, const size_t len);
    void setRxNotifier( EventGroupHandle_t egh );

private:
    void setupGPIOPins();
    t_serial_cfg cfg;
    HardwareSerial *hw_serial;
    uart_port_t uart_nr;
    gpio_num_t tx_gpio;
    gpio_num_t rx_gpio;
    uint8_t tx_req;
    static gpio_num_t prior_tx_gpio[3];
    static gpio_num_t prior_rx_gpio[3];
    RingBufCPP<SString>* tx_q;
    uart_t* _uart;
    void stop();                                              // stops RS232 interface
    const InterfaceId   _intfid;
    const char*         _id_memo;
};

#endif // SERIALMANAGER_H
