#include "SerialLine.h"

#include "logdefnone.h"

#include "comm/CircularCharBuffer.h"
#include "SetupNG.h"

#include <soc/uart_reg.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

constexpr int BUF_LEN = 128;
constexpr int UARTFIFO_LEN = 512;
constexpr int UARTEVENTQ_LEN = 10;

const int baud[] = { 0, 4800, 9600, 19200, 38400, 57600, 115200 };


t_serial_cfg sm_serial_config[] = {
		{ SM_FLARM,       BAUD_19200, true, false, true },
		{ SM_RADIO,       BAUD_9600,  true, false, true },
		{ SM_XCTNAV_S3,   BAUD_19200, true, false, true },
		{ SM_OPENVARIO,   BAUD_19200, true, false, true },
		{ SM_XCFLARMVIEW, BAUD_57600, true, false, true }
};

static const char *MEMOS[3] = { "S0", "S1", "S2" };

// connection to nvs setup variables
static const t_serial_nvs_setup uart_setup[3] = {
	{ 0,0,0,0 },
	{ &serial1_speed, &serial1_tx_inverted, &serial1_pins_twisted, &serial1_tx_enable },
	{ &serial2_speed, &serial2_tx_inverted, &serial2_pins_twisted, &serial2_tx_enable }
};

static uart_event_t send_trigger = {UART_DATA_BREAK, 0, false};

// Thread body to handle UART events
void IRAM_ATTR uartTask(SerialLine* s)
{
	uart_event_t event;
	char rx_buf[BUF_LEN+1];
	CircularCharBuffer& tx_buf = s->_tx_buf;
	uart_port_t un = s->uart_nr;

	ESP_LOGI(FNAME, "IoSerTask UART%d", un);

	while (true)
	{
		// sleep until the UART gives us something to do
		xQueueReceive((QueueHandle_t)s->_event_queue, &event, portMAX_DELAY);

		if (event.type == -1)
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
				if ( count > 0 ) {
					rx_buf[count] = '\0';
					// ESP_LOGI(FNAME, "Data received from UART%d: %dc", un, count);
					auto dlit = s->_dlink.begin();
					if ( dlit != s->_dlink.end() ) {
						dlit->second->process(rx_buf, count); // SX interfaces do have only one data link
					}
				}
				break;
			}
			case UART_BREAK:
				ESP_LOGI(FNAME, "Break detected on UART%d", un);
				break;
			case UART_BUFFER_FULL:
				ESP_LOGI(FNAME, "Full detected on UART%d", un);
				break;
			case UART_FIFO_OVF:
				ESP_LOGI(FNAME, "OVF detected on UART%d", un);
				break;
			case UART_DATA_BREAK:
			{
				int len;
				char* data = tx_buf.next_block(len);
				while ( len > 0 ) {
					int count = uart_tx_chars(un, data, len);
					if ( count <= 0 ) {
						break;
					}
					tx_buf.pop(count);
					data = tx_buf.next_block(len);
				}
				// ESP_LOGI(FNAME, "DataBreak detected on UART%d ", un);
				break;
			}
			default:
				ESP_LOGI(FNAME, "UART%d caught event %d", un, event.type);
				break;
		}
	}
}


SerialLine::SerialLine(uart_port_t uart, gpio_num_t rx, gpio_num_t tx ) :
	InterfaceCtrl(true),
	uart_nr(uart),
	tx_gpio(tx),
	rx_gpio(rx),
	_intfid(InterfaceId(S0_RS232+uart)),
	_id_memo(MEMOS[uart]),
	_setup(uart_setup[uart]),
	_tx_buf(*(new CircularCharBuffer(2*BUF_LEN)))
{
	loadSetupDefaults();
	ESP_LOGI(FNAME,"CONSTR. UART:%d (new) RX:%d TX:%d baud:%d pol:%d swap:%d tx:%d", uart_nr, rx_gpio, tx_gpio, baud[cfg.baud], cfg.polarity, cfg.pin_swp, cfg.tx_ena );
}

SerialLine::~SerialLine()
{
	if ( uart_is_driver_installed(uart_nr) ) {
		stop();
	}
	if ( _iotask ) {
		vTaskDelete(_iotask);
	}
	delete &_tx_buf;
}

// -1: OFF; 0: as is; 1,2,3..: load profile
void SerialLine::ConfigureIntf(int cfg)
{
	if ( cfg == -1 ) {
		stop();
		return;
	}
	if ( cfg > 0 ) {
		loadProfile( e_profile(cfg-1) );
	}
	
	ESP_LOGI(FNAME,"configure UART:%d", uart_nr );
	if ( uart_is_driver_installed(uart_nr) ) { stop(); }
	applyBaud();
	applyPins();
	applyLineInverse();
	start();

}

