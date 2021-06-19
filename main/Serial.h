#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <cstring>
#include "driver/gpio.h"
#include <esp_log.h>
#include "RingBufCPP.h"
#include <string>

class Serial {
public:
  Serial() {};
  static void begin();
  static void taskStart();
  static void serialHandler(void *pvParameters);
  static bool selfTest( int num );

private:

};

#endif
