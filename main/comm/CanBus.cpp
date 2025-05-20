
#include "CanBus.h"

#include "Messages.h"
#include "DeviceMgr.h"
#include "DataLink.h"
#include "sensor.h"
#include "logdef.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/gpio.h>
#include <esp_err.h>


/*
 *  Connect two XCVario with a fixed message ID
 *  Connect other peripherals to the XCVario
 *  Maybe use a breakout bos to furthe connect serial devices and tunnel protocols
 */

// used CAN Id's
static constexpr int CANTEST_ID = CAN_REG_PORT+1;

static TaskHandle_t rxTask = nullptr;
CANbus *CAN = 0;
static bool terminate_receiver = false;
static bool do_recover = false;

// CAN receiver task
void canRxTask(void *arg)
{
    CANbus *can = static_cast<CANbus *>(arg);
    unsigned int tick = 0;
    bool to_once = true;

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
            ESP_LOGD(FNAME, "CAN RX NMEA chunk, id:0x%x, len:%d msg: %s", (unsigned int)rx.identifier, rx.data_length_code, msg.c_str());
            xSemaphoreTake(can->_dlink_mutex, portMAX_DELAY);
            auto dl = can->_dlink.find(rx.identifier);
            bool valid = dl != can->_dlink.end();
            xSemaphoreGive(can->_dlink_mutex);
            if ( valid ) {
                dl->second->process(msg.data(), msg.size());
                to_once = true;
            }
        }
        else
        {
            // protocol state machine may want to react on no traffic
            if ( to_once ) { 
                // not yet used
                // xSemaphoreTake(_dlink_mutex, portMAX_DELAY);
            	// for (auto &dl : can->_dlink ) {
                // 	dl.second->process(nullptr, 0);
                // }
                // xSemaphoreGive(_dlink_mutex);
                to_once = false;
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
}

CANbus::CANbus()
{

    _tx_io = GPIO_NUM_26;
    _rx_io = GPIO_NUM_33;
    _slope_ctrl = GPIO_NUM_2;
}

CANbus *CANbus::createCAN()
{
    if ( ! CAN ) {
        CAN = new CANbus();
    }
    return CAN;
}

void CANbus::ConfigureIntf(int cfg)
{
    if (cfg == 1) {
        // data rate got changed
        terminate_receiver = true;
        vTaskDelay(pdMS_TO_TICKS(600)); // wait for the task to finish
    }
    begin();
}

// install/reinstall CAN driver in corresponding mode
void CANbus::driverInstall(twai_mode_t mode)
{
    if (_initialized)
    {
        driverUninstall();
    }
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(_tx_io, _rx_io, mode);
    ESP_LOGI(FNAME, "default alerts %X", (unsigned int)g_config.alerts_enabled);
    // g_config.alerts_enabled = TWAI_ALERT_TX_FAILED | TWAI_ALERT_BUS_OFF | TWAI_ALERT_ABOVE_ERR_WARN | TWAI_ALERT_BUS_ERROR;
    g_config.alerts_enabled = TWAI_ALERT_ALL;
    if (_slope_support)
    {
    	ESP_LOGI(FNAME, "_slope_support is TRUE");
        g_config.bus_off_io = _slope_ctrl;
    }
    g_config.rx_queue_len = 15; // 1.5x the need of one NMEA sentence
    g_config.tx_queue_len = 15;
    ESP_LOGI(FNAME, "my alerts %X", (unsigned int)g_config.alerts_enabled);

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
        vTaskDelay(pdMS_TO_TICKS(10));
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
}

// begin CANbus, launch driver in normal mode after a selfTest
bool CANbus::begin()
{
    ESP_LOGI(FNAME, "CAN begin");

    if ( ! _initialized )
    {
        driverInstall(TWAI_MODE_NORMAL);
        terminate_receiver = false;
        xTaskCreate(&canRxTask, "canRxTask", 4096, this, 22, &rxTask);
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
    _slope_support = true; // this controles the driver installation
    driverInstall(TWAI_MODE_NO_ACK);
    bool res = false;
    int id = CANTEST_ID;
    delay(100);
    _slope_support = false;
    for (int gpio_level = 0; gpio_level <=1; gpio_level++)
    {
        ESP_LOGI(FNAME,"slope pin level %s.", (gpio_level==0) ? "low" : "high");
        // in case of GPIO 2 wired to CAN this would inhibit sending and cause a failing test
        gpio_set_level(_slope_ctrl, gpio_level);
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
            	if( gpio_level == 1 ){
            		_slope_support = true;
            		ESP_LOGI(FNAME, "CAN HW connected to slope !");
            	}
                std::string msg((char*)rx.data, rx.data_length_code);
                ESP_LOGW(FNAME, "RX FAILED:  bytes:%d rxid:%x rxmsg:%s", rx.data_length_code, (unsigned int)rx.identifier, msg.c_str());
                twai_clear_receive_queue();
            }
        }
    }
    driverUninstall();
    ESP_LOGW(FNAME, "Final Result: CAN bus selftest %s, slope support: %d", res ? "OKAY" : "FAILED", _slope_support );
    _functional = res;
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
        res = twai_transmit(&message, pdMS_TO_TICKS(_tx_timeout));
        if ( res == ESP_OK ) {
            break;
        }
        ESP_LOGE(FNAME, "Transmit error: %s", esp_err_to_name(res));
        if (res == ESP_ERR_TIMEOUT) {
            ESP_LOGW(FNAME, "Transmit timeout. Message dropped.");
        }
        twai_read_alerts(&alerts, pdMS_TO_TICKS(_tx_timeout));
        ESP_LOGW(FNAME, "Tx chunk failed alerts 0x%x", (unsigned int)alerts);
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
    return (res == ESP_OK);
}
