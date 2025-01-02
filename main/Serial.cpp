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
// #include "canbus.h"
#include "ESPAudio.h"
#include "SerialLine.h"


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
EventGroupHandle_t Serial::rxTxNotifier = nullptr;

// Event group bits
#define RX0_CHAR 1
#define RX0_NL 2

#define TX1_REQ 64
#define RX1_CHAR 4
#define RX1_NL 8

#define TX2_REQ 128
#define RX2_CHAR 16
#define RX2_NL 32


xcv_serial_t Serial::_S1 = { .name="S1", .tx_q = &s1_tx_q, .mySL = 0,
		                    .rx_char = RX1_CHAR, .rx_nl = RX1_NL, .tx_req = TX1_REQ,
		                    .monitor =	MON_S1, .pid = 0, .cfg2 = nullptr, .route_disable = true, .dl = &dl_S1, .port = 1
};
xcv_serial_t Serial::_S2 = { .name="S2", .tx_q = &s2_tx_q, .mySL = 0,
		                    .rx_char = RX2_CHAR, .rx_nl = RX2_NL, .tx_req = TX2_REQ,
		                    .monitor=MON_S2, .pid = 0, .cfg2 = nullptr, .route_disable = true, .dl = &dl_S2, .port = 2
};

bool Serial::bincom_mode = false;  // we start with bincom timer inactive

// Serial Handler ttyS1, S1, port 8881
void Serial::serialHandler(void *pvParameters)
{
	char buf[512];  // 6 messages @ 80 byte
	xcv_serial_t *cfg = (xcv_serial_t *)pvParameters;
	ESP_LOGI(FNAME,"serialHandler %s SerialLine* 1:%p 2:%p", cfg->name, S1, S2 );
	if( cfg->port == 1 )
		cfg->mySL=S1;
	else if( cfg->port == 2 )
		cfg->mySL=S2;

	if( rxTxNotifier == nullptr ) {  // only once for both UARTS
		rxTxNotifier = xEventGroupCreate();
		ESP_LOGI( FNAME, "setRxNotifier" );
		uartRxEventHandler( rxTxNotifier );
	}
	cfg->route_disable = false;
	// Define timeout of 5s that the watchdog becomes not active.
	TickType_t ticksToWait = 5000 / portTICK_PERIOD_MS;

	while( true ) {
		// Stack supervision
		if( uxTaskGetStackHighWaterMark( cfg->pid ) < 256 )
			ESP_LOGW(FNAME,"Warning serial %d task stack low: %d bytes", cfg->mySL->number(), uxTaskGetStackHighWaterMark( cfg->pid ) );

		if( _selfTest ) {
			delay( 100 );
			continue;
		}
		if( routingStopped( cfg ) ) {
			// Flarm download of other Serial is running, stop RX processing and empty TX queue.
			cfg->tx_q->clear();
			//cfg->rx_q->clear();
			delay( 1000 );
			continue;
		}
		// Define expected event bits. They can come from the Uart RX ISR or from the Serial TX router queue.
		EventBits_t bitsToWaitFor = cfg->tx_req | cfg->rx_char;
		// ESP_LOGI( FNAME, "%s: Wait for event", cfg->name );
		// We do wait for events from Uart RX, router TX side or timeout
		EventBits_t ebits = xEventGroupWaitBits( rxTxNotifier, bitsToWaitFor, pdTRUE, pdFALSE, ticksToWait );
		if( (ebits & bitsToWaitFor ) == 0 ) {
			// Timeout occurred, that is used to reset the watchdog.
			continue;
		}
		// ESP_LOGI( FNAME, "%s: TO=%dms, EventBits=%X, RXAvail=%d", cfg->name, ticksToWait, ebits, cfg->mySL->available() );
		// TX part, check if there is data for Serial Interface to send
		if( (ebits & cfg->tx_req) && cfg->mySL->availableForWrite() ) {
			// ESP_LOGI(FNAME,"S%d: TX and available TXQ=%d P=%p", cfg->mySL->number(), cfg->tx_q->numElements(), cfg->tx_q );
			size_t len = Router::pullBlock( *(cfg->tx_q), buf, 512 );
			ESP_LOGI(FNAME,"S%d: bytes=%d QL=%d", cfg->mySL->number(), len, cfg->tx_q->numElements() );
			if( len ){
				// ESP_LOGI(FNAME,"S%d: TX len: %d bytes", cfg->mySL->number(), len );
				// ESP_LOG_BUFFER_HEXDUMP(FNAME,buf,len, ESP_LOG_INFO);
				cfg->mySL->write( buf, len );
				if( !bincom_mode )
					DM.monitorString( cfg->monitor, DIR_TX, buf, len );
				// ESP_LOGD(FNAME,"S%d: TX written: %d", cfg->mySL->number(), wr);
			}
		}
		// RX part
		if( ebits & cfg->rx_char ) { // only one transparent mode from now on, frame slicing in upper layer for UBX and NMEA
			uint32_t available = std::min( cfg->mySL->available(), 511 );
			if( available ){
				uint16_t rxBytes = cfg->mySL->readBufFromQueue( (uint8_t*)buf, available );  // read out all characters from the RX queue
				// ESP_LOGI(FNAME,"S%d: RX: %d bytes, avail: %d", cfg->mySL->number(), rxBytes, available );
				// ESP_LOG_BUFFER_HEXDUMP(FNAME,buf, rxBytes, ESP_LOG_INFO);
				buf[rxBytes] = 0;
				cfg->mySL->receive( buf, rxBytes, cfg->port );
				DM.monitorString( cfg->monitor, DIR_RX, buf, rxBytes );
			}
		}
		if( Flarm::bincom ){
			if( !bincom_mode ) {   // we are in bincom mode, stop this mode if Flarm has detected otherwise
				if(cfg->port == Flarm::bincom_port)
					enterBincomMode(cfg);
			}
		}
		else{
			if( bincom_mode ){  // Flarm says we are in textmode, exit serial bincom mode if not yet done
				if(cfg->port == Flarm::bincom_port)
					exitBincomMode(cfg);
			}
		}
	} // end while( true )
}

