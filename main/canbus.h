#pragma once
#include "driver/gpio.h"
#include "driver/twai.h"


#define CAN_BUS_TX_PIN GPIO_NUM_26
#define CAN_BUS_RX_PIN GPIO_NUM_33

#define CAN_NMEA_ID_MASTER      0x20
#define CAN_CONFIG_ID_MASTER    0x21
#define CAN_KEEPALIVE_ID_MASTER 0x11

#define CAN_NMEA_ID_CLIENT      0x22
#define CAN_CONFIG_ID_CLIENT    0x23
#define CAN_KEEPALIVE_ID_CLIENT 0x12

#define CAN_MAGSENS_ID          0x31

class SString;

class CANbus {
public:
	CANbus();
	~CANbus(){};
	void begin();
	void restart();
	void recover();
	bool selfTest(bool rs_bit);
    bool GotNewClient() const { return _new_can_client_connected; }
    void ResetNewClient() { _new_can_client_connected = false; }
	bool connectedXCV() { return _connected_xcv; };
	bool connectedMagSens() { return _connected_magsens; };
	bool connected() { return( _connected_xcv | _connected_magsens ); };
	bool isOkay() { return _ready_initialized; };
	bool sendNMEA( const SString& msg );
	bool hasSlopeSupport() { return _slope_support; };

private:
    friend void canTxTask(void *arg);
    friend void canRxTask(void *arg);
	void txtick(int);
	void rxtick(int);

private:
	bool sendData( int id, const char* msg, int length, int self=0 );
	int receive(  int *id, SString& msg, int timeout=5);
	void driverInstall( twai_mode_t mode );
	void driverUninstall();
	gpio_num_t _tx_io = CAN_BUS_TX_PIN;
	gpio_num_t _rx_io = CAN_BUS_RX_PIN;
	bool _ready_initialized = false;
	bool _connected_magsens = false;
	bool _connected_xcv = false;
	bool _slope_support = false;
    bool _new_can_client_connected = false;
    bool _keep_alive_fails = false;
    bool _send_nmea_fails = false;
	int _connected_timeout_magsens = 0;
	int _connected_timeout_xcv = 0;
	int _can_id_config_tx;  // to unify CAN id's, the following _can* variables are initialized depending on master/client role in constructor
	int _can_id_config_rx;
	int _can_id_nmea_tx;
	int _can_id_nmea_rx;
	int _can_id_keepalive_tx;
	int _can_id_keepalive_rx;
    TickType_t _tx_timeout = 2; // [msec] about two times the time for 111 bit to send
};

extern CANbus* CAN;
