/**
 * Serial.cpp
 *
 * 02.01.2022 Axel Pauli: handling of 2 uart channels in one method accomplished.
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
#include "ESPAudio.h"

/* Note that the standard NMEA 0183 baud rate is only 4.8 kBaud.
Nevertheless, a lot of NMEA-compatible devices can properly work with
higher transmission speeds, especially at 9.6 and 19.2 kBaud.
As any sentence can consist of 82 characters maximum with 10 bit each (including start and stop bit),
any sentence might take up to 171 ms (at 4.8k Baud), 85 ms (at 9.6 kBaud) or 43 ms (at 19.2 kBaud).
This limits the overall channel capacity to 5 sentences per second (at 4.8k Baud), 11 msg/s (at 9.6 kBaud) or 23 msg/s (at 19.2 kBaud).
If  too  many  sentences  are  produced  with  regard  to  the  available  transmission  speed,
some sentences might be lost or truncated.
 */


bool Serial::_selfTest = false;
EventGroupHandle_t Serial::rxTxNotifier = 0;

// Event group bits
#define RX0_CHAR 1
#define RX0_NL 2

#define TX1_REQ 64
#define RX1_CHAR 4
#define RX1_NL 8

#define TX2_REQ 128
#define RX2_CHAR 16
#define RX2_NL 32

xcv_serial_t Serial::S1 = { .name="S1", .tx_q = &s1_tx_q, .rx_q = &s1_rx_q, .route=Router::routeS1, .uart=&Serial1,
		                    .rx_char = RX1_CHAR, .rx_nl = RX1_NL, .tx_req = TX1_REQ,
		                    .monitor=MON_S1, .pid = 0, .cfg2 = nullptr, .route_disable = true
};
xcv_serial_t Serial::S2 = { .name="S2", .tx_q = &s2_tx_q, .rx_q = &s2_rx_q, .route=Router::routeS2, .uart=&Serial2,
		                    .rx_char = RX2_CHAR, .rx_nl = RX2_NL, .tx_req = TX2_REQ,
		                    .monitor=MON_S2, .pid = 0, .cfg2 = nullptr, .route_disable = true
};

bool Serial::bincom_mode = false;  // we start with bincom timer inactive

