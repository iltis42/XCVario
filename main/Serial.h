#include <btstack.h>
#include "driver/gpio.h"
#include <esp_log.h>
#include "RingBufCPP.h"
#include <string>
#ifndef __SERIAL_H__
#define __SERIAL_H__

class Serial {
public:
  Serial() {};
  static void begin();
  static void serialHandlerS1(void *pvParameters);
  static void serialHandlerS2(void *pvParameters);

private:

};

#endif
