/**
 * Serial.cpp
 *
 * 24.12.2021 Axel Pauli: added some RX/TX handling stuff.
 */

#include <esp_log.h>
#include "BTSender.h"
#include <cstring>
#include "sdkconfig.h"
#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include <freertos/semphr.h>
#include <algorithm>
#include <HardwareSerial.h>
#include "RingBufCPP.h"
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
static TaskHandle_t pid1 = nullptr;
static TaskHandle_t pid2 = nullptr;

bool Serial::_selfTest = false;
EventGroupHandle_t Serial::rxTxNotifier;

// Serial Handler ttyS1, S1, port 8881
void Serial::serialHandler1(void *pvParameters)
{
	SString s;
  // Flag that Flarm exit command has been sent.
  bool flarmExitCmd = false;
  // Flarm exit command sequence counter
  uint8_t flarmAckExitSeq[2];

  // Make a pause, that has avoided core dumps during enable interrupt.
  delay( 100 );

  // Clear Uart RX receiver buffer to get a clean start point.
  Serial1.flush( false );

  // Enable Uart RX interrupt
  ESP_LOGI(FNAME,"S1: enable RX by Interrupt" );
  Serial1.enableInterrupt();

	while( true ) {
	  esp_task_wdt_reset();

	  if( uxTaskGetStackHighWaterMark( pid1 ) < 256 )
	    ESP_LOGW(FNAME,"Warning serial 1 task stack low: %d bytes", uxTaskGetStackHighWaterMark( pid1 ) );

		if( _selfTest ) {
		  vTaskDelay( HEARTBEAT_PERIOD_MS_SERIAL / portTICK_PERIOD_MS );
		  continue;
		}

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

    // Define expected event bits. They can come from the Uart RX ISR or from
    // the Serial 1 TX router queue.
    // In Flarm text mode we may get informed only for newlines in the Uart
    // RX queue. We tolerate that also in binary mode to get reset this bit
    // in the event bit mask after a read.
    EventBits_t bitsToWaitFor = TX1_REQ | RX1_NL;

    if( Flarm::bincom ) {
      // In Flarm binary mode, we may get informed for every available
      // character in the RX queue.
      bitsToWaitFor |= RX1_CHAR;
    }

    // Define timeout of 5s that the watchdog becaomes not active.
    TickType_t ticksToWait = 5000 / portTICK_PERIOD_MS;

    // We do wait for events from Uart RX, router TX side or timeout
    EventBits_t ebits = xEventGroupWaitBits( rxTxNotifier,
                                             bitsToWaitFor,
                                             pdTRUE,
                                             pdFALSE,
                                             ticksToWait );

    if( (ebits & bitsToWaitFor ) == 0 ) {
      // Timeout occurred, that is used to reset the watchdog.
      continue;
    }

    ESP_LOGI( FNAME, "S1: EVTO=%dms, bincom=%d, EventBits=%X, RXA=%d, NLC=%d",
              ticksToWait, Flarm::bincom, ebits, Serial1.available(), Serial1.getNlCounter() );

    if( Serial1.stopRouting() ) {
      // Flarm download of other Serial is running, stop RX processing and empty TX queue.
      s1_tx_q.clear();
      continue;
    }

    // Check, if Serial Interface 1 has something to send
    if( ebits & TX1_REQ && Serial1.availableForWrite() ) {
      // ESP_LOGI(FNAME,"S1: TX and available");
      while( Router::pullMsg( s1_tx_q, s ) ) {
        ESP_LOGD(FNAME,"S1: TX len: %d bytes", s.length() );
        // ESP_LOG_BUFFER_HEXDUMP(FNAME,s.c_str(),s.length(), ESP_LOG_DEBUG);
        ESP_LOG_BUFFER_HEXDUMP(FNAME,s.c_str(),s.length(), ESP_LOG_INFO);
        Serial1.write( s.c_str(), s.length() );
        if( ! Flarm::bincom )
          DM.monitorString( MON_S1, DIR_TX, s.c_str() );
        // ESP_LOGD(FNAME,"S1: TX written: %d", wr);

        // Look, if a Flarm exit command has been sent in binary mode.
        if( Flarm::bincom && flarmExitCmd == false ) {
          flarmExitCmd = Serial1.checkFlarmTx( s.c_str(), s.length(), flarmAckExitSeq );
          if( flarmExitCmd ) {
            ESP_LOGI( FNAME, "S1: Flarm Exit Cmd detected" );
          }
        }
      }
    }

    if( ! Flarm::bincom ) {
      // Flarm works in text mode, check if NL is reported.
      if( ebits & RX1_NL ) {
        handleTextMode( 1, flarmExitCmd );
      }
      // wait for the next newline
      continue;
    }

    if( ebits & RX1_CHAR ) {
      // Flarm is in binary mode and we can read bytes.
      // Due to delays we can have more than one character in the RX queue.
      int available = Serial1.available();

      if( available == 0 ) {
          // Nothing contained in RX queue
          continue;
      }

      uint8_t* flarmBuf = (uint8_t *) malloc( available + 1 );
      uint16_t flarmBufFilled = 0;
      bool flarmAckExit = false;

      while( available > 0 ) {
        // read out all characters from the RX queue
        uint8_t c;
        uint8_t res = Serial1.readCharFromQueue( &c );
        available--;

        if( res == 0 ) // Nothing has read, break loop
          break;

        flarmBuf[flarmBufFilled] = c;
        flarmBufFilled++;

        // Check, if Flarm has sent an acknowledge to the exit command.
        if( flarmExitCmd == true ) {
          int start;
          flarmAckExit = Serial1.checkFlarmRx( (const char* ) flarmBuf, flarmBufFilled, flarmAckExitSeq, &start );
          if( flarmAckExit ) {
            ESP_LOGI( FNAME, "S1: Flarm Ack Exit detected" );
            break;
          }
        }
      }

      // ESP_LOGI(FNAME, "S1: Flarm::bincom forward %d RX data", flarmBufFilled );
      s.set( (char *) flarmBuf, flarmBufFilled );
      routeS1RxData( s );

      // Fall back check, if Flarm has self exited from the binary mode. A
      // classic Flarm will never do that but a PowerFlarm after 60s of no
      // traffic.
      const char *pflau = "$PFLAU,";
      if( flarmAckExit == false && flarmBufFilled > strlen(pflau) ) {
          flarmBuf[flarmBufFilled] = '\0';
          if( strstr( (const char *) flarmBuf, pflau) != nullptr ) {
            // we assume a Flarm self switch to text mode.
            flarmAckExit = true;
            ESP_LOGI(FNAME, "S1: $PFLAU found --> Flarm has self switched to text mode" );
          }
      }
      free( flarmBuf );

      // Reset binary mode, if Flarm ACK Exit has received.
      if( flarmAckExit == true ) {
          Flarm::bincom = 0;
          flarmExitCmd = false;
          if( pid2 != nullptr ) {
            ESP_LOGI(FNAME, "S1: Activate S2 after Flarm download." );
            Serial2.flush( false );
            Serial2.enableInterrupt();
          }
          Serial2.setStopRouting( false );
          Serial1.flush( false );
          ESP_LOGI(FNAME, "S1: Flarm Ack Exit received --> switched Flarm to text mode" );
      }
    }
  } // end while( true )
}

