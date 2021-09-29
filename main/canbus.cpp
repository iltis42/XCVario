
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


/*
 *  Code for a 1:1 connection between two XCVario with a fixed message ID
 *
 *
 *
 */

int CANbus::_tick = 0;
bool CANbus::_ready_initialized = false;
gpio_num_t CANbus::_tx_io = CAN_BUS_TX_PIN;
gpio_num_t CANbus::_rx_io = CAN_BUS_RX_PIN;
bool       CANbus::_connected_magsens=false;
bool       CANbus::_connected_xcv=false;
int        CANbus::_connected_timeout_magsens=0;
int        CANbus::_connected_timeout_xcv=0;
bool       CANbus::_master_present = false; // todo, create connect event
int        CANbus::dataIndex=0;

static TaskHandle_t cpid;

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
	twai_timing_config_t t_config;
	if( can_speed.get() == CAN_SPEED_250KBIT ){
		ESP_LOGI(FNAME,"CAN rate 250KBit");
		t_config = TWAI_TIMING_CONFIG_250KBITS();
	}
	else if( can_speed.get() == CAN_SPEED_500KBIT ){
		ESP_LOGI(FNAME,"CAN rate 500KBit");
		t_config = TWAI_TIMING_CONFIG_500KBITS();
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
	delay(10);
	//Start TWAI driver
	if (twai_start() == ESP_OK) {
		ESP_LOGI(FNAME,"Driver started");
        _ready_initialized = true;
        // Set RS pin
        // bus_off_io may operate invers, so for now set this here
      	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    	gpio_set_level(GPIO_NUM_2, 0 );
	    delay(100);
	} else {
        twai_driver_uninstall();
		ESP_LOGI(FNAME,"Failed to start driver");
	}
}

void CANbus::driverUninstall(){
    if( _ready_initialized ){
        _ready_initialized = false;
		twai_stop();
		twai_driver_uninstall();
        gpio_set_level(GPIO_NUM_2, 1 );
		delay(10);
	}
}

void canTask(void *pvParameters){
	while (true) {
		TickType_t xLastWakeTime = xTaskGetTickCount();
		CANbus::tick();
		if( (CANbus::_tick % 100) == 0) {
			// ESP_LOGI(FNAME,"Free Heap: %d bytes", heap_caps_get_free_size(MALLOC_CAP_8BIT) );
			if( uxTaskGetStackHighWaterMark( cpid ) < 128 )
				ESP_LOGW(FNAME,"Warning canbus task stack low: %d bytes", uxTaskGetStackHighWaterMark( cpid ) );
		}
		vTaskDelayUntil(&xLastWakeTime, 10/portTICK_PERIOD_MS);
	}
}

void::CANbus::restart(){
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
	xTaskCreatePinnedToCore(&canTask, "canTask", 4096, nullptr, 8, &cpid, 0);
}

