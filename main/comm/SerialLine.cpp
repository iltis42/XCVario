#include "SerialLine.h"

#include "setup/SetupNG.h"
#include "logdefnone.h"

#include <soc/uart_reg.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

constexpr int BUF_LEN = 128;
constexpr int UARTRXFIFO_LEN = 256;
constexpr int UARTTXFIFO_LEN = 1024;
constexpr int UARTEVENTQ_LEN = 10;

static constexpr std::array<int, 6> baud_rate_table = { 4800, 9600, 19200, 38400, 57600, 115200 };

// enumeration is: SM_FLARM, SM_RADIO, SM_XCTNAV_S3, SM_OPENVARIO
static constexpr std::array<t_serial_cfg, NR_OF_SER_PROFILES>  sm_serial_config = {{
		// baud,      polarity,  pinswp, tx enable
		{ BAUD_19200, RS232_TTL, false, true },
		{ BAUD_9600,  RS232_TTL, false, true },
		{ BAUD_19200, RS232_TTL, false, true },
		{ BAUD_19200, RS232_TTL, false, true }
}};

static constexpr std::array<std::string_view, 3> MEMOS = { "S0", "S1", "S2" };

// connection to nvs setup variables
static constexpr std::array<t_serial_nvs_setup, 3> uart_setup = {{
	{ nullptr, nullptr, nullptr, nullptr },
	{ &serial1_speed, &serial1_ttl_signals, &serial1_pin_swap, &serial1_tx_enable },
	{ &serial2_speed, &serial2_ttl_signals, &serial2_pin_swap, &serial2_tx_enable }
}};

static constexpr uart_event_t stop_trigger = {UART_EVENT_MAX, 0, false};

// Thread body to handle UART events
void uartTask(SerialLine* s)
{
	uart_event_t event;
	char rx_buf[BUF_LEN+1];
	uart_port_t un = s->uart_nr;

	ESP_LOGI(FNAME, "IoSerTask UART%d", un);

	while (true)
	{
		// sleep until the UART gives us something to do
		BaseType_t ret = xQueueReceive((QueueHandle_t)s->_event_queue, &event, pdMS_TO_TICKS(10000));
		// fetch the data link
		xSemaphoreTake(s->_dlink_mutex, portMAX_DELAY);
		auto dlit = s->_dlink.begin();
		bool valid = dlit != s->_dlink.end();
		xSemaphoreGive(s->_dlink_mutex);
		if ( ret != pdTRUE) {
			// time-out, propagate as empty message
			if ( valid ) {
				dlit->second->process(nullptr, 0);
			}
			continue;
		}

		if (event.type == UART_EVENT_MAX)
		{
			ESP_LOGI(FNAME, "Terminate UART%d task", un);
			break;
		} // termination signal

		switch (event.type) {
			case UART_PATTERN_DET:
				// ESP_LOGI(FNAME, "Pattern detected on UART%d", un);
			case UART_DATA:
			{
				int count = uart_read_bytes(un, rx_buf, BUF_LEN, 0);
				if ( count > 0 && valid ) {
					rx_buf[count] = '\0';
					// ESP_LOGI(FNAME, "Data received from UART%d: %dc", un, count);
					dlit->second->process(rx_buf, count); // SX interfaces do have only one data link
				}
				break;
			}
			case UART_BREAK:
				s->break_count++;
				// ESP_LOGI(FNAME, "Break detected on UART%d", un);
				break;
			case UART_BUFFER_FULL:
				// ESP_LOGI(FNAME, "Full detected on UART%d", un);
				break;
			case UART_FIFO_OVF:
				// ESP_LOGI(FNAME, "OVF detected on UART%d", un);
				break;
			case UART_DATA_BREAK:
				// ESP_LOGI(FNAME, "DataBreak detected on UART%d ", un);
				break;
			default:
				// ESP_LOGI(FNAME, "UART%d caught event %d", un, event.type);
				break;
		}
	}
	vTaskDelete(nullptr);
}


SerialLine::SerialLine(uart_port_t uart, gpio_num_t rx, gpio_num_t tx ) :
	InterfaceCtrl(true),
	cfg(0),
	uart_nr(uart),
	tx_gpio(tx),
	rx_gpio(rx),
	_intfid(InterfaceId((int)S0_RS232+(int)uart)),
	_id_memo(MEMOS[uart].data()),
	_setup(uart_setup[uart])
{
	loadSetupDefaults();
	ESP_LOGI(FNAME,"CONSTR. UART:%d (new) RX:%d TX:%d baud:%d pol:%d swap:%d tx:%d", uart_nr, rx_gpio, tx_gpio, baud_rate_table[cfg.baud], cfg.polarity, cfg.pin_swp, cfg.tx_ena );
}

