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

xcv_serial_t Serial::S[] = 
{
 {  
  .name="S1", 
  .tx_q = &s1_tx_q, 
  .mySL = 0,
  .rx_char = RX1_CHAR, 
  .rx_nl = RX1_NL, 
  .tx_req = TX1_REQ,
  .monitor =	MON_S1, 
  .cfg2 = nullptr, 
  .route_disable = true, 
  .dl = &dl_S1, 
  .port = 1 },
  {  
   .name="S2", 
   .tx_q = &s2_tx_q, 
   .mySL = 0,
   .rx_char = RX2_CHAR, 
   .rx_nl = RX2_NL, 
   .tx_req = TX2_REQ,
   .monitor=MON_S2, .cfg2 = nullptr, 
   .route_disable = true, 
   .dl = &dl_S2, 
   .port = 2 } 
};

bool Serial::bincom_mode = false;  // we start with bincom timer inactive
TaskHandle_t Serial::pid = NULL;

void Serial::serialHandler(void *pvParameters)
{
    char buf[512];  // 6 messages @ 80 byte
    xcv_serial_t *cfg1 = &S[0];
    xcv_serial_t *cfg2 = &S[1];
    
    ESP_LOGI(FNAME, "serialHandler Handling both UARTs S1: %p, S2: %p", S1, S2);

    // Initialize event group if not already done
    if (rxTxNotifier == nullptr) {
        rxTxNotifier = xEventGroupCreate();
        ESP_LOGI(FNAME, "setRxNotifier");
        uartRxEventHandler(rxTxNotifier);
    }

    // Define timeout of 5s that the watchdog becomes not active.
    TickType_t ticksToWait = 5000 / portTICK_PERIOD_MS;

    while (true) {
        // Stack supervision
        if (uxTaskGetStackHighWaterMark(pid) < 256) {
            ESP_LOGW(FNAME, "Warning serial task stack low: %d bytes", uxTaskGetStackHighWaterMark(pid));
        }

        if (_selfTest) {
            delay(100);
            continue;
        }

        if (routingStopped(cfg1)) {
            // Flarm download of other Serial is running, stop RX processing and empty TX queue.
            cfg1->tx_q->clear();
            delay(1000);
            continue;
        }

        // Define expected event bits for both UARTs
        EventBits_t bitsToWaitFor = cfg1->tx_req | cfg1->rx_char | cfg2->tx_req | cfg2->rx_char;

        // Wait for events from either UART or timeout
        EventBits_t ebits = xEventGroupWaitBits(rxTxNotifier, bitsToWaitFor, pdTRUE, pdFALSE, ticksToWait);
        if ((ebits & bitsToWaitFor) == 0) {
            // Timeout occurred, reset watchdog
            continue;
        }

        // Handle TX for UART 1 (S1)
        if ((ebits & cfg1->tx_req) && cfg1->mySL->availableForWrite()) {
            size_t len = Router::pullBlock(*(cfg1->tx_q), buf, 512);
            if (len) {
                cfg1->mySL->write(buf, len);
                if (!bincom_mode)
                    DM.monitorString(cfg1->monitor, DIR_TX, buf, len);
            }
        }

        // Handle RX for UART 1 (S1)
        if (ebits & cfg1->rx_char) {
            uint32_t available = std::min(cfg1->mySL->available(), 511);
            if (available) {
                uint16_t rxBytes = cfg1->mySL->readBufFromQueue((uint8_t*)buf, available);
                buf[rxBytes] = 0;
                cfg1->mySL->receive(buf, rxBytes);
                DM.monitorString(cfg1->monitor, DIR_RX, buf, rxBytes);
            }
        }

        // Handle TX for UART 2 (S2)
        if ((ebits & cfg2->tx_req) && cfg2->mySL->availableForWrite()) {
            size_t len = Router::pullBlock(*(cfg2->tx_q), buf, 512);
            if (len) {
                cfg2->mySL->write(buf, len);
                if (!bincom_mode)
                    DM.monitorString(cfg2->monitor, DIR_TX, buf, len);
            }
        }

        // Handle RX for UART 2 (S2)
        if (ebits & cfg2->rx_char) {
            uint32_t available = std::min(cfg2->mySL->available(), 511);
            if (available) {
                uint16_t rxBytes = cfg2->mySL->readBufFromQueue((uint8_t*)buf, available);
                buf[rxBytes] = 0;
                cfg2->mySL->receive(buf, rxBytes);
                DM.monitorString(cfg2->monitor, DIR_RX, buf, rxBytes);
            }
        }

        if (Flarm::bincom) {
            if (!bincom_mode) {  // We are in bincom mode, stop this mode if Flarm has detected otherwise
                if (cfg1->mySL->number() == Flarm::bincom_port)
                    enterBincomMode(cfg1);
                else if (cfg2->mySL->number() == Flarm::bincom_port)
                    enterBincomMode(cfg2);
            }
        } else {
            if (bincom_mode) {  // Flarm says we are in text mode, exit serial bincom mode if not yet done
                if (cfg1->mySL->number() == Flarm::bincom_port)
                    exitBincomMode(cfg1);
                else if (cfg2->mySL->number() == Flarm::bincom_port)
                    exitBincomMode(cfg2);
            }
        }
    } // end while(true)
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

bool Serial::selfTest(SerialLine *mySerial){
	ESP_LOGI(FNAME,"Serial S%d selftest", mySerial->number() );
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

void Serial::begin( SerialLine *s1, SerialLine *s2 ){   // will be obsoleted when Devices launch serial interfaces.
	ESP_LOGI(FNAME,"Serial::begin()" );
	// Initialize configuration
	S[0].cfg2 = &S[1];
	S[1].cfg2 = &S[0];
	S[0].mySL = s1;
	S[1].mySL = s2;
}

bool Serial::taskStarted(){
	bool ret=true;
	if( pid == NULL )
		ret=false;
	ESP_LOGI(FNAME,"taskStarted() ret:%d pid:%d", ret, (int)pid );
	return ret;
}

// will be obsoleted when devices launch serial lines
void Serial::taskStart(){
	ESP_LOGI(FNAME,"Serial::taskStart()" );
	if( !taskStarted() )
		xTaskCreatePinnedToCore(&serialHandler, "serialHandler", 4096, NULL, 13, &pid, 0);  // stay below canbus
	delay(100);
	bool serial1 = (serial1_speed.get() != 0 || wireless != 0);
	bool serial2 = (serial2_speed.get() != 0 && hardwareRevision.get() >= XCVARIO_21);
	if( S1  && serial1 ){
		ESP_LOGI(FNAME,"S1 enable Int" );
		S1->enableRxInterrupt();
	}
	if( S2 && serial2 ){
		ESP_LOGI(FNAME,"S2 enable Int" );
		S2->enableRxInterrupt();
	}
}

void Serial::taskStop(){
	if( pid != NULL ){
		vTaskDelete( pid );
		pid = NULL;
	}
}
