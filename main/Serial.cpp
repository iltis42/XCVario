#include <esp_log.h>
#include "BTSender.h"
#include <string>
#include "sdkconfig.h"
#include <stdio.h>
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include <freertos/semphr.h>
#include <algorithm>
#include <HardwareSerial.h>
#include "RingBufCPP.h"
#include <driver/uart.h>
#include "Protocols.h"
#include <logdef.h>
#include "Switch.h"
#include "sensor.h"
#include "Router.h"
#include "Serial.h"
#include "Flarm.h"
#include "Protocols.h"
#include "canbus.h"

/* Note that the standard NMEA 0183 baud rate is only 4.8 kBaud.
Nevertheless, a lot of NMEA-compatible devices can properly work with
higher transmission speeds, especially at 9.6 and 19.2 kBaud.
As any sentence can consist of 82 characters maximum with 10 bit each (including start and stop bit),
any sentence might take up to 171 ms (at 4.8k Baud), 85 ms (at 9.6 kBaud) or 43 ms (at 19.2 kBaud).
This limits the overall channel capacity to 5 sentences per second (at 4.8k Baud), 11 msg/s (at 9.6 kBaud) or 23 msg/s (at 19.2 kBaud).
If  too  many  sentences  are  produced  with  regard  to  the  available  transmission  speed,
some sentences might be lost or truncated.

*/

// Option to simulate FLARM sentences
const char *flarm[] = {
		"$PFLAU,3,1,2,1,1,-60,2,-100,755,1234*\n",
		"$PFLAU,3,1,2,1,1,-20,2,-100,655,1234*\n",
		"$PFLAU,3,1,2,1,1,-10,2,-80,455,1234*\n",
		"$PFLAU,3,1,2,1,2,10,2,-40,155,1234*\n",
		"$PFLAU,3,1,2,1,2,20,2,-20,155,1234*\n",
		"$PFLAU,3,1,2,1,3,30,2,0,155,1234*\n",
		"$PFLAU,3,1,2,1,3,60,2,20,255,1234*\n",
		"$PFLAU,3,1,2,1,2,80,2,40,455,1234*\n",
		"$PFLAU,3,1,2,1,1,90,2,80,855,1234*\n",
		"$PFLAU,3,1,2,1,1,90,2,80,1555,1234*\n"
};

int sim=100;
#define HEARTBEAT_PERIOD_MS_SERIAL 20
#define SERIAL_STRLEN SSTRLEN
static TaskHandle_t *pid;
// char Serial::rxbuf[SERIAL_STRLEN];
bool Serial::_selfTest = false;

