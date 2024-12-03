/**
 * Serial.h
 *
 * 02.01.2022 Axel Pauli: handling of 2 uart channels in one method accomplished.
 * 01.01.2022 Axel Pauli: updates after first delivery.
 * 24.12.2021 Axel Pauli: added some RX/TX handling stuff.
 */

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <cstring>
#include "SString.h"
#include "driver/gpio.h"
#include <esp_log.h>
#include "RingBufCPP.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "Router.h"
#include "HardwareSerial.h"
#include "DataLink.h"

#define SERIAL_STRLEN SSTRLEN

// Event mask definitions
#define RX0_CHAR 1
#define RX0_NL 2
#define RX1_CHAR 4
#define RX1_NL 8
#define RX2_CHAR 16
#define RX2_NL 32
#define TX1_REQ 64
#define TX2_REQ 128

// All data of one Uart channel
typedef struct xcv_serial {
	const char* name;
	RingBufCPP<SString>* tx_q;
	//RingBufCPP<SString>* rx_q;
	// void (*route)();
	HardwareSerial *uart;
	uint8_t rx_char;
	uint8_t rx_nl;
	uint8_t tx_req;
	uint8_t monitor;
	TaskHandle_t pid;
	xcv_serial *cfg2; // configuration of other Uart
	bool route_disable;
	DataLinkOld* dl;
	int port; // info is in dl, tb deleted
} xcv_serial_t;

class Serial {
public:
	Serial(){
	}

	static void begin();
	static void taskStart();
	static void taskStartS1();
	static void taskStartS2();
	static void taskStop( int uart_nr );
	static bool taskStarted( int num );
	static void serialHandler(void *pvParameters);
	static bool selfTest( int num );
	/*
	 * Uart event bits
	 * Bit 0: Uart 0 RX any character received
	 * Bit 1: Uart 0 RX nl received
	 * Bit 2: Uart 1 RX any character received
	 * Bit 3: Uart 1 RX nl received
	 * Bit 4: Uart 2 RX RX any character received
	 * Bit 5: Uart 2 RX nl received
	 * Bit 6: Uart 1 TX characters to send
	 * Bit 7: Uart 1 TX characters to send
	 */
	static void setRxTxNotifier( const uint8_t eventMask )
	{
		if( rxTxNotifier )
			xEventGroupSetBits( rxTxNotifier, eventMask );
	};

	/*
	 *  Pacing for second serial interface, disable interrupt, clear queue, etc.
	 */
	static void enterBincomMode( xcv_serial_t *cfg );

	/*
	 *  Exit binary mode
	 */
	static void exitBincomMode( xcv_serial_t *cfg );

	/**
	 * Stop data routing of the Uart channel.
	 */
	static void setroutingStopped( xcv_serial_t *cfg, const bool flag )
	{
		cfg->route_disable = flag;
	};

	/**
	 * Query the stop routing flag.
	 */
	static bool routingStopped( xcv_serial_t *cfg )
	{
		return cfg->route_disable;
	};


private:
	static bool _selfTest;
	static EventGroupHandle_t rxTxNotifier;
	// Stop routing of TX/RX data. That is used in case of Flarm binary download.
	static bool bincom_mode;
	static xcv_serial_t S1;
	static xcv_serial_t S2;
};

#endif