SerialLine::~SerialLine()
{
	stop();
}

// 0,1,2,3..: load profile
void SerialLine::ConfigureIntf(int profile)
{
	ESP_LOGI(FNAME,"configure UART S%d: %d", uart_nr, profile );
	if ( ! uart_is_driver_installed(uart_nr) ) {
		if ( profile >= 0 ) {
			loadProfile( e_profile(profile) );
		}
		start();
	}
	else {
		if ( profile >= 0 && profile < NR_OF_SER_PROFILES ) {
			const t_serial_cfg *newcfg = &sm_serial_config[profile];
			if ( cfg.baud != newcfg->baud ) {
				cfg.baud = newcfg->baud;
				applyBaud();
			}
			if ( cfg.polarity != newcfg->polarity ) {
				cfg.polarity = newcfg->polarity;
				applyLineInverse();
			}
			if ( cfg.tx_ena != newcfg->tx_ena
				|| cfg.pin_swp != newcfg->pin_swp ) {
				cfg.tx_ena = newcfg->tx_ena;
				cfg.pin_swp = newcfg->pin_swp;
				applyPins();
			}
		}
	}
}

// returns 0: sucess; or retry wait time in msec
int SerialLine::Send(const char *msg, int &len, int port)
{
	int count = uart_write_bytes(uart_nr, msg, len);
	ESP_LOGI(FNAME,"Send UART%d, len %d/%d", uart_nr, len, count);
	if ( count == len ) {
		return 0;
	}
	count = (count>=0) ? count : 0; // enforce count >= 0
	int dur = rint( (len-count)*12000.0/baud_rate_table[cfg.baud] + 1 );  // [msec] ; byte/sec = baudrate / 12 bits/char
	ESP_LOGI(FNAME,"Send UART%d, ETA for free buf in %d msec", uart_nr, dur);
	len = count; // buffered chars
	return dur;  // rough ETA for telegram of len bytes
}


void SerialLine::applyBaud()
{
	ESP_LOGI(FNAME,"setBaud UART%d baud idx %d: %d", uart_nr, cfg.baud, baud_rate_table[cfg.baud]);
	if ( uart_is_driver_installed(uart_nr) ) {
		uart_flush(uart_nr);
	}
	uart_set_baudrate(uart_nr, baud_rate_table[cfg.baud]);
}

