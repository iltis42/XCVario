#include "driver/gpio.h"
#include "logdef.h"
#include "canbus.h"
#include "string.h"
#include "esp_err.h"
#include "sensor.h"
#include "Router.h"

/*
 *  Code for a 1:1 connection between two XCVario with a fixed message ID
 *
 *
 *
 */

int CANbus::_tick = 0;
bool CANbus::can_ready = false;
gpio_num_t CANbus::_tx_io;
gpio_num_t CANbus::_rx_io;
bool       CANbus::_connected;
int        CANbus::_connected_timeout;

TaskHandle_t *cpid;

/*
#define MY_TWAI_GENERAL_CONFIG_DEFAULT(tx_io_num, rx_io_num, op_mode) {.mode = op_mode, .tx_io = tx_io_num, .rx_io = rx_io_num,     \
                                                                    .clkout_io = TWAI_IO_UNUSED, .bus_off_io = TWAI_IO_UNUSED,          \
                                                                    .tx_queue_len = 5, .rx_queue_len = 5,                           \
                                                                    .alerts_enabled = TWAI_ALERT_NONE,  .clkout_divider = 0,         \
                                                                    .intr_flags = ESP_INTR_FLAG_LEVEL1}
*/

#define MSG_ID 0x555


// install/reinstall CAN driver in corresponding mode
void CANbus::driverInstall( twai_mode_t mode, bool reinstall ){
	if( reinstall ){
		twai_stop();
		twai_driver_uninstall();
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
	} else {
		ESP_LOGI(FNAME,"Failed to start driver");
		return;
	}
}


void canTask(void *pvParameters){
	while (1) {
		CANbus::tick();
		if( (CANbus::_tick % 100) == 0) {
			// ESP_LOGI(FNAME,"Free Heap: %d bytes", heap_caps_get_free_size(MALLOC_CAP_8BIT) );
			if( uxTaskGetStackHighWaterMark( cpid ) < 128 )
				ESP_LOGW(FNAME,"Warning canbus task stack low: %d bytes", uxTaskGetStackHighWaterMark( cpid ) );
		}
	}
}


// begin CANbus, start selfTest and launch driver in normal (bidir) mode afterwards
void CANbus::begin( gpio_num_t tx_io, gpio_num_t rx_io )
{
    //Initialize configuration structures using macro initializers
	if( can_speed.get() == CAN_SPEED_OFF ){
		ESP_LOGI(FNAME,"CAN bus OFF");
		return;
	}
	ESP_LOGI(FNAME,"CANbus::begin");
	_tx_io = tx_io;
	_rx_io = rx_io;
    driverInstall( TWAI_MODE_NO_ACK );
    // Set RS pin
    // bus_off_io may operate invers, so for now set this here
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_2, 0 );
    delay(100);
    can_ready = true;
	selfTest();
	driverInstall( TWAI_MODE_NORMAL, true );
	xTaskCreatePinnedToCore(&canTask, "canTask", 4096, NULL, 8, cpid, 0);
}

// receive message of corresponding ID
SString nmea;

int CANbus::receive( int *id, char *msg, int timeout ){
	twai_message_t rx;
	esp_err_t ret = twai_receive(&rx, pdMS_TO_TICKS(timeout) );
	// ESP_LOGI(FNAME,"RX CAN bus message ret=%02x", ret );
	if(ret == ESP_OK ){
		if( rx.data_length_code > 0 && rx.data_length_code <= 8 ){
			memcpy( msg,rx.data, rx.data_length_code );
			msg[rx.data_length_code] = 0;
			// ESP_LOGI(FNAME,"RX CAN bus message ret=(%02x), id:%04x bytes:%d, data:%s", ret, rx.identifier, rx.data_length_code, msg );
			*id = rx.identifier;
			return rx.data_length_code;
		}
	}
	return 0;
}

