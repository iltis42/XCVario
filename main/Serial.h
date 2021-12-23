#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <cstring>
#include "driver/gpio.h"
#include <esp_log.h>
#include "RingBufCPP.h"
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define SERIAL_STRLEN SSTRLEN

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
  static void setRxTxNotifier( const uint8_t eventMask );

private:
  static unsigned char* flarmBuf;
  static unsigned short flarmBufFilled;
  static bool _selfTest;
  static EventGroupHandle_t rxTxNotifier;
};

#endif
