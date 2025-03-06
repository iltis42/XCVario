
#include "canbus.h"

#include "sensor.h"
#include "RingBufCPP.h"
#include "Router.h"
#include "QMCMagCAN.h"
#include "Flarm.h"
#include "Switch.h"


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"

#include <cstring>
#include "DataMonitor.h"
#include "Flarm.h"
#include "DataLink.h"

DataLink dlink;
DataLink dlinkXs;  // use a second CAN id and datalink to avoid checksum errors by mixed up messages

/*
 *  Code for a 1:1 connection between two XCVario with a fixed message ID
 *
 */
CANbus* CAN = 0;

xSemaphoreHandle sendMutex = 0;
xSemaphoreHandle nmeaMutex = 0;

CANbus::CANbus()
{
	sendMutex = xSemaphoreCreateMutex();
	nmeaMutex = xSemaphoreCreateMutex();

	if( SetupCommon::isClient() ){  // client uses different ID, so prepare canbus for client role
		_can_id_config_tx = CAN_CONFIG_ID_CLIENT;
		_can_id_config_rx = CAN_CONFIG_ID_MASTER;
		_can_id_nmea_tx = CAN_NMEA_ID_CLIENT;
		_can_id_nmea_rx = CAN_NMEA_ID_MASTER;
		_can_id_keepalive_tx = CAN_KEEPALIVE_ID_CLIENT;
		_can_id_keepalive_rx = CAN_KEEPALIVE_ID_MASTER;
	}else{
		_can_id_config_tx = CAN_CONFIG_ID_MASTER;
		_can_id_config_rx = CAN_CONFIG_ID_CLIENT;
		_can_id_nmea_tx = CAN_NMEA_ID_MASTER;
		_can_id_nmea_rx = CAN_NMEA_ID_CLIENT;
		_can_id_keepalive_tx = CAN_KEEPALIVE_ID_MASTER;
		_can_id_keepalive_rx = CAN_KEEPALIVE_ID_CLIENT;
	}
}

// install/reinstall CAN driver in corresponding mode
void CANbus::driverInstall( twai_mode_t mode ){
	if( _ready_initialized ){
		driverUninstall();
	}
	twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT( _tx_io, _rx_io, mode );
	ESP_LOGI(FNAME, "default alerts %X", g_config.alerts_enabled);
	g_config.alerts_enabled |= TWAI_ALERT_TX_IDLE | TWAI_ALERT_TX_SUCCESS | TWAI_ALERT_TX_FAILED;
	if ( _slope_support ) {
		g_config.bus_off_io = GPIO_NUM_2;
	}
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
	// t_config.triple_sampling = true; // improved sampling incoming bits, no effect in test

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
		if ( _slope_support ) {
			gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
		}
		_ready_initialized = true;
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
		delay(100);
	}
}

void canTxTask(void *arg){
	unsigned int tick = 0;
	while (true) {
		TickType_t xLastWakeTime = xTaskGetTickCount();
		if( !Flarm::bincom ){
			static_cast<CANbus*>(arg)->txtick(tick);
		}
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
		// TickType_t xLastWakeTime = xTaskGetTickCount();
		ESP_LOGI(FNAME,"Tick: %d", tick );
		if( !Flarm::bincom ){
			static_cast<CANbus*>(arg)->rxtick(tick);
		}
		if( (tick++ % 100) == 0) {
			ESP_LOGI(FNAME,"Free Heap: %d bytes", heap_caps_get_free_size(MALLOC_CAP_8BIT) );
			if( uxTaskGetStackHighWaterMark( nullptr ) < 128 )
				ESP_LOGW(FNAME,"Warning canbus rxtask stack low: %d bytes", uxTaskGetStackHighWaterMark( nullptr ) );
		}
	}
}

void CANbus::restart(){
	if( can_speed.get() == CAN_SPEED_OFF ){
		return;
	}
	ESP_LOGW(FNAME,"CANbus restart");
	driverUninstall();
	driverInstall( TWAI_MODE_NORMAL );
	_connected_timeout_magsens = 0;
	_connected_timeout_xcv = 0;
}