// configures ESP32 matrix, here also the line polartity (inverting) is handled
void SerialLine::applyPins()
{
	ESP_LOGI(FNAME,"setPinSwap UART%d swap:%d (0:NORM, 1:SWAPPED), rx:%d, tx:%d,", uart_nr, cfg.pin_swp, rx_gpio, tx_gpio );

	gpio_num_t tx_pin = cfg.pin_swp ? rx_gpio : tx_gpio;
	gpio_num_t rx_pin = cfg.pin_swp ? tx_gpio : rx_gpio;

	if ( uart_is_driver_installed(uart_nr) ) {
		// pause it
		uart_flush(uart_nr);
		uart_disable_rx_intr(uart_nr);
	}
	uart_set_pin(uart_nr, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	if ( uart_is_driver_installed(uart_nr) ) {
		// and restart it
		uart_enable_rx_intr(uart_nr);
	}

	if( ! cfg.tx_ena ) {
		// Overrule uart module and switch tx off if configured
		gpio_set_direction(tx_pin, GPIO_MODE_INPUT);
		gpio_pullup_dis( tx_pin );
	}
}

void SerialLine::applyLineInverse()
{
	ESP_LOGI(FNAME, "setLineInverse UART%d: %d (0: Norm, 1:Invers)", uart_nr, cfg.polarity);
	if ( uart_is_driver_installed(uart_nr) ) {
		uart_flush(uart_nr);
	}
	if( cfg.polarity ) {
		uart_set_line_inverse(uart_nr, UART_SIGNAL_RXD_INV | UART_SIGNAL_TXD_INV);
		gpio_pulldown_dis(tx_gpio);
		gpio_pulldown_dis(rx_gpio);
		gpio_pullup_en(tx_gpio);
		gpio_pullup_en(rx_gpio);

	}
	else {
		uart_set_line_inverse(uart_nr, 0);
		gpio_pulldown_en(tx_gpio);
		gpio_pulldown_en(rx_gpio);
		gpio_pullup_dis(tx_gpio);
		gpio_pullup_dis(rx_gpio);
	}
}

void SerialLine::flush(){
	uart_flush(uart_nr);
}

// set defaults according to given profile
void SerialLine::loadProfile(e_profile profile)
{
	ESP_LOGI(FNAME,"loadProfile: %d (0: Flarm, 1:Radio)", profile );
	cfg = sm_serial_config[profile];
	_setup.baud->set( cfg.baud );
	_setup.polarity->set( cfg.polarity );
	_setup.tx_ena->set( cfg.tx_ena );
	_setup.pin_swp->set( cfg.pin_swp );
}
// load setup defaults
void SerialLine::loadSetupDefaults()
{
	cfg.baud     = (e_baud)_setup.baud->get();
	cfg.polarity = _setup.polarity->get();
	cfg.tx_ena   = _setup.tx_ena->get();
	cfg.pin_swp  = _setup.pin_swp->get();
	ESP_LOGI(FNAME,"load default: %d/%d/%d/%d", cfg.baud, cfg.polarity, cfg.tx_ena, cfg.pin_swp);
}

void SerialLine::start()
{
	if ( ! uart_is_driver_installed(uart_nr) )
	{
		ESP_LOGI(FNAME, "start UART%d baud:%d, rx:%d, tx:%d, TTL:%d ", uart_nr, cfg.baud, rx_gpio, tx_gpio, cfg.polarity );

		// all the details and baudrate
		uart_config_t uart_config = {
			.baud_rate = baud_rate_table[cfg.baud],
			.data_bits = UART_DATA_8_BITS,
			.parity = UART_PARITY_DISABLE,
			.stop_bits = UART_STOP_BITS_1,
			.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
			.rx_flow_ctrl_thresh = 0,
			.source_clk = UART_SCLK_DEFAULT,
			.flags = {
				.allow_pd = 0,
				.backup_before_sleep = 0 },
			};
		uart_param_config(uart_nr, &uart_config);

		// line logic levels and pins
		applyLineInverse();
		applyPins();

		// Setup UART buffered IO with event queue
		// Install UART driver creating an event queue here
		uart_driver_install(uart_nr, UARTRXFIFO_LEN, UARTTXFIFO_LEN, UARTEVENTQ_LEN, (QueueHandle_t*)&_event_queue, ESP_INTR_FLAG_IRAM);

		// Set UART pattern detect function
		uart_enable_pattern_det_baud_intr(uart_nr, '\n', 1, 5, 0, 0);

		if ( ! _iotask ) {
			char tmp[] = "S1rx";
			tmp[1] = '0'+uart_nr;
			xTaskCreate((TaskFunction_t)uartTask, tmp, 4000, this, 22, (tskTaskControlBlock**)&_iotask);
		}
		uart_intr_config_t intr_config = {
			.intr_enable_mask = UART_RXFIFO_TOUT_INT_ENA | UART_RXFIFO_OVF_INT_ENA | UART_AT_CMD_CHAR_DET_INT_ENA | UART_RXFIFO_FULL_INT_ENA
								| UART_TXFIFO_EMPTY_INT_ENA,
								// | UART_TXFIFO_EMPTY_INT_ENA | UART_TX_BRK_IDLE_DONE_INT_ENA | UART_TX_BRK_DONE_INT_ENA | UART_TX_DONE_INT_ENA,
			.rx_timeout_thresh = 9,
			.txfifo_empty_intr_thresh = 0,
			.rxfifo_full_thresh = 60
		};
		uart_intr_config(uart_nr, &intr_config);

		_functional = true; // self test passed (!)
	}
}

void SerialLine::stop()
{
	if ( _iotask ) {
		// Last chance to use the queue to terminate the receiver task
		xQueueSend((QueueHandle_t)_event_queue, (void *)&stop_trigger, (TickType_t)0);
	}
	_iotask = nullptr;

	if ( uart_is_driver_installed(uart_nr) ) {
		ESP_LOGI(FNAME,"UART %d end(), pins %d %d", uart_nr, tx_gpio, rx_gpio);
		uart_flush(uart_nr);
		uart_driver_delete(uart_nr);
	}
}
