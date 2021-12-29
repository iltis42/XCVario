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

class Serial {
public:
  Serial() {
  }

  static void begin();
  static void taskStart();
  static void serialHandler1(void *pvParameters);
  static void serialHandler2(void *pvParameters);
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
  static void handleTextMode( uint8_t uartNum, bool &flarmExitCmd );

  static void routeS1RxData( SString& s );
  static void routeS2RxData( SString& s );

private:
  static bool _selfTest;
  static EventGroupHandle_t rxTxNotifier;
};

#endif