void CANbus::recover(){
	if( can_speed.get() == CAN_SPEED_OFF ){
		return;
	}

	twai_status_info_t status_info;
	if ( twai_get_status_info(&status_info) == ESP_OK ) {
		if ( status_info.state == TWAI_STATE_BUS_OFF ) {
			// recovery is only possible in this state
			ESP_LOGW(FNAME,"CANbus recover");
			twai_initiate_recovery();
			delay(10);
			twai_start();
		}
	}
	_connected_timeout_xcv = 0;
	_connected_timeout_magsens = 0;
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
	xTaskCreatePinnedToCore(&canTxTask, "canTxTask", 4096, this, 15, 0, 0);
	xTaskCreatePinnedToCore(&canRxTask, "canRxTask", 4096, this, 15, 0, 0);
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
	// ESP_LOGI(FNAME,"RX CAN bus message ret=%02x len_%d ID:%d dlc:%d ext:%d rtr:%d", ret, rx.data_length_code, rx.identifier, rx.dlc_non_comp, rx.extd, rx.rtr );
	// if( rx.data_length_code <= 8 ){
	//	ESP_LOG_BUFFER_HEXDUMP(FNAME,rx.data, rx.data_length_code , ESP_LOG_INFO);
	// }
	if(ret == ESP_OK ){
		// ESP_LOGI(FNAME,"RX CAN bus message ret=%02x TO:%d", ret, _connected_timeout_xcv );
		if( rx.data_length_code > 0 && rx.data_length_code <= 8 ){
			msg.set((const char*)rx.data, rx.data_length_code);
			// ESP_LOGI(FNAME,"RX CAN bus message ret=(%02x), id:%04x bytes:%d, data:%s", ret, rx.identifier, rx.data_length_code, msg.c_str() );
			// ESP_LOG_BUFFER_HEXDUMP(FNAME,msg.c_str(), msg.length() , ESP_LOG_INFO);
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
	if ( !can_tx_q.isEmpty() ){
		// ESP_LOGI(FNAME,"There is CAN data");
		if( _connected_xcv ){
			// ESP_LOGI(FNAME,"CAN TX Q:%d", can_tx_q.numElements() );
			while( Router::pullMsg( can_tx_q, msg ) ){
				// ESP_LOGI(FNAME,"CAN TX len: %d bytes Q:%d", msg.length(), can_tx_q.numElements() );
				// ESP_LOG_BUFFER_HEXDUMP(FNAME,msg.c_str(),msg.length(), ESP_LOG_INFO);
				DM.monitorString( MON_CAN, DIR_TX, msg.c_str(), msg.length() );
				if( !sendNMEA( msg ) ){
					_connected_timeout_xcv +=150;  // if sending fails as indication for disconnection
					// ESP_LOGW(FNAME,"CAN TX NMEA failed, timeout=%d", _connected_timeout_xcv );
					if( _connected_timeout_xcv > 1000 )
						recover();
				}
			}
		}
	}
	// Router::routeCAN();
	if( !(tick%100) ){
		if( ((can_mode.get() == CAN_MODE_CLIENT)  && _connected_xcv) || can_mode.get() == CAN_MODE_MASTER ){ // sent from client only if keep alive is there
			msg.set( "K" );
			if( !sendData( _can_id_keepalive_tx, msg.c_str(), 1 ) )
			{
				_connected_timeout_xcv +=150;  // if sending fails as indication for disconnection
				if( !_keep_alive_fails ){
					ESP_LOGW(FNAME,"Permanent CAN TX Keep Alive failure");
					_keep_alive_fails = true;
				}
				if( _connected_timeout_xcv > 1000 )
					recover();
			}else{
				if( _keep_alive_fails ){
					ESP_LOGI(FNAME,"Okay again CAN TX Keep Alive");
					_keep_alive_fails = false;
				}
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
	int bytes = 0;
	bool xcv_came=false;
	bool magsens_came=false;
	do{
		bytes = receive( &id, msg, 500 );
		// ESP_LOGI(FNAME,"CAN RX id:%02x, bytes:%d, connected XCV:%d Magsens: %d", id, bytes, _connected_xcv, _connected_magsens );
		if( bytes  ){ // keep alive from second XCV
			if( id == _can_id_keepalive_rx ){ // keep alive of msg from peer XCV ?
				// ESP_LOGI(FNAME,"CAN RX Keep Alive");
				xcv_came = true;
				_connected_timeout_xcv = 0;
				if( !_connected_xcv ){
					bool entry=false;
					do{ // cleanup congestion at startup
						entry = Router::pullMsg( can_tx_q, msg );
					}while( entry );
					ESP_LOGI(FNAME,"CAN XCV connected");
					_connected_xcv = true;
					_new_can_client_connected = (the_can_mode == CAN_MODE_MASTER);
				}
			}
			if( id == CAN_MAGSENS_ID ){
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
				if( compass_enable.get() == CS_CAN && (_connected_timeout_magsens > 10000) && !_connected_xcv ){
					// only restart when xcv is not connected, otherwise magsensor may be just plugged out
					ESP_LOGI(FNAME,"CAN Magnet Sensor restart timeout");
					restart();
				}
			}
		}
		if( !xcv_came ){
			_connected_timeout_xcv++;
			if( _connected_timeout_xcv > 200 ){
				if(  _connected_xcv ){
					ESP_LOGI(FNAME,"CAN XCV connection timeout");
					_connected_xcv = false;
				}
				if( (can_mode.get() != CAN_MODE_STANDALONE) && !(_connected_timeout_xcv % 10000) ){
					ESP_LOGI(FNAME,"CAN restart timeout");
					restart();
				}
			}
		}
		// process NMEA message depending on role and peer
		if( id == _can_id_nmea_rx ) {
			// ESP_LOGI(FNAME,"CAN RX NMEA chunk, len:%d msg: %s", bytes, msg.c_str() );
			// ESP_LOG_BUFFER_HEXDUMP(FNAME, msg.c_str(), msg.length(), ESP_LOG_INFO);
			_connected_timeout_xcv = 0;
			dlink.process( msg.c_str(), msg.length(), 3 );  // (char *packet, int len, int port );
		}
		if( id == _can_id_config_rx ) {   // CAN messages for config (!xs) and NMEA may mix up: Fixed by a second ID and datalink layer for config variables
			// ESP_LOGI(FNAME,"CAN RX NMEA chunk, len:%d msg: %s", bytes, msg.c_str() );
			// ESP_LOG_BUFFER_HEXDUMP(FNAME, msg.c_str(), msg.length(), ESP_LOG_INFO);
			_connected_timeout_xcv = 0;
			dlinkXs.process( msg.c_str(), msg.length(), 3 );  // (char *packet, int len, int port );
		}
		else if( id == CAN_MAGSENS_ID ){ // magnet sensor
			// ESP_LOGI(FNAME,"CAN RX MagSensor, msg: %d", bytes );
			// ESP_LOG_BUFFER_HEXDUMP(FNAME, msg.c_str(), bytes, ESP_LOG_INFO);
			QMCMagCAN::fromCAN( msg.c_str(), msg.length() );
			_connected_timeout_magsens = 0;
		}
		DM.monitorString( MON_CAN, DIR_RX, msg.c_str(), msg.length());

	}while( bytes );
}

bool CANbus::sendNMEA( const SString& msg ){
	if( !_ready_initialized ){
		// ESP_LOGI(FNAME,"CAN not initialized");
		return false;
	}
	xSemaphoreTake(nmeaMutex,portMAX_DELAY );

	bool ret = true;
	uint32_t alerts;
	twai_read_alerts(&alerts, 0); // read and clear alerts
	if( alerts != 0 )
		ESP_LOGW(FNAME,"Before send alerts %X", alerts);

	// ESP_LOGI(FNAME,"send CAN NMEA len %d msg: %s ", msg.length(), msg.c_str() );
	// ESP_LOG_BUFFER_HEXDUMP(FNAME, msg.c_str(), msg.length(), ESP_LOG_INFO);

	const int chunk=8;
	int id = _can_id_nmea_tx;
	if( !strncmp( msg.c_str(), "!xs", 3) )  // segregate internal NMEA by different id for !xs
		id = _can_id_config_tx;

	const char *cptr = msg.c_str();
	int len = msg.length(); // Including the terminating \0 -> need to remove this one byte at RX from strlen
	while( len > 0 )
	{
		int dlen = std::min(chunk, len);
		// Underlaying queue does block until there is space,
		// only a timeout would return false.
		if( !sendData(id, cptr, dlen) ) {
			if( !_send_nmea_fails ){
				ESP_LOGW(FNAME,"Permanent send CAN NMEA failure msg: %s chunk: %s", msg.c_str(), cptr );
				_send_nmea_fails = true;
			}
			ret = false;
		}else{
			if( _send_nmea_fails ){
				ESP_LOGI(FNAME,"Okay again send CAN NMEA failed msg: %s chunk: %s", msg.c_str(), cptr );
				_send_nmea_fails = false;
			}
		}
		cptr += dlen;
		len -= dlen;
	}
	xSemaphoreGive(nmeaMutex);
	return ret;
}

#define CANTEST_ID 0x100

bool CANbus::selfTest()
{
    ESP_LOGI(FNAME, "CAN bus selftest");

    // Pretend slope control off and probe the reaction on GPIO 2 here
    _slope_support = true;
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    // in case of GPIO 2 wired to CAN this would inhibit sending and cause a failing test

    driverInstall(TWAI_MODE_NO_ACK);
    bool res = false;
    int id = CANTEST_ID;
    delay(100);
    _slope_support = false;
    for (int slope = 0; slope <=1; slope++)
    {
        ESP_LOGI(FNAME,"slope support %s.", (slope==0) ? "on" : "off");
        gpio_set_level(GPIO_NUM_2, slope);
        for (int i = 0; i < 3; i++)
        {
            // repeat test 3x
        	ESP_LOGI(FNAME,"test #%d", i);
            char tx[10] = {"1827364"};
            int len = strlen(tx);
            ESP_LOGI(FNAME, "strlen %d", len);
            twai_clear_receive_queue(); // there might be data from a remote device

            if ( ! sendData(id, tx, len, 1) ) {
                ESP_LOGW(FNAME, "TX Errors");
            }
            twai_message_t rx;
            if ( (ESP_OK == twai_receive(&rx, pdMS_TO_TICKS(20)))
                && (rx.identifier == id)
                && (rx.data_length_code == len)
                && (memcmp(rx.data, tx, len) == 0) )
            {
                ESP_LOGI(FNAME, "RX OKAY");
                res = true;
            }
            else
            {	// we can only detect this if it fails
            	if( slope == 1 ){
            		_slope_support = true;
            		ESP_LOGI(FNAME, "CAN HW supports slope !");
            	}
                std::string msg((char*)rx.data, rx.data_length_code);
                ESP_LOGW(FNAME, "RX FAILED:  bytes:%d rxid:%x rxmsg:%s", rx.data_length_code, (unsigned int)rx.identifier, msg.c_str());
                twai_clear_receive_queue();
            }
        }
    }
    driverUninstall();
    ESP_LOGW(FNAME, "Final Result: CAN bus selftest %s, slope support: %d", res ? "OKAY" : "FAILED", _slope_support );
    return res;
}

// Send, handle alerts, do max 3 retries
bool CANbus::sendData( int id, const char* msg, int length, int self ){
	if( !_ready_initialized ){
		ESP_LOGI(FNAME,"CANbus not ready initialized");
		return false;
	}
	xSemaphoreTake(sendMutex,portMAX_DELAY );

	twai_message_t message;
	memset( &message, 0, sizeof( message ) );
	message.identifier = id;
	message.self = self;
	message.data_length_code = length;

	for (int i = 0; i < length; i++) {
		message.data[i] = msg[i];
	}
	// ESP_LOGI(FNAME,"TX CAN bus message id:%04x, bytes:%d, data:%s, self:%d", message.identifier, message.data_length_code, message.data, message.self );
	// ESP_LOGI(FNAME,"send CAN chunk len %d msg: %s ", length, msg );
	// ESP_LOG_BUFFER_HEXDUMP(FNAME, msg, length, ESP_LOG_INFO);

	// Queue message for transmission
	uint32_t alerts;
	int retry = 3;
	esp_err_t error = ESP_OK;
	while( (retry-- > 0) && (ESP_OK != (error=twai_transmit(&message, 0))) ) {
		twai_read_alerts(&alerts, pdMS_TO_TICKS(_tx_timeout));
		// ESP_LOGW(FNAME,"Tx chunk failed alerts %X", alerts );
	}
	bool ret=false;
	if ( error == ESP_OK ) {
		twai_read_alerts(&alerts, pdMS_TO_TICKS(_tx_timeout));
		// if ( alerts & TWAI_ALERT_TX_IDLE )
		//    ESP_LOGW(FNAME,"TX IDLE alert %X", alerts );
		if( alerts & TWAI_ALERT_RX_QUEUE_FULL )
			ESP_LOGW(FNAME,"RX QUEUE FULL alert %X", alerts );
		if( alerts & TWAI_ALERT_TX_FAILED )
			ESP_LOGW(FNAME,"TX_FAILED alert %X", alerts );
		// ESP_LOGI(FNAME,"Send CAN bus message okay");
		ret = true;
	}
	xSemaphoreGive(sendMutex);
	return ret;
}