int rx_pos=0;
// hook this into another task to save memory
void CANbus::tick(){
	_tick++;
	if( !can_ready ){
		// ESP_LOGI(FNAME,"CANbus not ready");
		return;
	}
	SString msg;
	// CAN bus send tick
	if ( !can_tx_q.isEmpty() ){
		// ESP_LOGI(FNAME,"There is CAN data");
		if( _connected ){
			while( Router::pullMsg( can_tx_q, msg ) ) {
				// ESP_LOGI(FNAME,"CAN TX len: %d bytes", msg.length() );
				// ESP_LOG_BUFFER_HEXDUMP(FNAME,s.c_str(),s.length(), ESP_LOG_DEBUG);
				sendNMEA( msg.c_str() );
			}
		}
	}
	msg.clear();
	// Can bus receive tick
	int id = 0;
	int bytes = receive( &id, msg.c_str(), 10 );
	// ESP_LOGI(FNAME,"CAN RX id:%02x, bytes:%d, connected:%d", id, bytes, _connected );
	if( bytes ){
		_connected = true;
		_connected_timeout = 0;
	}
	else{
		_connected_timeout++;
		if( _connected_timeout > 100 )
			_connected = false;
	}
	if( id == 0x20 ) {     // start of nmea
		// ESP_LOGI(FNAME,"CAN RX Start of frame");
		nmea.clear();
		nmea.append( msg.c_str(), bytes );
	}
	else if( id == 0x21 ){ // segment
		// ESP_LOGI(FNAME,"CAN RX frame segment");
		nmea.append( msg.c_str(), bytes );
	}
	else if( id == 0x22 ){
		nmea.append( msg.c_str(), bytes );
		// ESP_LOGI(FNAME,"CAN RX, msg: %s", nmea.c_str() );
		Router::forwardMsg( nmea, can_rx_q );
	}
	if( !(_tick%4) )
		Router::routeCAN();
	if( !(_tick%50) )
		sendData( 0x11, msg.c_str(), 1 ); // keep alive
}


bool CANbus::sendNMEA( const char *msg ){
	if( !can_ready )
		return false;
	int len=strlen(msg);
	// ESP_LOGI(FNAME,"send CAN NMEA len %d, msg: %s", len, msg );
	bool ret = true;
	int id = 0x20;
	int dlen=8;
	int pos;
	for( pos=0; pos < len; pos+=8 ){
		int rest = len - pos;
		if( !sendData( id, &msg[pos], dlen ) )
			ret = false;
		if( rest > 0 && rest <= 8 ){
			dlen = rest;
			break;
		}else{
			rest = 0;
		}
		// ESP_LOGI(FNAME,"Sent id:%d pos:%d dlen %d", id, pos, dlen );
		id = 0x21;
	}
	id = 0x22;
	// ESP_LOGI(FNAME,"Sent id:%d pos:%d dlen %d", id, pos, dlen );
	if( !sendData( id, &msg[pos], dlen ) )
		ret = false;

	return ret;
}


bool CANbus::selfTest(){
	ESP_LOGI(FNAME,"CAN bus selftest");
	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_NUM_2, 0 );
	delay(100);
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
		char msg[12];
		int rxid;
		int bytes = receive( &rxid, msg, 5 );
		// ESP_LOGI(FNAME,"RX CAN bus message bytes:%d, id:%04x, data:%s", bytes, id, msg );
		if( bytes == 0 || rxid != id ){
			ESP_LOGW(FNAME,"CAN bus selftest RX call FAILED");
			delay(10*i);
		}
		else if( memcmp( msg ,tx, len ) == 0 ){
			ESP_LOGW(FNAME,"+++ CAN bus selftest TX/RX OKAY +++");
			res=true;
			break;
		}
	}
    if( res ){
    	ESP_LOGI(FNAME,"CAN bus selftest TX/RX OKAY");
    	return true;
    }else{
    	ESP_LOGW(FNAME,"CAN bus selftest TX/RX FAILED");
    	can_ready = false;
    }
    return false;
}

bool CANbus::sendData( int id, const char* msg, int length, int self ){
	// ESP_LOGI(FNAME,"CANbus::send %d bytes, msg %s, self %d", length, msg, self );
	if( !can_ready ){
		// ESP_LOGI(FNAME,"CANbus not ready, abort");
		return false;
	}
	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_NUM_2, 0 );
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
	esp_err_t error = twai_transmit(&message, pdMS_TO_TICKS(10));
	if(error == ESP_OK){
		// ESP_LOGI(FNAME,"Send CAN bus message okay");
		return true;
	}
	else{
		// ESP_LOGI(FNAME,"Send CAN bus message failed, ret:%02x", error );
		return false;
	}
}



