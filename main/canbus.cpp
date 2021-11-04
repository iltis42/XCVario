
#include "canbus.h"

#include "sensor.h"
#include "RingBufCPP.h"
#include "Router.h"
#include "QMC5883L.h"
#include "Flarm.h"
#include "Switch.h"


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"

#include <cstring>
#include "DataMonitor.h"



/*
 *  Code for a 1:1 connection between two XCVario with a fixed message ID
 *
 */
CANbus* CAN = 0;


/*
#define MY_TWAI_GENERAL_CONFIG_DEFAULT(tx_io_num, rx_io_num, op_mode) {.mode = op_mode, .tx_io = tx_io_num, .rx_io = rx_io_num,     \
                                                                    .clkout_io = TWAI_IO_UNUSED, .bus_off_io = TWAI_IO_UNUSED,          \
                                                                    .tx_queue_len = 5, .rx_queue_len = 5,                           \
                                                                    .alerts_enabled = TWAI_ALERT_NONE,  .clkout_divider = 0,         \
                                                                    .intr_flags = ESP_INTR_FLAG_LEVEL1}
*/


// install/reinstall CAN driver in corresponding mode
void CANbus::driverInstall( twai_mode_t mode ){
	if( _ready_initialized ){
        driverUninstall();
	}
	twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT( _tx_io, _rx_io, mode );
    ESP_LOGI(FNAME, "default alerts %X", g_config.alerts_enabled);
    g_config.alerts_enabled |= TWAI_ALERT_TX_IDLE | TWAI_ALERT_TX_SUCCESS | TWAI_ALERT_TX_FAILED;
    g_config.rx_queue_len = 15; // 1.5x the need of one NMEA sentence
    g_config.tx_queue_len = 15;
    ESP_LOGI(FNAME, "my alerts %X", g_config.alerts_enabled);

	twai_timing_config_t t_config;
    _tx_timeout = 2; // 111usec/chunk -> 2msec
	if( can_speed.get() == CAN_SPEED_250KBIT ){
		ESP_LOGI(FNAME,"CAN rate 250KBit");
		t_config = TWAI_TIMING_CONFIG_250KBITS();
        _tx_timeout = 4; // 444usec/chunk -> 4msec

	}
	else if( can_speed.get() == CAN_SPEED_500KBIT ){
		ESP_LOGI(FNAME,"CAN rate 500KBit");
		t_config = TWAI_TIMING_CONFIG_500KBITS();
        _tx_timeout = 2; // 222usec/chunk -> 2msec
	}
	else if( can_speed.get() == CAN_SPEED_1MBIT ){
		ESP_LOGI(FNAME,"CAN rate 1MBit");
		t_config = TWAI_TIMING_CONFIG_1MBITS();
	}
	else{
		ESP_LOGI(FNAME,"CAN rate 1MBit for selftest");
		t_config = TWAI_TIMING_CONFIG_1MBITS();
	}

	twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    //Install TWAI driver
	if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
		ESP_LOGI(FNAME,"Driver installed OK, mode %d", mode );
	} else {
		ESP_LOGI(FNAME,"Failed to install driver");
		return;
	}

	//Start TWAI driver
	if (twai_start() == ESP_OK) {
		ESP_LOGI(FNAME,"Driver started");
		delay(10);
        _ready_initialized = true;
        // Set RS pin
        // bus_off_io may operate invers, so for now set this here
      	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    	gpio_set_level(GPIO_NUM_2, 0 );
	    delay(10);
	} else {
        twai_driver_uninstall();
		ESP_LOGI(FNAME,"Failed to start driver");
	}
}

void CANbus::driverUninstall(){
    if( _ready_initialized ){
        _ready_initialized = false;
        delay(100);
		twai_stop();
		delay(100);
		twai_driver_uninstall();
        gpio_set_level(GPIO_NUM_2, 1 );
		delay(100);
	}
}

void canTxTask(void *arg){
    unsigned int tick = 0;
	while (true) {
		TickType_t xLastWakeTime = xTaskGetTickCount();
		static_cast<CANbus*>(arg)->txtick(tick);
		if( (tick++ % 100) == 0) {
			// ESP_LOGI(FNAME,"Free Heap: %d bytes", heap_caps_get_free_size(MALLOC_CAP_8BIT) );
			if( uxTaskGetStackHighWaterMark( nullptr ) < 128 )
				ESP_LOGW(FNAME,"Warning canbus txtask stack low: %d bytes", uxTaskGetStackHighWaterMark( nullptr ) );
		}
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
	}
}

