#include "SerialLine.h"
#include <esp_log.h>
#include "Serial.h"
#include "esp32-hal-uart.h"

t_serial_cfg sm_serial_config[] = {
		{ SM_FLARM,       BAUD_19200, RS232_TTL, RJ45_3TX_4RX, SM_MASTER },
		{ SM_RADIO,       BAUD_9600,  RS232_TTL, RJ45_3TX_4RX, SM_MASTER },
		{ SM_XCTNAV_S3,   BAUD_19200, RS232_TTL, RJ45_3RX_4TX, SM_MASTER },
		{ SM_OPENVARIO,   BAUD_19200, RS232_TTL, RJ45_3RX_4TX, SM_MASTER },
		{ SM_XCFLARMVIEW, BAUD_57600, RS232_TTL, RJ45_3RX_4TX, SM_MASTER }
};

static const char *MEMOS[3] = { "S0", "S1", "S2" };

gpio_num_t SerialLine::prior_tx_gpio[3] = { GPIO_NUM_36,GPIO_NUM_36,GPIO_NUM_36 };
gpio_num_t SerialLine::prior_rx_gpio[3] = { GPIO_NUM_36,GPIO_NUM_36,GPIO_NUM_36 };

static const t_serial_setup uart_setup[3] = {
	{ 0,0,0,0 },
	{ &serial1_speed, &serial1_tx_inverted, &serial1_tx_enable, &serial1_pins_twisted },
	{ &serial2_speed, &serial2_tx_inverted, &serial2_tx_enable, &serial2_pins_twisted }
};

static RingBufCPP<SString>* tx_queues[3] = { 0, &s1_tx_q, &s2_tx_q };
static const uint8_t tx_requests[3] = { 0, TX1_REQ, TX2_REQ };

SerialLine::SerialLine(uart_port_t uart, gpio_num_t rx_pin, gpio_num_t tx_pin ) :
	InterfaceCtrl(true),
	_intfid(InterfaceId(S0_RS232+uart)),
	_id_memo(MEMOS[uart]),
	_setup(uart_setup[uart]),
	tx_q(tx_queues[uart]),
	tx_req(tx_requests[uart])
{
	uart_nr = uart;
	_uart = 0;
	rx_pin_norm = rx_pin;
	tx_pin_norm = tx_pin;
	cfg.baud = (e_baud)_setup.baud->get();        // load settings from NVS
	cfg.pol = (e_polarity)_setup.polarity->get();
	cfg.tx = (e_tx)_setup.tx_ena->get();
	cfg.pin = (e_pin)_setup.pin->get();
	tx_q->setSize( 5 );
	setupGPIOPins();
	uartBegin();
	ESP_LOGI(FNAME,"CONSTR. UART:%d (new) RX:%d TX:%d baud:%d pol:%d swap:%d tx:%d", uart_nr, rx_gpio, tx_gpio, baud[cfg.baud], cfg.pol, cfg.pin, cfg.tx );
};

void SerialLine::ConfigureIntf(int cfg){
	loadProfile( (_e_profile)cfg );
	configure();
};

int SerialLine::Send(const char *msg, int len, int port){
	ESP_LOGI(FNAME,"Send(): UART: %d, msg: %s", uart_nr, msg );
	if( !tx_q->isFull() ) {
		tx_q->add( SString(msg) );
		Serial::setRxTxNotifier( tx_req );
		ESP_LOGI(FNAME,"Send(): add to Q OK");
		return 0;
	}
	int dur = rint( len*12000.0/baud[cfg.baud] );  // 8 bits/byte * 1.5 = 12 bits * 1000 for mS
	if( dur )
		ESP_LOGI(FNAME,"Send(): UART: %d mS P=%p, ETA for free buf in %d mS, qsize=%d", uart_nr, tx_q, dur, tx_q->numElements() );
	return dur; // heuristic now here
};

size_t SerialLine::read(uint8_t *buffer, size_t size)
{
	size_t avail = available();
	if (size < avail) {
		avail = size;
	}
	size_t count = 0;
	while(count < avail) {
		*buffer++ = uartRead(_uart);
		count++;
	}
	return count;
}


void SerialLine::setBaud(e_baud abaud, bool coldstart ){
	ESP_LOGI(FNAME,"setBaud: UART: %d baud:%d", uart_nr, baud[abaud]);
	cfg.baud = abaud;
	// if( abaud == 0 ){
	//	Serial::taskStop( uart_nr );
	//	return;
	//}
	uart_set_baudrate(uart_nr, baud[abaud] );
	// if( !Serial::taskStarted( uart_nr ) ){
	// 	Serial::taskStart(uart_nr);
	// }
};

void SerialLine::setLineInverse(e_polarity apol){
	ESP_LOGI(FNAME,"setLineInverse: UART: %d %d (0: Norm, 1:Invers)", uart_nr, apol);
	cfg.pol = apol;
	bool invert = false;
	if( cfg.pol == RS232_TTL )
		invert = true;  // RS232 TTL is inverted here
	uartSetRxInvert(_uart, invert );
	uartSetTxInvert(_uart, invert );
};

void SerialLine::setSlaveRole( e_tx tx ){
	ESP_LOGI(FNAME,"setRole: UART:%d (0: Slave, 1:Master)", tx);
	cfg.tx = tx;
	setPinSwap( cfg.pin );
}