// receive message of corresponding ID,
// return a terminated SString containing exactly the received chars
int CANbus::receive( int *id, SString& msg, int timeout ){
	twai_message_t rx;
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

int rx_pos=0;
// hook this into another task to save memory

void CANbus::on_can_connect( int msg ){
	if( !Flarm::bincom ){ // have a client to XCVario protocol connected
		// ESP_LOGV(FNAME, "on_client_connect: Send MC, Ballast, Bugs, etc");
		SetupCommon::syncEntry(msg);
	}
}

void CANbus::tick(){
	_tick++;
	if( !_ready_initialized ){
		ESP_LOGI(FNAME,"CAN not initialized");
		return;
	}
	SString msg;
	// CAN bus send tick
	if ( !can_tx_q.isEmpty() ){
		// ESP_LOGI(FNAME,"There is CAN data");
		if( _connected_xcv ){
			if( Router::pullMsg( can_tx_q, msg ) ){
				// ESP_LOGI(FNAME,"CAN TX len: %d bytes", msg.length() );
				// ESP_LOG_BUFFER_HEXDUMP(FNAME,msg.c_str(),msg.length(), ESP_LOG_INFO);
				if( !sendNMEA( msg ) ){
					_connected_timeout_xcv +=20;  // if sending fails as indication for disconnection
					ESP_LOGI(FNAME,"CAN TX NMEA failed, timeout=%d", _connected_timeout_xcv );
				}
			}
		}
	}
	// Can bus receive tick
	int id = 0;
	int bytes = receive( &id, msg, 10 );
	bool xcv_came=false;
	bool magsens_came=false;
	if( bytes  ){ // keep alive from second XCV
		// ESP_LOGI(FNAME,"CAN RX id:%02x, bytes:%d, connected XCV:%d Magsens: %d", id, bytes, _connected_xcv, _connected_magsens );
		if( id == 0x11 ){ // keep alive of msg from peer XCV ?
			// ESP_LOGI(FNAME,"CAN RX Keep Alive");
			xcv_came = true;
			_connected_timeout_xcv = 0;
			if( !_connected_xcv ){
				ESP_LOGI(FNAME,"CAN XCV connected");
				_connected_xcv = true;
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
			if( compass_enable.get() && !(_connected_timeout_magsens % 10000) )
				restart();
		}
	}
	if( !xcv_came ){
		_connected_timeout_xcv++;
		if( _connected_timeout_xcv > 500 ){
			if(  _connected_xcv ){
				ESP_LOGI(FNAME,"CAN XCV connection timeout");
				_connected_xcv = false;
				dataIndex = 0;
			}
			if( (can_mode.get() != CAN_MODE_STANDALONE) && !(_connected_timeout_xcv % 10000) )
				restart();
		}
	}
	// Handle data sync
	if( !(_tick%20) && _connected_xcv ){
		if( dataIndex < SetupCommon::numEntries() ){
			on_can_connect( dataIndex );
			dataIndex++;
		}
	}

	//if( !(_connected_timeout_xcv%100) )
	//	ESP_LOGI(FNAME,"CAN XCV connection timer: %d", _connected_timeout_xcv );
	// receive message of corresponding ID
	static SString nmea;
    static int nmea_state = 0; // poor man's nmea chunk counter and receiver state machine
    // nmea sentence: "$/! some content, some more, ... *5e\r\n" -> note * and checksum is by NMEA0183 spec optional
    // init w/ n_s=0   ^n_s=1      ^n_s=2...                 ^n_s=nr_of_chunks
	if( id == 0x20 ) { // nmea
		// ESP_LOGI(FNAME,"CAN RX NMEA frame");
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
                Router::forwardMsg( nmea, can_rx_q ); // All good
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

//	if( !(_tick%4) )
	Router::routeCAN();
	if( !(_tick%100) ){
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

bool CANbus::sendNMEA( const SString& msg ){
	if( !_ready_initialized ){
		ESP_LOGI(FNAME,"CAN not initialized");
		return false;
    }

	// ESP_LOGI(FNAME,"send CAN NMEA len %d, msg: %s", len, msg );
	const int chunk=8;
	const int id = 0x20;
    const char *cptr = msg.c_str();
    int len = msg.length()+1;
	while( len > 0 )
    {
    	int dlen = std::min(chunk, len);
        // Underlaying queue does does block until there is space,
        // only a timeout would return false.
		if( !sendData( id, cptr, dlen ) ){
			ESP_LOGW(FNAME,"send CAN chunk failed, %d remaining", len );
			return(false);
        }
		delay(10);
        cptr += dlen;
        len -= dlen;
    }

	return true;
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
		delay(10);
		SString msg;
		int rxid;
		int bytes = receive( &rxid, msg, 5 );
		// ESP_LOGI(FNAME,"RX CAN bus message bytes:%d, id:%04x, data:%s", bytes, id, msg.c_str() );
		if( bytes == 0 || rxid != id ){
			ESP_LOGW(FNAME,"CAN bus selftest RX call FAILED");
			delay(10*i);
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

	//Queue message for transmission
	esp_err_t error = twai_transmit(&message, pdMS_TO_TICKS(20));
	if(error == ESP_OK){
		// ESP_LOGI(FNAME,"Send CAN bus message okay");
		return true;
	}
	else{
		ESP_LOGW(FNAME,"Send CAN bus message failed, ret:%02x", error );
		return false;
	}
}
