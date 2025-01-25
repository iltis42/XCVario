
#include "CanBus.h"

#include "Messages.h"
#include "DeviceMgr.h"
#include "sensor.h"
#include "QMCMagCAN.h"
#include "Flarm.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "driver/gpio.h"
#include "logdef.h"
#include "esp_err.h"

#include <cstring>
#include "DataMonitor.h"
#include "Flarm.h"
#include "../DataLink.h"

DataLinkOld dlink;
DataLinkOld dlinkXs; // use a second CAN id and datalink to avoid checksum errors by mixed up messages

/*
 *  Connect two XCVario with a fixed message ID
 *  Connect other peripherals to the XCVario
 *  Maybe use a breakout bos to furthe connect serial devices and tunnel protocols
 */

#define CANTEST_ID 0x100
static int can_id_config_tx; // to unify CAN id's, the following _can* variables are initialized depending on master/client role in constructor
static int can_id_config_rx;
static int can_id_nmea_tx;
static int can_id_nmea_rx;
static int can_id_keepalive_tx;
static int can_id_keepalive_rx;

static TaskHandle_t rxTask = nullptr;
CANbus *CAN = 0;
static bool terminate_receiver = false;
static bool do_recover = false;

// CAN receiver task
void canRxTask(void *arg)
{
    CANbus *can = static_cast<CANbus *>(arg);
    unsigned int tick = 0;

    if ( ! can->isInitialized() )
    {
        ESP_LOGI(FNAME, "CANbus not ready");
        vTaskDelete(NULL);
        // return;
    }

    std::string msg; // scratch buffer
    msg.resize(10);
    do {
        // basically block on the twai receiver for ever
        twai_message_t rx;
        if (ESP_OK == twai_receive(&rx, pdMS_TO_TICKS(500)) && rx.data_length_code > 0)
        {
            msg.assign((char *)rx.data, rx.data_length_code);
            ESP_LOGD(FNAME, "CAN RX NMEA chunk, id:0x%x, len:%d msg: %s", rx.identifier, rx.data_length_code, msg.c_str());
            auto dl = can->_dlink.find(rx.identifier);
            if ( dl != can->_dlink.end() ) {
                dl->second->process(msg.data(), msg.size());
            }

            // bool xcv_came = false;
            // bool magsens_came = false;

            // // old receiver missing a data link
            // if (rx.identifier == can_id_keepalive_rx)
            // { // keep alive of msg from peer XCV ?
            //     // ESP_LOGI(FNAME,"CAN RX Keep Alive");
            //     xcv_came = true;
            //     // _connected_timeout_xcv = 0;
            //     // if( !_connected_xcv ){
            //     // 	bool entry=false;
            //     // 	do{ // cleanup congestion at startup
            //     // 		entry = Router::pullMsg( can_tx_q, msg );
            //     // 	}while( entry );
            //     // 	ESP_LOGI(FNAME,"CAN XCV connected");
            //     // 	_connected_xcv = true;
            //     // 	_new_can_client_connected = (the_can_mode == CAN_MODE_MASTER);
            //     // }
            // }
            // // process NMEA message depending on role and peer
            // else if (rx.identifier == can_id_nmea_rx)
            // {
            //     // ESP_LOGI(FNAME,"CAN RX NMEA chunk, len:%d msg: %s", bytes, msg.c_str() );
            //     // ESP_LOG_BUFFER_HEXDUMP(FNAME, msg.c_str(), msg.length(), ESP_LOG_INFO);
            //     // _connected_timeout_xcv = 0;
            //     dlink.process(msg.c_str(), msg.length(), 3); // (char *packet, int len, int port );
            // }
            // else if (rx.identifier == can_id_config_rx)
            // { // CAN messages for config (!xs) and NMEA may mix up: Fixed by a second ID and datalink layer for config variables
            //     // ESP_LOGI(FNAME,"CAN RX NMEA chunk, len:%d msg: %s", bytes, msg.c_str() );
            //     // ESP_LOG_BUFFER_HEXDUMP(FNAME, msg.c_str(), msg.length(), ESP_LOG_INFO);
            //     // _connected_timeout_xcv = 0;
            //     dlinkXs.process(msg.c_str(), msg.length(), 3); // (char *packet, int len, int port );
            // }
        }
        else
        {
            // protocol state machine may want to react on no traffic
            for (auto &dl : can->_dlink ) {
                dl.second->process(nullptr, 0);
            }
        }
        if ( terminate_receiver ) { break; }
        if ( do_recover ) {
            can->recover(); // Can only do this not waiting in twai_receive
            do_recover = false;
        }

        if ((tick++ % 100) == 0)
        {
            if (uxTaskGetStackHighWaterMark(rxTask) < 128)
            {
                ESP_LOGW(FNAME, "Warning canbus task stack low: %d bytes", uxTaskGetStackHighWaterMark(rxTask));
            }
        }
    } while (true);

    // cannot stop twai when waiting on twai_receive (->crash)
    can->driverUninstall();

    terminate_receiver = false; // handshake
    rxTask = nullptr;
    vTaskDelete(NULL);

    // // Can bus receive
    // SString msg;
    // int id = 0;
    // int bytes = 0;
    // bool xcv_came=false;
    // bool magsens_came=false;
    // do{
    // 	bytes = receive( &id, msg, 500 );
    // 	// ESP_LOGI(FNAME,"CAN RX id:%02x, bytes:%d, connected XCV:%d Magsens: %d", id, bytes, _connected_xcv, _connected_magsens );
    // 	if( bytes  ){ // keep alive from second XCV
    // 		if( id == _can_id_keepalive_rx ){ // keep alive of msg from peer XCV ?
    // 			// ESP_LOGI(FNAME,"CAN RX Keep Alive");
    // 			xcv_came = true;
    // 			_connected_timeout_xcv = 0;
    // 			if( !_connected_xcv ){
    // 				bool entry=false;
    // 				do{ // cleanup congestion at startup
    // 					entry = Router::pullMsg( can_tx_q, msg );
    // 				}while( entry );
    // 				ESP_LOGI(FNAME,"CAN XCV connected");
    // 				_connected_xcv = true;
    // 				_new_can_client_connected = (the_can_mode == CAN_MODE_MASTER);
    // 			}
    // 		}
    // 		if( id == CAN_MAGSENS_ID ){
    // 			magsens_came = true;
    // 			_connected_timeout_magsens = 0;
    // 			if( !_connected_magsens ){
    // 				ESP_LOGI(FNAME,"CAN Magnetsensor connected");
    // 				_connected_magsens = true;
    // 			}
    // 		}
    // 	}
    // 	if( !magsens_came ){
    // 		_connected_timeout_magsens++;
    // 		if( _connected_timeout_magsens > 500 ){
    // 			if( _connected_magsens ){
    // 				ESP_LOGI(FNAME,"CAN Magsensor connection timeout");
    // 				_connected_magsens = false;
    // 			}
    // 			if( compass_enable.get() == CS_CAN && (_connected_timeout_magsens > 10000) && !_connected_xcv ){
    // 				// only restart when xcv is not connected, otherwise magsensor may be just plugged out
    // 				ESP_LOGI(FNAME,"CAN Magnet Sensor restart timeout");
    // 				restart();
    // 			}
    // 		}
    // 	}
    // 	if( !xcv_came ){
    // 		_connected_timeout_xcv++;
    // 		if( _connected_timeout_xcv > 200 ){
    // 			if(  _connected_xcv ){
    // 				ESP_LOGI(FNAME,"CAN XCV connection timeout");
    // 				_connected_xcv = false;
    // 			}
    // 			if( (can_mode.get() != CAN_MODE_STANDALONE) && !(_connected_timeout_xcv % 10000) ){
    // 				ESP_LOGI(FNAME,"CAN restart timeout");
    // 				restart();
    // 			}
    // 		}
    // 	}
}