void SerialLine::flush(){
	uartFlush(_uart);
}

void SerialLine::setRxNotifier( EventGroupHandle_t egh )
{
	ESP_LOGI( FNAME, "setRxNotifier" );
    uartRxEventHandler( egh );
}

void SerialLine::enableRxInterrupt()
{
	ESP_LOGI( FNAME, "Now enableRxInterrupt S%d", uart_nr );
	// Make sure that the previous interrupt_info is not used anymore
	if( _uart ){
		uartDisableInterrupt( _uart );
		uartEnableRxInterrupt( _uart );
	}
}

int SerialLine::number() const
{
	return uart_nr;
}

int SerialLine::available(void)
{
    return uartAvailable(_uart);
}

int SerialLine::availableForWrite()
{
	return uartAvailableForWrite(_uart);
}

size_t SerialLine::write(const char * buffer, size_t size)
{
	uartWriteBuf(_uart, (uint8_t*)buffer, size);
	return size;
}

uint16_t SerialLine::readBufFromQueue( uint8_t* buffer, const size_t len)
{
	return uartReadBufFromQueue( _uart, buffer, len);
}

void SerialLine::setupGPIOPins(){
	switch( cfg.pin ){
	case RJ45_3TX_4RX:            // normal RX:16, TX:17
		rx_gpio=rx_pin_norm;
		tx_gpio=tx_pin_norm;
		break;
	case RJ45_3RX_4TX:
		rx_gpio=tx_pin_norm;
		tx_gpio=rx_pin_norm;
		break;
	}
}

void SerialLine::setPinSwap( e_pin pinmode ){  // configures ESP32 matrix, here also the line polartity (inverting) is handled
	cfg.pin = pinmode;
	ESP_LOGI(FNAME,"setPinSwap UART:%d swap:%d (0:NORM, 1:SWAPPED), rx:%d, tx:%d,", uart_nr, pinmode, rx_gpio, tx_gpio );
	if( prior_rx_gpio[uart_nr] != GPIO_NUM_36 ){
		ESP_LOGI(FNAME,"Detach rx:%d", prior_rx_gpio[uart_nr] );
		uartDetachRx(_uart, prior_rx_gpio[uart_nr] );
	}
	if( prior_tx_gpio[uart_nr] != GPIO_NUM_36 ){
		ESP_LOGI(FNAME,"Detach tx:%d", prior_tx_gpio[uart_nr] );
		uartDetachTx(_uart, prior_tx_gpio[uart_nr] );
	}
	if( cfg.tx == SM_MASTER ){
		ESP_LOGI(FNAME,"Attach tx %d", tx_gpio );
		gpio_set_direction(tx_gpio, GPIO_MODE_OUTPUT);
		uartAttachTx( _uart, tx_gpio, cfg.pol != RS232_TTL );
		gpio_pullup_en( tx_gpio );
		prior_tx_gpio[uart_nr] = tx_gpio;
	}else{
		gpio_set_direction(tx_gpio, GPIO_MODE_INPUT);
		gpio_pullup_dis( tx_gpio );
	}
	ESP_LOGI(FNAME,"Attach rx %d", rx_gpio );
	uartAttachRx( _uart, rx_gpio, cfg.pol != RS232_TTL );
	gpio_pullup_en( rx_gpio );
	prior_rx_gpio[uart_nr] = rx_gpio;
}

void SerialLine::loadProfile(e_profile profile){   // load defaults according to given profile
	ESP_LOGI(FNAME,"loadProfile: %d (1: Flarm, 2:Radio)", profile );
	cfg = sm_serial_config[profile];
	_setup.baud->set( cfg.baud );
	_setup.polarity->set( cfg.pol );
	_setup.tx_ena->set( cfg.tx );
	_setup.pin->set( cfg.pin );
}

void SerialLine::receive(const char *msg, int len){
	if ( _dlink.size() >= 1 ) {
		_dlink.begin()->second->process( msg, len );
	}
}

void SerialLine::uartBegin(){
	int baudrate = baud[cfg.baud];
	if( baudrate ){
		ESP_LOGI(FNAME,"uartBegin UART:%d baud:%d, rx:%d, tx:%d, TTL:%d ", uart_nr, baudrate, rx_gpio, tx_gpio, cfg.pol );
		if( _uart )
			stop();
		_uart = ::uartBegin(uart_nr, baudrate, SERIAL_8N1, rx_gpio, tx_gpio, 512, cfg.pol, cfg.pol );
		uartResizeRxBuffer(_uart, 512);
	}
}

void SerialLine::configure(){
	ESP_LOGI(FNAME,"configure UART:%d", uart_nr );
	stop();
	uartBegin();
	setPinSwap( cfg.pin );  // includes Master/Client role handling
	setLineInverse( cfg.pol );
	setBaud( cfg.baud );
}

void SerialLine::stop(){
	if(uartGetDebug() == uart_nr)
		uartSetDebug(0);
	ESP_LOGI(FNAME,"UART %d end(), pins %d %d", uart_nr, tx_gpio, rx_gpio);
	uartEnd(_uart, tx_gpio, rx_gpio);
	_uart = 0;
}


