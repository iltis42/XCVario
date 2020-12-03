#include <btstack.h>
#include <esp_log.h>
#include "RingBufCPP.h"
#include <string>
#ifndef __BTSENDER_H__
#define __BTSENDER_H__

#define RFCOMM_SERVER_CHANNEL 1
#define HEARTBEAT_PERIOD_MS 30
#define SPP_SERVICE_BUFFER_SIZE 200

class BTSender {

public:
  BTSender() { };
  void begin();
  void send( char * s );
  static int queueFull();
  static void transmit(SString &s);
  bool selfTest() { return bluetooth_up; };  // call 3 seconds after begin

private:
   static bool 		bluetooth_up;
   static uint8_t   rfcomm_channel_nr;
   static uint8_t   spp_service_buffer[SPP_SERVICE_BUFFER_SIZE];
   static btstack_packet_callback_registration_t hci_event_callback_registration;
   static void bt_data_packet (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
};

#endif