CANbus::CANbus()
{

    _tx_io = GPIO_NUM_26;
    _rx_io = GPIO_NUM_33;
    _slope_ctrl = GPIO_NUM_2;

    if (SetupCommon::isClient())
    { // client uses different ID, so prepare canbus for client role
        can_id_config_tx = CAN_CONFIG_ID_CLIENT;
        can_id_config_rx = CAN_CONFIG_ID_MASTER;
        can_id_nmea_tx = CAN_NMEA_ID_CLIENT;
        can_id_nmea_rx = CAN_NMEA_ID_MASTER;
        can_id_keepalive_tx = CAN_KEEPALIVE_ID_CLIENT;
        can_id_keepalive_rx = CAN_KEEPALIVE_ID_MASTER;
    }
    else
    {
        can_id_config_tx = CAN_CONFIG_ID_MASTER;
        can_id_config_rx = CAN_CONFIG_ID_CLIENT;
        can_id_nmea_tx = CAN_NMEA_ID_MASTER;
        can_id_nmea_rx = CAN_NMEA_ID_CLIENT;
        can_id_keepalive_tx = CAN_KEEPALIVE_ID_MASTER;
        can_id_keepalive_rx = CAN_KEEPALIVE_ID_CLIENT;
    }
}


void CANbus::ConfigureIntf(int cfg)
{
}

