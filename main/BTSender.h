#include <btstack.h>
#include "driver/gpio.h"
#include <esp_log.h>
#include <string>
#ifndef __BTSENDER_H__
#define __BTSENDER_H__

class BTSender {
public:
  BTSender( void (* cb)(char * rx, uint16_t len)  ) {
	   _callback = cb;
  };
  void begin( bool *enable, char* bt_name );
  void send( char * s );
  static int queueFull();

private:
   static bool *_enable;
   static void ( * _callback)(char * rx, uint16_t len);
   static int i;
   static uint8_t   rfcomm_channel_nr;
   static uint16_t  rfcomm_channel_id;
   static uint8_t   spp_service_buffer[100];
   static btstack_timer_source_t heartbeat;
   static btstack_packet_callback_registration_t hci_event_callback_registration;

   void one_shot_timer_setup(void);
   static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
   static void heartbeat_handler(struct btstack_timer_source *ts);

};

#endif