void canRxTask(void *arg){
    int tick = 0;
	while (true) {
		TickType_t xLastWakeTime = xTaskGetTickCount();
		static_cast<CANbus*>(arg)->rxtick(tick);
		if( (tick++ % 100) == 0) {
			// ESP_LOGI(FNAME,"Free Heap: %d bytes", heap_caps_get_free_size(MALLOC_CAP_8BIT) );
			if( uxTaskGetStackHighWaterMark( nullptr ) < 128 )
				ESP_LOGW(FNAME,"Warning canbus rxtask stack low: %d bytes", uxTaskGetStackHighWaterMark( nullptr ) );
		}
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(5));
	}
}

void CANbus::restart(){
	if( can_speed.get() == CAN_SPEED_OFF ){
		return;
	}
	driverUninstall();
	driverInstall( TWAI_MODE_NORMAL );
}

// begin CANbus, start selfTest and launch driver in normal (bidir) mode afterwards
void CANbus::begin()
{
    //Initialize configuration structures using macro initializers
	if( can_speed.get() == CAN_SPEED_OFF ){
		ESP_LOGI(FNAME,"CAN bus OFF");
		return;
	}
	ESP_LOGI(FNAME,"CANbus::begin");
	driverInstall( TWAI_MODE_NORMAL );
	xTaskCreatePinnedToCore(&canTxTask, "canTxTask", 4096, this, 22, 0, 0);
	xTaskCreatePinnedToCore(&canRxTask, "canRxTask", 4096, this, 23, 0, 0);
}

// receive message of corresponding ID,
// return a terminated SString containing exactly the received chars
int CANbus::receive( int *id, SString& msg, int timeout ){
	twai_message_t rx;

	uint32_t alerts;
	twai_read_alerts(&alerts, pdMS_TO_TICKS(_tx_timeout));
    if( alerts & TWAI_ALERT_RX_QUEUE_FULL )
    	ESP_LOGW(FNAME,"receive: RX QUEUE FULL alert %X", alerts );

	esp_err_t ret = twai_receive(&rx, pdMS_TO_TICKS(timeout) );
	// ESP_LOGI(FNAME,"RX CAN bus message ret=%02x TO:%d", ret, _connected_timeout_xcv );
	if(ret == ESP_OK ){
		// ESP_LOGI(FNAME,"RX CAN bus message ret=%02x TO:%d", ret, _connected_timeout_xcv );
		if( rx.data_length_code > 0 && rx.data_length_code <= 8 ){
            msg.set((const char*)rx.data, rx.data_length_code);
			// ESP_LOGI(FNAME,"RX CAN bus message ret=(%02x), id:%04x bytes:%d, data:%s", ret, rx.identifier, rx.data_length_code, msg );
			*id = rx.identifier;
			return rx.data_length_code;
		}
	}
	return 0;
}

// hook this into another task to save memory
void CANbus::txtick(int tick){
	if( !_ready_initialized ){
		// ESP_LOGI(FNAME,"CAN not initialized");
		return;
	}
	SString msg;
	// CAN bus send
    if ( !client_tx_q.isEmpty() ){
        // ESP_LOGI(FNAME,"There is CAN data");
        if( _connected_xcv ){
        	// ESP_LOGI(FNAME,"CAN TX Q:%d", client_tx_q.numElements() );
            while( Router::pullMsg( client_tx_q, msg ) ){
                // ESP_LOGI(FNAME,"CAN TX len: %d bytes Q:%d", msg.length(), client_tx_q.numElements() );
                // ESP_LOG_BUFFER_HEXDUMP(FNAME,msg.c_str(),msg.length(), ESP_LOG_INFO);
            	DM.monitorString( MON_CAN, DIR_TX, msg.c_str() );
                if( !sendNMEA( msg ) ){
                    _connected_timeout_xcv +=20;  // if sending fails as indication for disconnection
                    ESP_LOGI(FNAME,"CAN TX NMEA failed, timeout=%d", _connected_timeout_xcv );
                }
            }
        }
    }
	Router::routeClient();
	if( !(tick%100) ){
		if( ((can_mode.get() == CAN_MODE_CLIENT)  && _connected_xcv) || can_mode.get() == CAN_MODE_MASTER ){ // sent from client only if keep alive is there
			msg.set( "K" );
			if( !sendData( 0x11, msg.c_str(), 1 ) )
			{
				_connected_timeout_xcv +=20;  // if sending fails as indication for disconnection
				ESP_LOGI(FNAME,"CAN TX Keep Alive failed, timeout=%d", _connected_timeout_xcv );
			}
		}
	}

}