// Serial Handler  ttyS1, S1, port 8881
void Serial::serialHandler(void *pvParameters){
	SString s;
	while(true) {
		if( !_selfTest ){
		if( flarm_sim.get() ){
			sim=-3;
			flarm_sim.set( 0 );
		}
		if( sim < 10 ){
			if( sim >= 0 ){
				int cs = Protocols::calcNMEACheckSum( (char *)flarm[sim] );
				char str[80];
				sprintf( str, "%s%02X\r\n", flarm[sim], cs );
				SString sf( str );
				Router::forwardMsg( sf, s1_rx_q );
				ESP_LOGI(FNAME,"Serial FLARM SIM: %s",  sf.c_str() );
			}
			delay(2500);
			sim++;
		}
		// Serial Interface tty1 send
		if ( !s1_tx_q.isEmpty() && Serial1.availableForWrite() ){
			ESP_LOGD(FNAME,"Serial Data and avail");
			while( Router::pullMsg( s1_tx_q, s ) ) {
				// ESP_LOGD(FNAME,"Serial 1 TX len: %d bytes", s.length() );
				// ESP_LOG_BUFFER_HEXDUMP(FNAME,s.c_str(),s.length(), ESP_LOG_DEBUG);
				int wr = Serial1.write( s.c_str(), s.length() );
				DM.monitorString( MON_S1, DIR_TX, s.c_str() );
				ESP_LOGD(FNAME,"Serial 1 TX written: %d", wr);
			}
		}

		int available = Serial1.available();

		// Due to delays the receiver buffer can contain more than one NMEA sentence
		while( available > 0 ) {
			ESP_LOGI(FNAME,"Serial 1 RX avail %d bytes", available );
		  int num = available;
			if( available > SERIAL_STRLEN ) {
				ESP_LOGW(FNAME,"Serial 1 Overrun RX >= %d bytes avail: %d, Bytes", SERIAL_STRLEN, available);
				num=SERIAL_STRLEN;
			}
      char *rxbuf = (char*)malloc( num + 1 );
			// ESP_LOGI(FNAME,"Flarm::bincom %d", Flarm::bincom  );
			int numread = 0;
			if( Flarm::bincom ){    // normally wait unit sentence has ended, or in binary mode just continue
				numread = Serial1.read( rxbuf, num );
			}
			else{
				numread = Serial1.readLine( rxbuf, num );
			}
			if( numread ) {
				// ESP_LOGI(FNAME,"Serial 1 RX bytes read: %d  bincom: %d", numread,  Flarm::bincom  );
				// ESP_LOG_BUFFER_HEXDUMP(FNAME,rx.c_str(),numread, ESP_LOG_INFO);
				s.set( rxbuf, numread );

				rxbuf[numread] = '\0';
				ESP_LOGI(FNAME,"S1: '%s'", rxbuf );

				Router::forwardMsg( s, s1_rx_q );
		    Router::routeS1();
		    Router::routeBT();
		    Router::routeClient();
		    BTSender::progress();   // piggyback this here, saves one task for BT sender
				DM.monitorString( MON_S1, DIR_RX, s.c_str() );

				// Note there could be more NMEA sentences contained in the receiver
				// due to delays.
				available -= numread;
				if( available ) {
				    continue;
				}
			}
			free( rxbuf );
			break;
		}

  if( serial2_speed.get() != 0  && hardwareRevision.get() >= 3 ){
    // ESP_LOGI(FNAME,"Serial 2 tick");
    if ( !s2_tx_q.isEmpty() && Serial2.availableForWrite() ){
      if( Router::pullMsg( s2_tx_q, s ) ) {
        // ESP_LOGD(FNAME,"Serial 2 TX len: %d bytes", s.length() );
        // ESP_LOG_BUFFER_HEXDUMP(FNAME,s.c_str(),s.length(), ESP_LOG_DEBUG);
        int wr = Serial2.write( s.c_str(), s.length() );
        DM.monitorString( MON_S2, DIR_TX, s.c_str() );
        ESP_LOGD(FNAME,"Serial 2 TX written: %d", wr);
      }
    }

    int available = Serial2.available();

    // Due to delays the receiver buffer can contain more than one NMEA sentence
     while( available > 0 ) {
       ESP_LOGI(FNAME,"Serial 2 RX avail %d bytes", available );
       int num = available;
       if( available > SERIAL_STRLEN ) {
         ESP_LOGW(FNAME,"Serial 2 Overrun RX >= %d bytes avail: %d, Bytes", SERIAL_STRLEN, available);
         num=SERIAL_STRLEN;
       }
       char *rxbuf = (char*)malloc( num + 1 );
       // ESP_LOGI(FNAME,"Flarm::bincom %d", Flarm::bincom  );
       int numread = 0;
       if( Flarm::bincom ){    // normally wait unit sentence has ended, or in binary mode just continue
         numread = Serial2.read( rxbuf, num );
       }
       else{
         numread = Serial2.readLine( rxbuf, num );
       }
       if( numread ) {
         // ESP_LOGI(FNAME,"Serial 2 RX bytes read: %d  bincom: %d", numread,  Flarm::bincom  );
         // ESP_LOG_BUFFER_HEXDUMP(FNAME,rx.c_str(),numread, ESP_LOG_INFO);
         s.set( rxbuf, numread );

         rxbuf[numread] = '\0';
         ESP_LOGI(FNAME,"S2: '%s'", rxbuf );

         Router::forwardMsg( s, s2_rx_q );
         Router::routeS2();
         Router::routeBT();
         DM.monitorString( MON_S2, DIR_RX, s.c_str() );

         // Note there could be more NMEA sentences contained in the receiver
         // due to delays.
         available -= numread;
         if( available ) {
             continue;
         }
       }
       free( rxbuf );
       break;
     }
  }
}
    esp_task_wdt_reset();
    if( uxTaskGetStackHighWaterMark( pid ) < 256 )
      ESP_LOGW(FNAME,"Warning serial task stack low: %d bytes", uxTaskGetStackHighWaterMark( pid ) );
    vTaskDelay( HEARTBEAT_PERIOD_MS_SERIAL/portTICK_PERIOD_MS );
	}
}


bool Serial::selfTest(int num){
	HardwareSerial *mySerial;
	if( num == 1 )
		mySerial = &Serial1;
	else if( num == 2 )
		mySerial = &Serial2;
	else
		return false;
	ESP_LOGI(FNAME,"Serial %d selftest", num );
	delay(100);  // wait for serial hardware init
	_selfTest = true;
	std::string test( PROGMEM "The quick brown fox jumps over the lazy dog" );
	int tx = 0;
	if( mySerial->availableForWrite() ) {
		tx = mySerial->write( test.c_str(), test.length() );
		ESP_LOGI(FNAME,"Serial TX written: %d", tx );
	}
	else {
		ESP_LOGI(FNAME,"Serial not avail for sending, abort");
		return false;
	}
	char recv[50];
	memset(recv,0,50);
	int numread = 0;
	for( int i=1; i<10; i++ ){
			int avail = mySerial->available();
			if( avail >= tx ){
				if( avail > tx )
					avail = tx+1;
				numread = mySerial->read( recv, avail );
				ESP_LOGI(FNAME,"Serial RX bytes read: %d %s", numread, recv );
				break;
			}
		delay( 30 );
		ESP_LOGI(FNAME,"Serial bytes avail: %d", numread );
	}
	_selfTest = false;
	std::string r( recv );
	if( r.find( test ) != std::string::npos )  {
		ESP_LOGI(FNAME,"Serial Test PASSED");
		return true;
	}
	else {
		ESP_LOGI(FNAME,"Serial Test FAILED !");
		return false;
	}
	return false;
}

