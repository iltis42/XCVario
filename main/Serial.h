/**
 * Serial.h
 *
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


typedef struct xcv_serial {
  const char* name;
	RingBufCPP<SString, QUEUE_SIZE>* tx_q;
	RingBufCPP<SString, QUEUE_SIZE>* rx_q;
	void (*route)();
	HardwareSerial *uart;
	uint8_t uart_nr;
	uint8_t rx_char;
	uint8_t rx_nl;
	uint8_t tx_req;
	uint8_t monitor;
	TaskHandle_t pid;
}xcv_serial_t;

class Serial {
public:
  Serial() {
  }

  static void begin();
  static void taskStart();
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
    xEventGroupSetBits( rxTxNotifier, eventMask );
  }

  /**
   * Handle Serial1 RX data, if Flarm text mode is set.
   */
  static void handleS1TextMode();

  /**
   * Handle Serial 1/2 RX data, if Flarm works in text mode.
   */
  static void handleTextMode( bool &flarmExitCmd, xcv_serial_t *cfg );

  static void routeRxData( SString& s, xcv_serial_t *cfg );

  static void setStopRouting( const uint8_t uart_nr, bool flag )
  {
    if( uart_nr > 2 )
      return;
    _stopRouting[uart_nr] = flag;
  }

  static bool stopRouting( const uint8_t uart_nr )
  {
    if( uart_nr > 2 )
      return false;
    return _stopRouting[uart_nr];
  }

  static void clearStopRouting()
  {
    _stopRouting[0] = false;
    _stopRouting[1] = false;
    _stopRouting[2] = false;
  }

private:
  static bool _selfTest;
  static EventGroupHandle_t rxTxNotifier;
  // Stop routing of TX/RX data. That is used in case of Flarm binary download.
  static bool _stopRouting[3];
};

#endif