// install/reinstall CAN driver in corresponding mode
void CANbus::driverInstall(twai_mode_t mode)
{
    if (_initialized)
    {
        driverUninstall();
    }
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(_tx_io, _rx_io, mode);
    ESP_LOGI(FNAME, "default alerts %X", g_config.alerts_enabled);
    // g_config.alerts_enabled = TWAI_ALERT_TX_FAILED | TWAI_ALERT_BUS_OFF | TWAI_ALERT_ABOVE_ERR_WARN | TWAI_ALERT_BUS_ERROR;
    g_config.alerts_enabled = TWAI_ALERT_ALL;
    if (_slope_support)
    {
        g_config.bus_off_io = _slope_ctrl;
    }
    g_config.rx_queue_len = 15; // 1.5x the need of one NMEA sentence
    g_config.tx_queue_len = 15;
    ESP_LOGI(FNAME, "my alerts %X", g_config.alerts_enabled);

    twai_timing_config_t t_config;
    _tx_timeout = 2; // 111usec/chunk -> 2msec
    if (can_speed.get() == CAN_SPEED_250KBIT)
    {
        ESP_LOGI(FNAME, "CAN rate 250KBit");
        t_config = TWAI_TIMING_CONFIG_250KBITS();
        _tx_timeout = 4; // 444usec/chunk -> 4msec
    }
    else if (can_speed.get() == CAN_SPEED_500KBIT)
    {
        ESP_LOGI(FNAME, "CAN rate 500KBit");
        t_config = TWAI_TIMING_CONFIG_500KBITS();
        _tx_timeout = 2; // 222usec/chunk -> 2msec
    }
    else if (can_speed.get() == CAN_SPEED_1MBIT)
    {
        ESP_LOGI(FNAME, "CAN rate 1MBit");
        t_config = TWAI_TIMING_CONFIG_1MBITS();
    }
    else
    {
        ESP_LOGI(FNAME, "CAN rate 1MBit for selftest");
        t_config = TWAI_TIMING_CONFIG_1MBITS();
    }
    // t_config.triple_sampling = true; // improved sampling incoming bits, no effect in test

    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    // {
    //     // Shift for standard-ID (11 Bit) by 21, for multi filter mode another one shifted by 5.
    //     //.acceptance_code = uint32_t(JumboCmd::MAGSTREAM_ID << 21) | uint32_t(JumboCmd::MAGCTRL_ID << 5),
    //     .acceptance_code = uint32_t(CANbus::CTRL_ID << 21), // | uint32_t(JumboCmd::MAGCTRL_ID << 5),
    //     // Disregard bits that are set on this mask
    //     // For single filter mode you most likely wnat to set any bit, but those 11 id bits
    //     //  -> also disregard the first two message bytes: ~(uint32_t(0x7ff << 21))
    //     // Here additionally the TEST_ID is disregarded (== let through(!))
    //     .acceptance_mask = ~(uint32_t(0x7ff << 21)),
    //     .single_filter = true
    // };
    // Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
    {
        ESP_LOGI(FNAME, "Driver installed OK, mode %d", mode);
    }
    else
    {
        ESP_LOGI(FNAME, "Failed to install driver");
        return;
    }

    // Start TWAI driver
    if (twai_start() == ESP_OK)
    {
        ESP_LOGI(FNAME, "Driver started");
        _initialized = true;
        if (_slope_support)
        {
            gpio_set_direction(_slope_ctrl, GPIO_MODE_OUTPUT);
        }
    }
    else
    {
        twai_driver_uninstall();
        ESP_LOGI(FNAME, "Failed to start driver");
    }
}

void CANbus::driverUninstall()
{
    if (_initialized)
    {
        _initialized = false;
        twai_stop();
        twai_driver_uninstall();
    }
}

void CANbus::recover()
{
    twai_status_info_t status_info;
    if (twai_get_status_info(&status_info) == ESP_OK)
    {
        if (status_info.state == TWAI_STATE_BUS_OFF)
        {
            // recovery is only possible in this state
            ESP_LOGW(FNAME, "CANbus recover");
            twai_initiate_recovery();
            delay(10);
            twai_start();
        }
    }
    _connected_timeout_xcv = 0;
    _connected_timeout_magsens = 0;
}

// begin CANbus, launch driver in normal mode after a selfTest
bool CANbus::begin()
{
    // Initialize configuration structures using macro initializers
    if (can_speed.get() == CAN_SPEED_OFF)
    {
        ESP_LOGI(FNAME, "CAN bus OFF");
        return false;
    }

    if ( ! _initialized && selfTest() )
    {
        ESP_LOGI(FNAME, "CANbus::begin");
        if ( selfTest() ) {
            driverInstall(TWAI_MODE_NORMAL);
            terminate_receiver = false;
            xTaskCreate(&canRxTask, "canRxTask", 4096, this, 15, &rxTask);
        } else {
            driverUninstall();
        }
    }
    return _initialized;
}

// terminate CANbus
void CANbus::stop()
{
    ESP_LOGI(FNAME, "CAN stop");

    // send terminate signals to tasks
    terminate_receiver = true;            // for receiver

    DeleteAllDataLinks();
}

