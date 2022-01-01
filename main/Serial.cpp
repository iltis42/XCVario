/**
 * Serial.cpp
 *
 * 01.01.2022 Axel Pauli: updates after first delivery.
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

#define HEARTBEAT_PERIOD_MS_SERIAL 20

bool Serial::_selfTest = false;
EventGroupHandle_t Serial::rxTxNotifier;

// Event group bits
#define RX0_CHAR 1
#define RX0_NL 2

#define TX1_REQ 64
#define RX1_CHAR 4
#define RX1_NL 8

#define TX2_REQ 128
#define RX2_CHAR 16
#define RX2_NL 32

static xcv_serial_t S1 = { .name="S1", .tx_q = &s1_tx_q, .rx_q = &s1_rx_q, .route=Router::routeS1, .uart=&Serial1, .uart_nr = 1, .rx_char = RX1_CHAR, .rx_nl = RX1_NL, .tx_req = TX1_REQ, .monitor=MON_S1, .pid = 0 };
static xcv_serial_t S2 = { .name="S2", .tx_q = &s2_tx_q, .rx_q = &s2_rx_q, .route=Router::routeS2, .uart=&Serial2, .uart_nr = 2, .rx_char = RX2_CHAR, .rx_nl = RX2_NL, .tx_req = TX2_REQ, .monitor=MON_S2, .pid = 0 };

bool Serial::_stopRouting[3] = { false, false, false };

// Serial Handler ttyS1, S1, port 8881
void Serial::serialHandler(void *pvParameters)
{
	xcv_serial_t *cfg = (xcv_serial_t *)pvParameters;
	SString s;
	// Flag that Flarm exit command has been sent.
	bool flarmExitCmd = false;
	// Flarm exit command sequence counter
	uint8_t flarmAckExitSeq[2];

  uint8_t flarmTx[9];
  uint8_t flarmRx[11];

	// Make a pause, that has avoided core dumps during enable interrupt.
	delay( 100 );

	// Clear Uart RX receiver buffer to get a clean start point.
	cfg->uart->flush( false );

	// Enable Uart RX interrupt
	ESP_LOGI(FNAME,"%s: enable RX by Interrupt", cfg->name );
	cfg->uart->enableRxInterrupt();

	while( true ) {
		esp_task_wdt_reset();

		if( uxTaskGetStackHighWaterMark( cfg->pid ) < 256 )
			ESP_LOGW(FNAME,"Warning serial %d task stack low: %d bytes", cfg->uart_nr, uxTaskGetStackHighWaterMark( cfg->pid ) );

		if( _selfTest ) {
			vTaskDelay( HEARTBEAT_PERIOD_MS_SERIAL / portTICK_PERIOD_MS );
			continue;
		}
		// Define expected event bits. They can come from the Uart RX ISR or from
		// the Serial 1 TX router queue.
		// In Flarm text mode we may get informed only for newlines in the Uart
		// RX queue. We tolerate that also in binary mode to get reset this bit
		// in the event bit mask after a read.
		EventBits_t bitsToWaitFor = cfg->tx_req | cfg->rx_nl;

		if( Flarm::bincom ) {
			// In Flarm binary mode, we may get informed for every available
			// character in the RX queue.
			bitsToWaitFor |= cfg->rx_char;
		}
		// Define timeout of 5s that the watchdog becaomes not active.
		TickType_t ticksToWait = 5000 / portTICK_PERIOD_MS;

		// We do wait for events from Uart RX, router TX side or timeout
		EventBits_t ebits = xEventGroupWaitBits( rxTxNotifier, bitsToWaitFor, pdTRUE, pdFALSE, ticksToWait );

		if( (ebits & bitsToWaitFor ) == 0 ) {
			// Timeout occurred, that is used to reset the watchdog.
			continue;
		}

#if 1
		ESP_LOGI( FNAME, "%s: EVTO=%dms, bincom=%d, EventBits=%X, RXA=%d, NLC=%d",
				cfg->name, ticksToWait, Flarm::bincom, ebits, cfg->uart->available(), cfg->uart->getNlCounter() );
#endif
		if( stopRouting( cfg->uart_nr ) ) {
			// Flarm download of other Serial is running, stop RX processing and empty TX queue.
			cfg->tx_q->clear();
			continue;
		}

		// Check, if Serial Interface 1 has something to send
		if( ebits & TX1_REQ && cfg->uart->availableForWrite() ) {
			// ESP_LOGI(FNAME,"S%d: TX and available", cfg->uart_nr );
			while( Router::pullMsg( *(cfg->tx_q), s ) ) {
				// ESP_LOGD(FNAME,"S%d: TX len: %d bytes", cfg->uart_nr, s.length() );
				ESP_LOGI(FNAME,"S%d: TX len: %d bytes", cfg->uart_nr, s.length() );
				ESP_LOG_BUFFER_HEXDUMP(FNAME,s.c_str(),s.length(), ESP_LOG_INFO);
				cfg->uart->write( s.c_str(), s.length() );
				if( ! Flarm::bincom )
					DM.monitorString( cfg->monitor, DIR_TX, s.c_str() );
				// ESP_LOGD(FNAME,"S%d: TX written: %d", cfg->uart_nr, wr);

				// Look, if a Flarm exit command has been sent in binary mode.
				if( Flarm::bincom && flarmExitCmd == false ) {
					flarmExitCmd = Flarm::checkFlarmTx( flarmTx, s.c_str(), s.length(), flarmAckExitSeq );
					if( flarmExitCmd ) {
						ESP_LOGI( FNAME, "%s: Flarm Exit Cmd detected", cfg->name );
					}
				}
			}
		}

		if( ! Flarm::bincom ) {
			// Flarm works in text mode, check if NL is reported.
			if( ebits & cfg->rx_nl ) {
				handleTextMode( cfg );
	      if( Flarm::bincom ) {
	        // There was a switch to Flarm's binary mode
	        Flarm::clearFlarmTx( flarmTx );
	        Flarm::clearFlarmRx( flarmRx );
	        flarmExitCmd = false;
	      }
			}
			// wait for the next newline
			continue;
		}

		if( ebits & cfg->rx_char ) {
			// Flarm is in binary mode and we can read bytes.
			// Due to delays we can have more than one character in the RX queue.
			int available = cfg->uart->available();

			if( available == 0 ) {
				// Nothing contained in RX queue
				continue;
			}

			uint8_t* flarmBuf = (uint8_t *) malloc( available + 1 );
			bool flarmAckExit = false;

			// read out all characters from the RX queue
			uint16_t flarmBufFilled = cfg->uart->readBufFromQueue( flarmBuf, available );
			flarmBuf[flarmBufFilled] = 0;
			ESP_LOGI( FNAME, "%s RX bincom, available %d bytes, flarmBufFilled: %d bytes", cfg->name, available, flarmBufFilled  );
			ESP_LOG_BUFFER_HEXDUMP(FNAME,flarmBuf,flarmBufFilled, ESP_LOG_INFO);

			// Check, if Flarm has sent an acknowledge to the exit command.
			if( flarmExitCmd == true ) {   // TBD, move FLARM exit handling to upper layer
				int start;
				flarmAckExit = Flarm::checkFlarmRx( flarmRx, flarmBuf, flarmBufFilled, flarmAckExitSeq, &start );
				if( flarmAckExit )
					ESP_LOGI( FNAME, "%s: Flarm Ack Exit detected", cfg->name );
			}

			ESP_LOGI(FNAME, "%s: Flarm::bincom forward %d RX data", cfg->name, flarmBufFilled );
			s.set( (char *) flarmBuf, flarmBufFilled );
			routeRxData( s, cfg );
			ESP_LOG_BUFFER_HEXDUMP(FNAME,s.c_str(),s.length(), ESP_LOG_INFO);
			// Fall back check, if Flarm has self exited from the binary mode. A
			// classic Flarm will never do that but a PowerFlarm after 60s of no
			// traffic.
			const char *pflau = "$PFLAU,";  // tbd. move to appl layer
			if( flarmAckExit == false && flarmBufFilled > strlen(pflau) ) {  //TBD, move exit handling to upper layer, its not so time critical
				flarmBuf[flarmBufFilled] = '\0';
				if( strstr( (const char *) flarmBuf, pflau) != nullptr ) {
					// we assume a Flarm self switch to text mode.
					flarmAckExit = true;
					ESP_LOGI(FNAME, "%s: $PFLAU found --> Flarm has self switched to text mode", cfg->name );
				}
			}
			free( flarmBuf );

			// Reset binary mode, if Flarm ACK Exit has received.
			// TBD if binmode was with S2, reset must be S1
			if( flarmAckExit == true ) {   // TBD move to upper layer
				Flarm::bincom = 0;
				flarmExitCmd = false;
				if( S2.pid != nullptr ) {
					ESP_LOGI(FNAME, "%s: Activate S2 after Flarm download end.", cfg->name );
					Serial2.flush( false );
					Serial2.enableRxInterrupt();
				}
				setStopRouting( cfg->uart_nr, false );
				cfg->uart->flush( false );
				ESP_LOGI(FNAME, "%s: Flarm Ack Exit received --> switched Flarm to text mode", cfg->name );
			}
		}
	} // end while( true )
}

/**
 * Handle Serial 1/2 RX data, if Flarm works in text mode.
 */