/**
 * Handle Serial 1/2 RX data, if Flarm works in text mode.
 */
void Serial::handleTextMode( uint8_t uartNum, bool &flarmExitCmd ) {
  HardwareSerial *u1;
  HardwareSerial *u2;

  if( uartNum == 1 ) {
    u1 = &Serial1;
    u2 = &Serial2;
  }
  else if( uartNum == 2 ) {
    u1 = &Serial2;
    u2 = &Serial1;
  }
  else
    return;

  // Check available NLs in the receiver buffer.
  uint16_t nlc = u1->getNlCounter();

  if( nlc == 0 )
    return;

  // get byte count from RX queue
  int bc = u1->available();

  if( bc == 0 ) {
    // should normally not happens, clear RX buffer for a new synchronization.
    u1->flush( false );
    return;
  }

  uint8_t *rxbuf = (uint8_t *) malloc( bc );
  SString s;

  while( nlc > 0 ) {
    size_t bytes = u1->readLineFromQueue( rxbuf, bc );
    nlc--;
    u1->decNlCounter();

    if( bytes > 0 ) {
      s.set( (char *) rxbuf, bytes );

      const char* pflax = "$PFLAX,A*2E\r\n";
      // check Flarm response to $PFLAX, if it is ok. If yes, switch
      // to binary mode.
      if( strcmp( s.c_str(), pflax ) == 0 ) {
        u1->clearFlarmTx();
        u1->clearFlarmRx();
        flarmExitCmd = false;
        // Stop routing of TX/RX data in other Serial
        u2->setStopRouting( true );
        u2->disableInterrupt();
        Flarm::bincom = 5;
        // Wait so long until second RX queue is empty.
        delay( 10 );
        if( uartNum == 1 )
          while( ! s2_rx_q.isEmpty() ) delay( 10 );
        else
          while( ! s1_rx_q.isEmpty() ) delay( 10 );
        ESP_LOGI(FNAME, "S%d: $PFLAX,A*2E --> switching to binary mode", uartNum );
      }
      if( uartNum == 1 )
        routeS1RxData( s );
      else
        routeS2RxData( s );
    }
  }
  free( rxbuf );
}

