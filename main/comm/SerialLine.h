#pragma once

#include "comm/DataLink.h"
#include "comm/InterfaceCtrl.h"

#include "setup/SetupNG.h"

#include <driver/uart.h>
#include <driver/gpio.h>

#include <cinttypes>
#include <atomic>

typedef enum { BAUD_2400, BAUD_4800, BAUD_9600, BAUD_19200, BAUD_38400, BAUD_57600, BAUD_115200 } e_baud; // stay compatible with baudrates in existing setup
typedef enum { SM_FLARM, SM_RADIO, SM_XCTNAV_S3, SM_OPENVARIO, NR_OF_SER_PROFILES } e_profile;
typedef enum { RS232_NORMAL, RS232_TTL } rs232lm_t;


union t_serial_cfg {
	struct {
		e_baud     baud:4;
		bool       polarity:1;
		bool       pin_swp:1;
		bool       tx_ena:1;
	};
	uint16_t raw = 0;
	constexpr t_serial_cfg(int r) : raw(r) {}
	constexpr t_serial_cfg(int b, bool pty, bool swp, bool tx) :
		raw((b & 0xf) | (pty & 0x1)<<4 | (swp & 0x1)<<5 | (tx & 0x1)<<6) {}
};

struct t_serial_nvs_setup {
	SetupNG<int> *baud;
	SetupNG<int> *polarity;
	SetupNG<int> *pin_swp;
	SetupNG<int> *tx_ena;
	constexpr t_serial_nvs_setup(SetupNG<int>* ba, SetupNG<int>* po, SetupNG<int>* pi, SetupNG<int>* tx) : 
		baud(ba), polarity(po), pin_swp(pi), tx_ena(tx) {}
};


class SerialLine final : public InterfaceCtrl {
public:
    SerialLine(uart_port_t uart, gpio_num_t rx_pin, gpio_num_t tx_pin);   // uart: [0 = console], 1 = S1, 2 = S2
    virtual ~SerialLine();

    void setBaud(e_baud baud) { cfg.baud = baud; applyBaud(); };    // BAUD_9600, etc
    void setLineInverse(bool apol) { cfg.polarity = apol; applyLineInverse(); } // 0: Normal, 1: Invert
    void setPinSwap(bool pinswp) { cfg.pin_swp = pinswp; applyPins(); }  // 0: Standard 1: Swapped
    void setTxOn(bool tx) { cfg.tx_ena = tx; applyPins(); }           // 0: no tx  1: tx on

    // Ctrl
    InterfaceId getId() const override { return _intfid; }
    const char* getStringId() const override { return _id_memo; }
    void ConfigureIntf(int cfg) override;                // 0:SM_FLARM, 1:SM_RADIO, 2: ..
    int Send(const char *msg, int &len, int port=0) override;
    void stop(); // the ConfigureIntf call would start it again

    // integrated vom ex HardwareSerial
    void flush();
    int number() const { return uart_nr; }
    int getBreakCount() const { return break_count; }

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
    int break_count = 0;
    // Sending data with uart buffer
};