void CANbus::rxtick(int tick){
	if( !_ready_initialized ){
		// ESP_LOGI(FNAME,"CAN not initialized");
        delay(100);
		return;
	}

	// Can bus receive
	SString msg;
	int id = 0;
	int bytes = receive( &id, msg, 10 ); // just block and wait for messages
	bool xcv_came=false;
	bool magsens_came=false;
	if( bytes  ){ // keep alive from second XCV
		// ESP_LOGI(FNAME,"CAN RX id:%02x, bytes:%d, connected XCV:%d Magsens: %d", id, bytes, _connected_xcv, _connected_magsens );
		if( id == 0x11 ){ // keep alive of msg from peer XCV ?
			// ESP_LOGI(FNAME,"CAN RX Keep Alive");
			xcv_came = true;
			_connected_timeout_xcv = 0;
			if( !_connected_xcv ){
				bool entry=false;
				do{ // cleanup congestion at startup
					entry = Router::pullMsg( client_tx_q, msg );
				}while( entry );
				ESP_LOGI(FNAME,"CAN XCV connected");
				_connected_xcv = true;
                _new_can_client_connected = (the_can_mode == CAN_MODE_MASTER);
			}
		}
		if( id == 0x031 ){
			magsens_came = true;
			_connected_timeout_magsens = 0;
			if( !_connected_magsens ){
				ESP_LOGI(FNAME,"CAN Magnetsensor connected");
				_connected_magsens = true;
			}
		}
	}
	if( !magsens_came ){
		_connected_timeout_magsens++;
		if( _connected_timeout_magsens > 500 ){
			if( _connected_magsens ){
				ESP_LOGI(FNAME,"CAN Magsensor connection timeout");
				_connected_magsens = false;
			}
			if( compass_enable.get() && !(_connected_timeout_magsens % 10000) && !_connected_xcv ){
				// only restart when xcv is not connected, otherwise magsensor may be just plugged out
				ESP_LOGI(FNAME,"CAN Magnet Sensor restart timeout");
				restart();
			}
		}
	}
	if( !xcv_came ){
		_connected_timeout_xcv++;
		if( _connected_timeout_xcv > 500 ){
			if(  _connected_xcv ){
				ESP_LOGI(FNAME,"CAN XCV connection timeout");
				_connected_xcv = false;
			}
			if( (can_mode.get() != CAN_MODE_STANDALONE) && !(_connected_timeout_xcv % 10000) ){
				ESP_LOGI(FNAME,"CAN XCV restart timeout");
				restart();
			}
		}
	}

	// receive message of corresponding ID
	static SString nmea;
    static int nmea_state = 0; // poor man's nmea chunk counter and receiver state machine
    // nmea sentence: "$/! some content, some more, ... *5e\r\n" -> note * and checksum is by NMEA0183 spec optional
    // init w/ n_s=0   ^n_s=1      ^n_s=2...                 ^n_s=nr_of_chunks
	if( id == 0x20 ) { // nmea
		// ESP_LOGI(FNAME,"CAN RX NMEA frame");
		// ESP_LOGI(FNAME,"CAN RX id:%02x len:%d", id, bytes );
		_connected_timeout_xcv = 0;

        // For n_s >=1 just increase it
        if ( nmea_state >= 1 ) nmea_state++;

        // Check always on sentence start signs
        const char* cptr = nullptr;
        if ( (cptr=std::strchr(msg.c_str(), '$')) != nullptr
            || (cptr=std::strchr(msg.c_str(), '!')) != nullptr ) {
            if ( nmea_state != 0 ) {
                ESP_LOGW(FNAME, "%d: %s\nUnexpected frame start: %s", nmea.length(), nmea.c_str(), msg.c_str());
            }
            nmea = msg; // Ok copy
            nmea_state = 1;
        }
        else cptr = msg.c_str();

        // Append follow-up chunks w/o extra condition
        if ( nmea_state > 1 ) {
            nmea += msg; // Append further chunks
        }

        // Check on the remaining string for the end sign
        if ( std::strchr(cptr, '\n') != nullptr ) {
            if ( nmea_state >= 1 ) {
                Router::forwardMsg( nmea, client_rx_q ); // All good
                DM.monitorString( MON_CAN, DIR_RX, nmea.c_str() );
            }
            else {
                ESP_LOGW(FNAME, "Unexpected frame end, message dropped at %d", nmea_state);
            }
            nmea_state = 0;
        }
	}
	else if( id == 0x031 ){ // magnet sensor
		// ESP_LOGI(FNAME,"CAN RX MagSensor, msg: %d", bytes );
		// ESP_LOG_BUFFER_HEXDUMP(FNAME, msg.c_str(), bytes, ESP_LOG_INFO);
		QMC5883L::fromCAN( msg.c_str() );
		_connected_timeout_magsens = 0;
	}

}