void Serial::handleTextMode( const xcv_serial_t *cfg ) {
	HardwareSerial *u1 = 0;
	HardwareSerial *u2 = 0;
	uint8_t u2_nr = 2;

	if( cfg->uart_nr == 1 ) {   // TBD, more generic way to select other interface, maybe just add second interface to cfg ?
		u1 = S1.uart;
		u2 = S2.uart;
	}
	else if( cfg->uart_nr == 2 ) { // dito
		u1 = S2.uart;
		u2 = S1.uart;
		u2_nr = 1;
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
	while( nlc > 0 ) {
		size_t bytes = u1->readLineFromQueue( rxbuf, bc );
		nlc--;
		u1->decNlCounter();

		if( bytes > 0 ) {
			SString s;
			s.set( (char *) rxbuf, bytes );
      // check Flarm response to $PFLAX, if it is ok. If yes, switch to binary mode.
			Flarm::parsePFLAX( s );   // TBD: better we move this to upper layer, upper layer knows when there is no bincom
			                          // Another idea to make things easy: Just end download mode by a restart as like FLARM does.
			                          // AP: Only a ClassicFlarm makes a restart not a PowerFlarm!!!
			if( Flarm::bincom ) {
				// Stop routing of TX/RX data of other Serial channel
				setStopRouting( u2_nr, true );
				u2->disableInterrupt();
				// Wait so long until second RX queue is empty.
				delay( 10 );
				if( cfg->uart_nr == 1 )
					while( ! S2.rx_q->isEmpty() ) delay( 10 );
				else
					while( ! S1.rx_q->isEmpty() ) delay( 10 );
				ESP_LOGI(FNAME, "%s: $PFLAX,A*2E --> switching to binary mode", cfg->name );
			}
			routeRxData( s, cfg );
		}
	}
	free( rxbuf );
}

void Serial::routeRxData( SString& s, const xcv_serial_t *cfg ) {
	if( stopRouting( cfg->uart_nr ) == true )
		return;
	Router::forwardMsg( s, *(cfg->rx_q) );
	cfg->route();
	if( !Flarm::bincom ){
		DM.monitorString( cfg->monitor, DIR_RX, s.c_str(), Flarm::bincom );
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
	  clearStopRouting();
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
		xTaskCreatePinnedToCore(&serialHandler, "serialHandler1", 4096, &S1, 11, &S1.pid, 0);  // stay below compass task
	}
	if( serial2 ){
		xTaskCreatePinnedToCore(&serialHandler, "serialHandler2", 4096, &S2, 10, &S2.pid, 0);  // stay below compass task and task for S1
	}
}