void Serial::begin(){
	ESP_LOGI(FNAME,"Serial::begin()" );
	if( serial1_speed.get() != 0  || wireless != 0 ){
		int baudrate = baud[serial1_speed.get()];
		if( baudrate != 0 ) {
			gpio_pullup_en( GPIO_NUM_16 );
			gpio_pullup_en( GPIO_NUM_17 );
			ESP_LOGI(FNAME,"Serial Interface ttyS1 enabled with serial speed: %d baud: %d tx_inv: %d rx_inv: %d",  serial1_speed.get(), baud[serial1_speed.get()], serial1_tx_inverted.get(), serial1_rx_inverted.get() );
			if( serial1_pins_twisted.get() ){
				if( serial1_tx_enable.get() ){

					Serial1.begin(baudrate,SERIAL_8N1,17,16, serial1_rx_inverted.get(), serial1_tx_inverted.get());   //  IO16: RXD2,  IO17: TXD2
				}else{
					Serial1.begin(baudrate,SERIAL_8N1,17,36, serial1_rx_inverted.get(), serial1_tx_inverted.get());   //  IO16: RXD2,  IO17: TXD2
					gpio_set_direction(GPIO_NUM_16, GPIO_MODE_INPUT);     // 2020 series 1, analog in
					gpio_pullup_dis( GPIO_NUM_16 );
				}
			}
			else{
				if( serial1_tx_enable.get() ){
					Serial1.begin(baudrate,SERIAL_8N1,16,17, serial1_rx_inverted.get(), serial1_tx_inverted.get());   //  IO16: RXD2,  IO17: TXD2
				}else{
					Serial1.begin(baudrate,SERIAL_8N1,16,36, serial1_rx_inverted.get(), serial1_tx_inverted.get());   //  IO16: RXD2,  IO17: TXD2
					gpio_set_direction(GPIO_NUM_17, GPIO_MODE_INPUT);     // 2020 series 1, analog in
					gpio_pullup_dis( GPIO_NUM_17 );
				}
			}
			Serial1.setRxBufferSize(512);
		}
		// need this for bluetooth
	}
	if( serial2_speed.get() != 0  && hardwareRevision.get() >= 3 ){
		ESP_LOGI(FNAME,"Serial Interface ttyS2 enabled with serial speed: %d baud: %d tx_inv: %d rx_inv: %d",  serial2_speed.get(), baud[serial2_speed.get()], serial2_tx_inverted.get(), serial2_rx_inverted.get() );
		if( serial2_pins_twisted.get() ){  //   speed, RX, TX, invRX, invTX
			gpio_pullup_en( GPIO_NUM_4 );
			gpio_pullup_en( GPIO_NUM_18 );
			if( serial2_tx_enable.get() ){
				Serial2.begin(baud[serial2_speed.get()],SERIAL_8N1,4,18, serial2_rx_inverted.get(), serial2_tx_inverted.get());   //  IO4: RXD2,  IO18: TXD2
			}else{
				Serial2.begin(baud[serial2_speed.get()],SERIAL_8N1,4,36, serial2_rx_inverted.get(), serial2_tx_inverted.get());
				gpio_set_direction(GPIO_NUM_18, GPIO_MODE_INPUT);     // 2020 series 1, analog in
				gpio_pullup_dis( GPIO_NUM_18 );
			}
		}
		else{
			if( serial2_tx_enable.get() ){
				Serial2.begin(baud[serial2_speed.get()],SERIAL_8N1,18,4, serial2_rx_inverted.get(), serial2_tx_inverted.get());   //  IO18: RXD2,  IO4: TXD2
			}
			else{
				Serial2.begin(baud[serial2_speed.get()],SERIAL_8N1,18,36, serial2_rx_inverted.get(), serial2_tx_inverted.get());
				gpio_set_direction(GPIO_NUM_4, GPIO_MODE_INPUT);     // 2020 series 1, analog in
				gpio_pullup_dis( GPIO_NUM_4 );
			}
		}
		Serial2.setRxBufferSize(512);
	}
}

void Serial::taskStart(){
	ESP_LOGI(FNAME,"Serial::taskStart()" );
	if( serial1_speed.get() != 0  || wireless != 0 || serial2_speed.get() != 0 ){
		xTaskCreatePinnedToCore(&Serial::serialHandler, "serialHandler", 4096, NULL, 11, pid, 0);  // stay below compass task
	}
	// handler S1 now serves both interfaces in one task
}
