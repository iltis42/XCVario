#pragma once

#include "comm/DataLink.h"
#include "comm/InterfaceCtrl.h"

#include "SetupNG.h"

#include <driver/uart.h>
#include <driver/gpio.h>

#include <atomic>

typedef enum { BAUD_4800, BAUD_9600, BAUD_19200, BAUD_38400, BAUD_57600, BAUD_115200 } e_baud;
typedef enum { SM_FLARM, SM_RADIO, SM_XCTNAV_S3, SM_OPENVARIO, SM_XCFLARMBIN, SM_XCFLARMVIEW } e_profile;

typedef struct _s_serial_cfg {
	e_profile  profile:5;
	e_baud     baud:4;
	bool       polarity:1;
	bool       pin_swp:1;
	bool       tx_ena:1;
} t_serial_cfg;

typedef struct serial_setup {
	SetupNG<int> *baud;
	SetupNG<int> *polarity;
	SetupNG<int> *pin_swp;
	SetupNG<int> *tx_ena;
} t_serial_nvs_setup;


class SerialLine final : public InterfaceCtrl {
public:
    SerialLine(uart_port_t uart, gpio_num_t rx_pin, gpio_num_t tx_pin);   // uart: [0 = console], 1 = S1, 2 = S2
    virtual ~SerialLine();

    void setBaud(e_baud baud) { cfg.baud = baud; };    // BAUD_9600, etc
    void setLineInverse(bool apol) { cfg.polarity = apol; } // 0: Normal, 1: Invert
    void setPinSwap(bool pinswp) { cfg.pin_swp = pinswp; }  // 0: Standard 1: Swapped
    void setTxOn(bool tx) { cfg.tx_ena = tx; }           // 0: no tx  1: tx on

    // Ctrl
    InterfaceId getId() const override { return _intfid; }
    const char* getStringId() const override { return _id_memo; }
    void ConfigureIntf(int cfg) override;                // 0:SM_FLARM, 1:SM_RADIO, 2: ..
    int Send(const char *msg, int &len, int port=0) override;

    // integrated vom ex HardwareSerial
    void flush();
    int number() const { return uart_nr; }

private:
    void loadProfile(e_profile profile);    // load defaults according to profile given
    void loadSetupDefaults();
    void applyBaud();
    void applyPins();
    void applyLineInverse();
    void start();                           // starts RS232 interface

    t_serial_cfg cfg;
   
    // UART hardware
    const uart_port_t uart_nr;
    gpio_num_t tx_gpio;
    gpio_num_t rx_gpio;
   
    const InterfaceId   _intfid;
    const char*         _id_memo;
    const t_serial_nvs_setup _setup;

    // Receiving data
    friend void uartTask(SerialLine *arg);
    void* _event_queue = nullptr;
    void* _iotask = nullptr;
    // Sending data with uart buffer
};