void Serial::routeS1RxData( SString& s ) {
  if( Serial1.stopRouting() == true )
    return;

  Router::forwardMsg( s, s1_rx_q );
  Router::routeS1();
  if( ! Flarm::bincom )
    DM.monitorString( MON_S1, DIR_RX, s.c_str(), Flarm::bincom );
}

////////////////////////////////////////////////////////////////////////////////
// Serial Handler  ttyS2, S2, port 8882
void Serial::serialHandler2(void *pvParameters)
{
  SString s;
  // Flag that Flarm exit command has been sent.
  bool flarmExitCmd = false;
  // Flarm exit command sequence counter
  uint8_t flarmAckExitSeq[2];

  // Make a pause, that has avoided core dumps during enable interrupt.
  delay( 100 );

  // Clear Uart RX receiver buffer to get a clean start point.
  Serial2.flush( false );

  // Enable Uart RX interrupt
  ESP_LOGI(FNAME,"S2: enable RX by Interrupt" );
  Serial2.enableInterrupt();

  while( true ) {
    esp_task_wdt_reset();

    if( uxTaskGetStackHighWaterMark( pid1 ) < 256 )
      ESP_LOGW(FNAME,"Warning serial 2 task stack low: %d bytes", uxTaskGetStackHighWaterMark( pid2 ) );

    if( _selfTest ) {
      vTaskDelay( HEARTBEAT_PERIOD_MS_SERIAL / portTICK_PERIOD_MS );
      continue;
    }

    // Define expected event bits. They can come from the Uart RX ISR or from
    // the Serial 2 TX router queue.
    // In Flarm text mode we may get informed only for newlines in the Uart
    // RX queue. We tolerate that also in binary mode to get reset this bit
    // in the event bit mask after a read.
    EventBits_t bitsToWaitFor = TX2_REQ | RX2_NL;

    if( Flarm::bincom ) {
      // In Flarm binary mode, we may get informed for every available
      // character in the RX queue.
      bitsToWaitFor |= RX2_CHAR;
    }

    // Define timeout of 5s that the watchdog becaomes not active.
    TickType_t ticksToWait = 5000 / portTICK_PERIOD_MS;

    // We do wait for events from Uart RX, router TX side or timeout
    EventBits_t ebits = xEventGroupWaitBits( rxTxNotifier,
                                             bitsToWaitFor,
                                             pdTRUE,
                                             pdFALSE,
                                             ticksToWait );

    if( (ebits & bitsToWaitFor ) == 0 ) {
      // Timeout occurred, that is used to reset the watchdog.
      continue;
    }

    ESP_LOGI( FNAME, "S2: EVTO=%dms, bincom=%d, EventBits=%X, RXA=%d, NLC=%d",
              ticksToWait, Flarm::bincom, ebits, Serial2.available(), Serial2.getNlCounter() );

    if( Serial2.stopRouting() ) {
      // Flarm download of other Serial is running, stop RX processing and empty TX queue.
      s2_tx_q.clear();
      continue;
    }

    // Check, if Serial Interface 2 has something to send
    if( ebits & TX2_REQ && Serial2.availableForWrite() ) {
      // ESP_LOGI(FNAME,"S2: TX and available");
      while( Router::pullMsg( s2_tx_q, s ) ) {
        ESP_LOGD(FNAME,"S2: TX len: %d bytes", s.length() );
        // ESP_LOG_BUFFER_HEXDUMP(FNAME,s.c_str(),s.length(), ESP_LOG_DEBUG);
        ESP_LOG_BUFFER_HEXDUMP(FNAME,s.c_str(),s.length(), ESP_LOG_INFO);
        Serial2.write( s.c_str(), s.length() );
        if( ! Flarm::bincom )
          DM.monitorString( MON_S2, DIR_TX, s.c_str() );
        // ESP_LOGD(FNAME,"S2: TX written: %d", wr);

        // Look, if a Flarm exit command has been sent in binary mode.
        if( Flarm::bincom && flarmExitCmd == false ) {
          flarmExitCmd = Serial2.checkFlarmTx( s.c_str(), s.length(), flarmAckExitSeq );
          if( flarmExitCmd ) {
            ESP_LOGI( FNAME, "S2: Flarm Exit Cmd detected" );
          }
        }
      }
    }

    if( ! Flarm::bincom ) {
      // Flarm works in text mode, check if NL is reported.
      if( ebits & RX2_NL ) {
        handleTextMode( 2, flarmExitCmd );
      }
      // wait for the next newline
      continue;
    }
    if( ebits & RX2_CHAR ) {
      // Flarm is in binary mode and we can read bytes.
      // Due to delays we can have more than one character in the RX queue.
      int available = Serial2.available();

      if( available == 0 ) {
          // Nothing contained in RX queue
          continue;
      }

      uint8_t* flarmBuf = (uint8_t *) malloc( available + 1 );
      uint16_t flarmBufFilled = 0;
      bool flarmAckExit = false;

      while( available > 0 ) {
        // read out all characters from the RX queue
        uint8_t c;
        uint8_t res = Serial2.readCharFromQueue( &c );
        available--;

        if( res == 0 ) // Nothing has read, break loop
          break;

        flarmBuf[flarmBufFilled] = c;
        flarmBufFilled++;

        // Check, if Flarm has sent an acknowledge to the exit command.
        if( flarmExitCmd == true ) {
          int start;
          flarmAckExit = Serial2.checkFlarmRx( (const char* ) flarmBuf, flarmBufFilled, flarmAckExitSeq, &start );
          if( flarmAckExit ) {
            ESP_LOGI( FNAME, "S2: Flarm Ack Exit detected" );
            break;
          }
        }
      }

      // ESP_LOGI(FNAME, "S2: Flarm::bincom forward %d RX data", flarmBufFilled );
      s.set( (char *) flarmBuf, flarmBufFilled );
      routeS2RxData( s );

      // Fall back check, if Flarm has self exited from the binary mode. A
      // classic Flarm will never do that but a PowerFlarm after 60s of no
      // traffic.
      const char *pflau = "$PFLAU,";
      if( flarmAckExit == false && flarmBufFilled > strlen(pflau) ) {
          flarmBuf[flarmBufFilled] = '\0';
          if( strstr( (const char *) flarmBuf, pflau) != nullptr ) {
            // we assume a Flarm self switch to text mode.
            flarmAckExit = true;
            ESP_LOGI(FNAME, "S2: $PFLAU found --> Flarm has self switched to text mode" );
          }
      }
      free( flarmBuf );

      // Reset binary mode, if Flarm ACK Exit has received.
      if( flarmAckExit == true ) {
          Flarm::bincom = 0;
          flarmExitCmd = false;
           if( pid1 != nullptr ) {
             ESP_LOGI(FNAME, "S2: Activate S1 after Flarm download." );
            Serial1.flush( false );
            Serial1.enableInterrupt();
          }
          Serial1.setStopRouting( false );
          Serial2.flush( false );
          ESP_LOGI(FNAME, "S2: Flarm Ack Exit received --> switched Flarm to text mode" );
      }
    }
  } // end while( true )
}