bool CANbus::selfTest()
{
    ESP_LOGI(FNAME, "CAN bus selftest");

    // Pretend slope control off and probe the reaction on GPIO 2 here
    _slope_support = false;
    gpio_set_direction(_slope_ctrl, GPIO_MODE_OUTPUT);
    // in case of GPIO 2 wired to CAN this would inhibit sending and cause a failing test
    gpio_set_level(_slope_ctrl, 1);

    driverInstall(TWAI_MODE_NO_ACK);
    bool res = false;
    int id = 0x100;
    for (int slope = 0; slope < 2; slope++)
    {
        ESP_LOGI(FNAME, "CAN slope support %s.", _slope_support ? "on" : "off");
        twai_clear_receive_queue();
        for (int i = 0; i < 3; i++)
        { // repeat test 3x
            char tx[10] = {"1827364"};
            int len = strlen(tx);
            ESP_LOGI(FNAME, "strlen %d", len);
            twai_clear_receive_queue(); // there might be data from a remote device
            if ( ! sendData(id, tx, len, 1) ) {
                ESP_LOGW(FNAME, "CAN bus selftest TX FAILED");
            }
            twai_message_t rx;
            if ( (ESP_OK == twai_receive(&rx, pdMS_TO_TICKS(20))) 
                && (rx.identifier == id) 
                && (rx.data_length_code == len) 
                && (memcmp(rx.data, tx, len) == 0) )
            {
                ESP_LOGI(FNAME, "RX CAN bus OKAY");
                res = true;
                break;
            }
            else
            {
                std::string msg((char*)rx.data, rx.data_length_code);
                ESP_LOGW(FNAME, "CAN bus selftest RX call FAILED bytes:%d rxid:%x rxmsg:%s", rx.data_length_code, rx.identifier, msg.c_str());
                twai_clear_receive_queue();
            }
        }
        if (res) {
            break;
        }

        _slope_support = true;
        gpio_set_level(_slope_ctrl, 0);
    }

    driverUninstall();
    ESP_LOGW(FNAME, "CAN bus selftest TX/RX %s", res ? "Ok" : "failed");
    return res;
}

int CANbus::Send(const char *cptr, int &len, int port)
{
    constexpr int chunk = 8;

    int rem = len;
    while (rem > 0)
    {
        int dlen = std::min(chunk, rem);
        if ( ! sendData(port, cptr, dlen) ) {
            break;
        }
        cptr += dlen;
        rem -= dlen;
    }

    if ( rem == 0 ) {
        return 0;
    }
    else {
        len = len - rem; // buffered bytes
        return (rem/chunk + 1) * _tx_timeout; // ETA to wait for next trial
    }
}

// Send, handle alerts, do max 3 retries
bool CANbus::sendData(int id, const char *msg, int length, int self)
{
    if (!_initialized)
    {
        ESP_LOGI(FNAME, "CANbus not ready initialized");
        return false;
    }

    // build message
    twai_message_t message;
    message.identifier = id;
    message.self = self;
    message.data_length_code = length;
    for (int i = 0; i < length; i++)
    {
        message.data[i] = msg[i];
    }
    // ESP_LOGI(FNAME,"TX CAN bus message id:%x, bytes:%d, data:%s, self:%d", message.identifier, message.data_length_code, message.data, message.self );
    // ESP_LOGI(FNAME,"send CAN chunk len %d msg: %s ", length, msg );
    // ESP_LOG_BUFFER_HEXDUMP(FNAME, msg, length, ESP_LOG_INFO);

    // Queue message for transmission
    uint32_t alerts = 0;
    int retry = 3;
    esp_err_t res = ESP_OK;
    while ( retry-- > 0 )
    {
        res = twai_transmit(&message, 0);
        if ( res == ESP_OK ) {
            break;
        }
        ESP_LOGE(FNAME, "Transmit error: %s", esp_err_to_name(res));
        if (res == ESP_ERR_TIMEOUT) {
            ESP_LOGW(FNAME, "Transmit timeout. Message dropped.");
        }
        twai_read_alerts(&alerts, pdMS_TO_TICKS(_tx_timeout));
        ESP_LOGW(FNAME, "Tx chunk failed alerts 0x%x", alerts);
    }
    if ( alerts != 0 )
    {
        if ( alerts & TWAI_ALERT_BUS_OFF ) {
            ESP_LOGE(FNAME, "BUS OFF alert");
            do_recover = true;
        }
        if (alerts & TWAI_ALERT_RX_QUEUE_FULL) {
            ESP_LOGW(FNAME, "RX QUEUE FULL alert");
        }
        if (alerts & TWAI_ALERT_TX_FAILED) {
            ESP_LOGW(FNAME, "TX_FAILED alert");
        }
        return false;
    }
    return true;
}