// Serial Handler ttyS1, S1, port 8881
void Serial::serialHandler(void *pvParameters)
{
	SString s;
	xcv_serial_t *cfg = (xcv_serial_t *)pvParameters;
	// Make a pause, that has avoided core dumps during enable the RX interrupt.
	delay( 8000 );                   // delay a bit serial task startup unit startup of system is through
	cfg->uart->flush( false );       // Clear Uart RX receiver buffer to get a clean start point.
	ESP_LOGI(FNAME,"%s: enable RX by Interrupt", cfg->name );
	cfg->uart->enableRxInterrupt();  // Enable Uart RX interrupt
	cfg->route_disable = false;
	// Define timeout of 5s that the watchdog becomes not active.
	TickType_t ticksToWait = 5000 / portTICK_PERIOD_MS;

	while( true ) {
		// Stack supervision
		if( uxTaskGetStackHighWaterMark( cfg->pid ) < 256 )
			ESP_LOGW(FNAME,"Warning serial %d task stack low: %d bytes", cfg->uart->number(), uxTaskGetStackHighWaterMark( cfg->pid ) );

		if( _selfTest ) {
			delay( 100 );
			continue;
		}
		if( routingStopped( cfg ) ) {
			// Flarm download of other Serial is running, stop RX processing and empty TX queue.
			cfg->tx_q->clear();
			delay( 1000 );
			continue;
		}
		// Define expected event bits. They can come from the Uart RX ISR or from
		// the Serial TX router queue.
		// In Flarm text mode we may get informed only for newlines in the Uart
		// RX queue. We tolerate that also in binary mode to get reset this bit
		// in the event bit mask after a read.
		EventBits_t bitsToWaitFor = cfg->tx_req | cfg->rx_nl;
		if( Flarm::bincom ) {
			// In Flarm binary mode, we may get informed for every available
			// character in the RX queue.
			bitsToWaitFor |= cfg->rx_char;
		}
		// We do wait for events from Uart RX, router TX side or timeout
		EventBits_t ebits = xEventGroupWaitBits( rxTxNotifier, bitsToWaitFor, pdTRUE, pdFALSE, ticksToWait );
		if( (ebits & bitsToWaitFor ) == 0 ) {
			// Timeout occurred, that is used to reset the watchdog.
			continue;
		}
#if 0
		ESP_LOGI( FNAME, "%s: EVTO=%dms, bincom=%d, EventBits=%X, RXA=%d, NLC=%d",
				cfg->name, ticksToWait, Flarm::bincom, ebits, cfg->uart->available(), cfg->uart->getNlCounter() );
#endif

		// TX part, check if there is data for Serial Interface to send
		if( ebits & cfg->tx_req && cfg->uart->availableForWrite() ) {
			// ESP_LOGI(FNAME,"S%d: TX and available", cfg->uart->number() );
			while( Router::pullMsg( *(cfg->tx_q), s ) ) {
				// ESP_LOGD(FNAME,"S%d: TX len: %d bytes", cfg->uart->number(), tx.length() );
				// ESP_LOG_BUFFER_HEXDUMP(FNAME,tx.c_str(),tx.length(), ESP_LOG_INFO);
				cfg->uart->write( s.c_str(),s.length() );
				if( !bincom_mode )
					DM.monitorString( cfg->monitor, DIR_TX, s.c_str() );
				// ESP_LOGD(FNAME,"S%d: TX written: %d", cfg->uart->number(), wr);
			}
		}
		// RX part
		// TBD: Unify bincom mode along with parseNMEA and binary UBX message parsing in one state machine
		//      Current solution is resource intensive (mainly CPU), as we got 3 places where frames are
		//      dissected analyzed.
		if( Flarm::bincom ){
			if( !bincom_mode ) {   // we are in bincom mode, stop this mode if Flarm has detected otherwise
				enterBincomMode(cfg);
			}
			if( ebits & cfg->rx_char ) {
				// Flarm is in binary mode and we can read bytes.
				// AP Note: The Uart driver can read up to 112 bytes in its queue, if no pause
			  // is in its input stream before sending an event. If a pause of 2 characters
			  // is in the input stream we will also get an event.
			  // Therefore the number of characters in the RX queue can be different.
				int available = cfg->uart->available();
				if( available ){
					uint8_t* rxBuf = (uint8_t *) malloc( available );
					uint16_t rxBytes = cfg->uart->readBufFromQueue( rxBuf, available );  // read out all characters from the RX queue
					s.set( (char *)rxBuf, rxBytes );
					routeRxData( s, cfg );
					free( rxBuf );
				}
			}
		}
		else{ // Flarm works in text mode, check if NL is reported.
			if( bincom_mode ){  // Flarm says we are in textmode, exit serial bincom mode if not yet done
				exitBincomMode(cfg);
			}
			if( ebits & cfg->rx_nl ) { // wait for the next newline
				while( cfg->uart->getNlCounter() > 1 ) {  // TBD: reading down to zero obviously not possible with the current driver below, clarifiy why we ran into core trying this.
				    cfg->uart->decNlCounter();            // AP: The decrement of the NL counter was missing here. Therefore an endless loop has caused the core dumps!
				    uint8_t rxbuf[128];                   // NMEA size is limited to 80 bytes, 128 is far enough.
					                                      // TDB: available() from driver returns size of whole byte queue containing multiple newlines, so malloc based on this was allocating far to much
				                                        // AP: That is right but afterwards all was freed again. The maximum NL counter what I saw in my tests was 2.
					                                      //      May need better concept here, e.g. place frames in Queue of 128 byte length, or pointer to allocated memory
					                                      //      Alternatively, text mode may vanish here when generic state machine may handle all frame types.
				                                        // Advantage of NL counting is, it is done by the interrupt routine during character read and not again later on in other code parts.
					size_t bytes = cfg->uart->readLineFromQueue( rxbuf, sizeof( rxbuf ) );  // read out until NL and decrement NL count
					s.set( (char *) rxbuf, bytes );
					// ESP_LOGI( FNAME, "%s RX, available: %d bytes, read: %d, postNLC: %d", cfg->name, bc, bytes, cfg->uart->getNlCounter() );
					// ESP_LOG_BUFFER_HEXDUMP(FNAME,rxbuf,bytes, ESP_LOG_INFO);
					routeRxData( s, cfg );
				}
			}
		}
	} // end while( true )
}

void Serial::enterBincomMode( xcv_serial_t *cfg ){
	// Stop routing of TX/RX data of other Serial channel, TDB: check if we did not forget what else to stop ?
	setroutingStopped( cfg->cfg2, true );
	// Stop RX interrupt of other serial channel
	cfg->cfg2->uart->disableInterrupt();
	// Wait so long until BT TX queue and second RX queue are empty.
	while( ! cfg->cfg2->rx_q->isEmpty() ) delay( 5 );
	while( ! bt_tx_q.isEmpty() ) delay( 5 );
	bincom_mode = true;
	ESP_LOGI(FNAME, "%s: --> Switching to binary mode", cfg->name );
}

void Serial::exitBincomMode( xcv_serial_t *cfg ){
	ESP_LOGI(FNAME, "%s: --> Leaving binary mode", cfg->name );
	// just reset device will heal any issue after download...
	Audio::shutdown();
	delay(100);
	esp_restart();
}

void Serial::routeRxData( SString& s, xcv_serial_t *cfg ) {
	if( routingStopped( cfg ) == true ){
		ESP_LOGI(FNAME,"routing is stopped for %s", cfg->name );
		return;
	}
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
	// Initialize static configuration
	S1.cfg2 = &S2;
	S2.cfg2 = &S1;

	// Create event notifier, when serial 1 or serial 2 are enabled
	// do in any case, S1 and S2 might be disabled
	rxTxNotifier = xEventGroupCreate();

	if( rxTxNotifier == nullptr ) {
		ESP_LOGI( FNAME, "Cannot create EventGroupHandle" );
	}
	else {
		// Set event notifier in uart interrupt routine.
		HardwareSerial::setRxNotifier( rxTxNotifier );
	}

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

	if( serial1 ){
		xTaskCreatePinnedToCore(&serialHandler, "serialHandler1", 4096, &S1, 11, &S1.pid, 0);  // stay below compass task
	}
	if( serial2 ){
		xTaskCreatePinnedToCore(&serialHandler, "serialHandler2", 4096, &S2, 10, &S2.pid, 0);  // stay below compass task and task for S1
	}
}