void Serial::routeS2RxData( SString& s ) {
  if( Serial2.stopRouting() == true )
    return;

  Router::forwardMsg( s, s2_rx_q );
  Router::routeS2();
  if( ! Flarm::bincom )
    DM.monitorString( MON_S2, DIR_RX, s.c_str() );
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

	bool serial1 = (serial1_speed.get() != 0 || wireless != 0);
	bool serial2 = (serial2_speed.get() != 0 && hardwareRevision.get() >= 3);

	if( serial1 || serial2 ) {
    // Create event notifier, when serial 1 or serial 2 are enabled
    rxTxNotifier = xEventGroupCreate();

    if( rxTxNotifier == nullptr ) {
      ESP_LOGI( FNAME, "Cannot create EventGroupHandle" );
    }
    else {
      // Set event notifier in uart interrupt routine.
      HardwareSerial::setRxNotifier( rxTxNotifier );
    }
	}

	if( serial1 ){
		xTaskCreatePinnedToCore(&serialHandler1, "serialHandler1", 4096, NULL, 11, &pid1, 0);  // stay below compass task
	}
	if( serial2 ){
		xTaskCreatePinnedToCore(&serialHandler2, "serialHandler2", 4096, NULL, 10, &pid2, 0);  // stay below compass task and task for S1
	}
}
