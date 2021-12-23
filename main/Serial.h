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
  Serial() {};
  static void begin();
  static void taskStart();
  static void serialHandler1(void *pvParameters);
  static void serialHandler2(void *pvParameters);
  static bool selfTest( int num );
  /*
   * Uart event bits
   * Bit 0: Uart 0 RX
   * Bit 1: Uart 1 RX
   * Bit 2: Uart 2 RX
   * Bit 4: Uart 0 TX
   * Bit 5: Uart 1 TX
   * Bit 6: Uart 2 TX
   */
  static void setRxTxNotifier( const uint8_t eventMask );

private:
  static char rxbuf[SERIAL_STRLEN];
  static bool _selfTest;
  static EventGroupHandle_t rxTxNotifier;
};

#endif