void Serial::enterBincomMode( xcv_serial_t *cfg ){
	// Stop routing of TX/RX data of a potential other device on the second serial channel needs to be done by user.
	// Both channels may be involved in igc download so we can't automate more here
	delay( 100 );
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

bool Serial::selfTest(int num){
	SerialLine *mySerial;
	if( num == 1 )
		mySerial = S1;
	else if( num == 2 )
		mySerial = S2;
	else
		return false;
	ESP_LOGI(FNAME,"Serial %d selftest", num );
	delay(100);  // wait for serial hardware init
	_selfTest = true;
	std::string test( "The quick brown fox jumps over the lazy dog" );
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
	delay( 30 );
	int numread = 0;
	for( int i=1; i<10; i++ ){
		int avail = mySerial->available();
		ESP_LOGI(FNAME,"Serial RX bytes avail: %d", avail );
		if( avail >= tx ){
			if( avail > 50 )
				avail = 50;
			numread = mySerial->read( (uint8_t*)recv, avail );
			ESP_LOGI(FNAME,"Serial RX bytes read: %d %s", numread, recv );
			break;
		}
		delay( 30 );
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

void Serial::begin(){   // will be obsoleted when Devices launch serial interfaces.
	ESP_LOGI(FNAME,"Serial::begin()" );
	// Initialize static configuration
	_S1.cfg2 = &_S2;
	_S2.cfg2 = &_S1;

	// Create event notifier, when serial 1 or serial 2 are enabled
	// do in any case, S1 and S2 might be disabled

}

bool Serial::taskStarted( int num ){
	bool ret=true;
	switch( num ){
	case UART_NUM_1:
		if( _S1.pid == NULL )
			ret=false;
		break;
	case UART_NUM_2:
		if( _S2.pid == NULL )
			ret=false;
		break;
	default:
		ret=false;
	}
	ESP_LOGI(FNAME,"taskStarted() ret:%d S1.pid:%p S2.pid:%p", ret, _S1.pid, _S2.pid );
	return ret;
}

// will be obsoleted when devices launch serial lines
void Serial::taskStart(){
	ESP_LOGI(FNAME,"Serial::taskStart()" );
	taskStart(1);
	taskStart(2);
	delay(100);
	if( S1 ){
		ESP_LOGI(FNAME,"S1 enable Int" );
		S1->enableRxInterrupt();
	}
	if( S2 ){
		ESP_LOGI(FNAME,"S2 enable Int" );
		S2->enableRxInterrupt();
	}
}

void Serial::taskStart(int uart_nr){
	ESP_LOGI(FNAME,"Serial::taskStart()" );
	bool serial1 = (serial1_speed.get() != 0 || wireless != 0);
	bool serial2 = (serial2_speed.get() != 0 && hardwareRevision.get() >= XCVARIO_21);

	if( serial1 && uart_nr == 1 ){
		taskStartS1();
	}
	if( serial2 && uart_nr == 2 ){
		taskStartS2();
	}
}

void Serial::taskStop( int uart_nr ){
	switch( uart_nr ){
	case UART_NUM_1:
		if( _S1.pid != NULL ){
			vTaskDelete( _S1.pid );
			_S1.pid = NULL;
		}
		break;
	case UART_NUM_2:
		if( _S2.pid != NULL ){
			vTaskDelete( _S2.pid );
			_S2.pid = NULL;
		}
		break;
	}
}

void Serial::taskStartS1(){
	ESP_LOGI(FNAME,"taskStartS1()");
	xTaskCreatePinnedToCore(&serialHandler, "serialHandler1", 4096, &_S1, 13, &_S1.pid, 0);  // stay below canbus
}

void Serial::taskStartS2(){
	ESP_LOGI(FNAME,"taskStartS2()");
	xTaskCreatePinnedToCore(&serialHandler, "serialHandler2", 4096, &_S2, 13, &_S2.pid, 0);  // stay below canbus
}

