#pragma once
#include "driver/gpio.h"
#include "driver/twai.h"


#define CAN_BUS_TX_PIN GPIO_NUM_26
#define CAN_BUS_RX_PIN GPIO_NUM_33

class SString;

class CANbus {
public:
	CANbus(){};
	~CANbus(){};
	void begin();
	void restart();
	bool selfTest();
    bool GotNewClient() const { return _new_can_client_connected; }
    void ResetNewClient() { _new_can_client_connected = false; }
	bool connectedXCV() { return _connected_xcv; };
	bool connectedMagSens() { return _connected_magsens; };
	bool connected() { return( _connected_xcv | _connected_magsens ); };
	bool isOkay() { return _ready_initialized; };

private:
    friend void canTxTask(void *arg);
    friend void canRxTask(void *arg);
	void txtick(int);
	void rxtick(int);

private:
	bool sendData( int id, const char* msg, int length, int self=0 );
	bool sendNMEA( const SString& msg );
	int receive(  int *id, SString& msg, int timeout=5);
	void driverInstall( twai_mode_t mode );
	void driverUninstall();
	bool _ready_initialized = false;
	gpio_num_t _tx_io = CAN_BUS_TX_PIN;
	gpio_num_t _rx_io = CAN_BUS_RX_PIN;
	bool _connected_magsens = false;
	bool _connected_xcv = false;
    bool _new_can_client_connected = false;
	int _connected_timeout_magsens = 0;
	int _connected_timeout_xcv = 0;
    TickType_t _tx_timeout = 2; // [msec] about two times the time for 111 bit to send
};

extern CANbus* CAN;
