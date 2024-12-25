#ifndef __BTSENDER_H__
#define __BTSENDER_H__

#include "RingBufCPP.h"
#include <string>

#define RFCOMM_SERVER_CHANNEL 1
#define HEARTBEAT_PERIOD_MS 50
#define SPP_SERVICE_BUFFER_SIZE 1024

class BluetoothSerial;

class BTSender {

public:
  BTSender() { SerialBT = 0; };
  void begin();
  static int queueFull();
  static void btTask(void *pvParameters);
  static void progress();  // progress loop
  bool selfTest();         // call 3 seconds after begin

private:

   static BluetoothSerial *SerialBT;

};

#endif