bool CANbus::sendNMEA( const SString& msg ){
	if( !_ready_initialized ){
		// ESP_LOGI(FNAME,"CAN not initialized");
		return false;
    }

    bool ret = true;
    uint32_t alerts;
    twai_read_alerts(&alerts, 0); // read and clear alerts
    if( alerts != 0 )
    	ESP_LOGW(FNAME,"Before send alerts %X", alerts);

	// ESP_LOGI(FNAME,"send CAN NMEA len %d, msg: %s", len, msg );
	const int chunk=8;
	const int id = 0x20;
    const char *cptr = msg.c_str();
    int len = msg.length() + 1; // Including the terminating \0
	while( len > 0 )
    {
    	int dlen = std::min(chunk, len);
        // Underlaying queue does does block until there is space,
        // only a timeout would return false.
		if( ! sendData(id, cptr, dlen) ) {
            ret = false;
        }
        cptr += dlen;
        len -= dlen;
    }
	return ret;
}

bool CANbus::selfTest(){
	ESP_LOGI(FNAME,"CAN bus selftest");
	driverInstall( TWAI_MODE_NO_ACK );
	bool res=false;
	int id=0x100;
	for( int i=0; i<10; i++ ){
		char tx[10] = { "18273645" };
		int len = strlen(tx);
		ESP_LOGI(FNAME,"strlen %d", len );
		twai_clear_receive_queue();  // there might be data from a remote device
		if( !sendData( id, tx,len, 1 ) ){
			ESP_LOGW(FNAME,"CAN bus selftest TX FAILED");
		}
		SString msg;
		int rxid;
		int bytes = receive( &rxid, msg, 5 );
		// ESP_LOGI(FNAME,"RX CAN bus message bytes:%d, id:%04x, data:%s", bytes, id, msg.c_str() );
		if( bytes == 0 || rxid != id ){
			ESP_LOGW(FNAME,"CAN bus selftest RX call FAILED");
			delay(1*i);
		}
		else if( memcmp( msg.c_str() ,tx, len ) == 0 ){
			res=true;
			break;
		}
	}
    if( res ){
    	ESP_LOGW(FNAME,"CAN bus selftest TX/RX OKAY");
    }else{
    	ESP_LOGW(FNAME,"CAN bus selftest TX/RX FAILED");
    	driverUninstall();
    }
    return _ready_initialized;
}

// Send, handle alerts, do max 3 retries
bool CANbus::sendData( int id, const char* msg, int length, int self ){
	if( !_ready_initialized ){
		ESP_LOGI(FNAME,"CANbus not ready initialized");
		return false;
	}
	twai_message_t message;
	memset( &message, 0, sizeof( message ) );
	message.identifier = id;
	message.self = self;
	message.data_length_code = length;

	for (int i = 0; i < length; i++) {
	    message.data[i] = msg[i];
	}
	// ESP_LOGI(FNAME,"TX CAN bus message id:%04x, bytes:%d, data:%s, self:%d", message.identifier, message.data_length_code, message.data, message.self );

	// Queue message for transmission
    uint32_t alerts;
    int retry = 3;
    esp_err_t error;
	while( (retry-- > 0) && (ESP_OK != (error=twai_transmit(&message, 0))) ) {
        twai_read_alerts(&alerts, pdMS_TO_TICKS(_tx_timeout));
        ESP_LOGW(FNAME,"Tx chunk failed alerts %X", alerts );
    }
    if ( error == ESP_OK ) {
        twai_read_alerts(&alerts, pdMS_TO_TICKS(_tx_timeout));
        // if ( alerts & TWAI_ALERT_TX_IDLE )
        //    ESP_LOGW(FNAME,"TX IDLE alert %X", alerts );
        if( alerts & TWAI_ALERT_RX_QUEUE_FULL )
		    ESP_LOGW(FNAME,"RX QUEUE FULL alert %X", alerts );
        if( alerts & TWAI_ALERT_TX_FAILED )
        	ESP_LOGW(FNAME,"TX_FAILED alert %X", alerts );

		// ESP_LOGI(FNAME,"Send CAN bus message okay");
		return true;
    }
	else{
		ESP_LOGW(FNAME,"Send CAN bus message failed, ret:%02x", error );
		return false;
	}
}