// returns 0: sucess; or retry wait time in msec 
int SerialLine::Send(const char *msg, int len, int port)
{
	ESP_LOGD(FNAME,"Send UART%d, len %d/%d", uart_nr, len, _tx_buf.size());
	int av_space = _tx_buf.available_space();
	bool buffered = false;
	if( len < av_space ) {
		_tx_buf.push_block(msg, len);
		xQueueSend((QueueHandle_t)_event_queue, (void *)&send_trigger, (TickType_t)0);
		buffered = true;
	}

	if ( buffered ) {
		return 0;
	}
	// else
	int dur = rint( (len-av_space)*12000.0/baud[cfg.baud] + 2 );  // [msec] ; byte/sec = baudrate / 12 bits/char
	ESP_LOGI(FNAME,"Send UART%d, ETA for free buf in %d msec", uart_nr, dur);
	return dur;
}


void SerialLine::applyBaud()
{
	ESP_LOGI(FNAME,"setBaud UART%d baud:%d", uart_nr, baud[cfg.baud]);
	// uart_set_baudrate(uart_nr, baud[cfg.baud]);
	uart_config_t uart_config = {
		.baud_rate = baud[cfg.baud],
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.rx_flow_ctrl_thresh = 0,
		.source_clk = UART_SCLK_APB
		};

	uart_param_config(uart_nr, &uart_config);
}

// configures ESP32 matrix, here also the line polartity (inverting) is handled
void SerialLine::applyPins()
{
	ESP_LOGI(FNAME,"setPinSwap UART%d swap:%d (0:NORM, 1:SWAPPED), rx:%d, tx:%d,", uart_nr, cfg.pin_swp, rx_gpio, tx_gpio );

	gpio_num_t tx_pin = cfg.pin_swp ? rx_gpio : tx_gpio;
	gpio_num_t rx_pin = cfg.pin_swp ? tx_gpio : rx_gpio;
	uart_set_pin(uart_nr, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	
	if( ! cfg.tx_ena ) {
		// Overrule uart module and switch tx off if configured
		gpio_set_direction(tx_pin, GPIO_MODE_INPUT);
		gpio_pullup_dis( tx_pin );
	}
}

void SerialLine::applyLineInverse(){
	ESP_LOGI(FNAME, "setLineInverse UART%d: %d (0: Norm, 1:Invers)", uart_nr, cfg.polarity);
	if( cfg.polarity ) {
		uart_set_line_inverse(uart_nr, UART_SIGNAL_RXD_INV | UART_SIGNAL_TXD_INV);
	}
	else {
		uart_set_line_inverse(uart_nr, 0);
	}
}

void SerialLine::flush(){
	uart_flush(uart_nr);
}

// set defaults according to given profile
void SerialLine::loadProfile(e_profile profile)
{
	ESP_LOGI(FNAME,"loadProfile: %d (1: Flarm, 2:Radio)", profile );
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
}

void SerialLine::start()
{
	if ( ! uart_is_driver_installed(uart_nr) )
	{
		ESP_LOGI(FNAME, "start UART%d baud:%d, rx:%d, tx:%d, TTL:%d ", uart_nr, cfg.baud, rx_gpio, tx_gpio, cfg.polarity );

		// Setup UART buffered IO with event queue
		// Install UART driver creating an event queue here
		uart_driver_install(uart_nr, UARTFIFO_LEN, UARTFIFO_LEN, UARTEVENTQ_LEN, (QueueHandle_t*)&_event_queue, 0);

		// Set UART pattern detect function
		uart_enable_pattern_det_baud_intr(uart_nr, '\n', 1, 5, 0, 0);

		if ( ! _iotask ) {
			char tmp[] = "S1rx";
			tmp[1] = '0'+uart_nr;
			xTaskCreate((TaskFunction_t)uartTask, tmp, 3000, this, 80, &_iotask);
		}
		uart_intr_config_t intr_config = {
			.intr_enable_mask = UART_RXFIFO_TOUT_INT_ENA | UART_RXFIFO_OVF_INT_ENA | UART_AT_CMD_CHAR_DET_INT_ENA | UART_RXFIFO_FULL_INT_ENA 
								| UART_TXFIFO_EMPTY_INT_ENA | UART_TX_BRK_IDLE_DONE_INT_ENA | UART_TX_BRK_DONE_INT_ENA | UART_TX_DONE_INT_ENA,
								// UART_TXFIFO_EMPTY_INT_ENA | UART_TX_DONE_INT_ENA
			.rx_timeout_thresh = 9,
			.txfifo_empty_intr_thresh = 9,
			.rxfifo_full_thresh = 60
		};
		uart_intr_config(uart_nr, &intr_config);
		// uart_enable_tx_intr(uart_nr, 1, 2);

	}
}

void SerialLine::stop()
{
	ESP_LOGI(FNAME,"UART %d end(), pins %d %d", uart_nr, tx_gpio, rx_gpio);
	uart_driver_delete(uart_nr);
}


