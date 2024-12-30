#include "SerialLine.h"
#include <esp_log.h>
#include <SetupNG.h>
#include "Serial.h"

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

SerialLine::SerialLine(uart_port_t uart) :
	_intfid(InterfaceId(S0_RS232+uart)),
	_id_memo(MEMOS[uart])
{
	uart_nr = uart;
	switch( uart_nr ){
	case UART_NUM_1:
		hw_serial = &Serial1;
		cfg.baud = (e_baud)serial1_speed.get();        // load settings from NVS
		cfg.pol = (e_polarity)serial1_tx_inverted.get();
		cfg.tx = (e_tx)serial1_tx_enable.get();
		cfg.pin = (e_pin)serial1_pins_twisted.get();
		tx_q = &s1_tx_q;
		tx_req = TX1_REQ;
		break;
	case UART_NUM_2:
		hw_serial = &Serial2;
		cfg.baud = (e_baud)serial2_speed.get();
		cfg.pol = (e_polarity)serial2_tx_inverted.get();
		cfg.tx = (e_tx)serial2_tx_enable.get();
		cfg.pin = (e_pin)serial2_pins_twisted.get();
		tx_q = &s2_tx_q;
		tx_req = TX2_REQ;
		break;
	}
	tx_q->setSize( 5 );
	setupGPIOPins();
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

void SerialLine::setBaud(e_baud abaud, bool coldstart ){
	ESP_LOGI(FNAME,"setBaud: UART: %d baud:%d", uart_nr, baud[abaud]);
	if( abaud == 0 ){
		Serial::taskStop( uart_nr );
		return;
	}
	uart_set_baudrate(uart_nr, baud[abaud] );
	if( !Serial::taskStarted( uart_nr ) ){
		Serial::taskStart(uart_nr);
	}
	cfg.baud = abaud;
};

void SerialLine::setLineInverse(e_polarity apol){
	ESP_LOGI(FNAME,"setLineInverse: UART: %d %d (0: Norm, 1:Invers)", uart_nr, apol);
	cfg.pol = apol;
	if( cfg.pol == RS232_TTL ){
		hw_serial->setRxInvert( true );  // RS232 TTL is inverted
		hw_serial->setTxInvert( true);
	}
	else{
		hw_serial->setRxInvert( false );
		hw_serial->setTxInvert( false );
	}
};

void SerialLine::setSlaveRole( e_tx tx ){
	ESP_LOGI(FNAME,"setRole: UART:%d (0: Slave, 1:Master)", tx);
	cfg.tx = tx;
	setPinSwap( cfg.pin );
}

void SerialLine::setupGPIOPins(){
	switch( uart_nr ){
	case UART_NUM_1:
		switch( cfg.pin ){
		case RJ45_3TX_4RX:            // normal RX:16, TX:17
			rx_gpio=GPIO_NUM_16;
			tx_gpio=GPIO_NUM_17;
			break;
		case RJ45_3RX_4TX:
			rx_gpio=GPIO_NUM_17;
			tx_gpio=GPIO_NUM_16;
			break;
		}
		break;
	case UART_NUM_2:
		switch( cfg.pin ){
		case RJ45_3TX_4RX:      // Normal RX:18, TX:4
			rx_gpio=GPIO_NUM_18;
			tx_gpio=GPIO_NUM_4;
			break;
		case RJ45_3RX_4TX:
			rx_gpio=GPIO_NUM_4;
			tx_gpio=GPIO_NUM_18;
			break;
		}
		break;
	}
}

void SerialLine::setPinSwap( e_pin pinmode ){  // configures ESP32 matrix, here also the line polartity (inverting) is handled
	cfg.pin = pinmode;
	ESP_LOGI(FNAME,"setPinSwap UART:%d swap:%d (0:NORM, 1:SWAPPED), rx:%d, tx:%d,", uart_nr, pinmode, rx_gpio, tx_gpio );
	if( prior_rx_gpio[uart_nr] != GPIO_NUM_36 ){
		ESP_LOGI(FNAME,"Detach rx:%d", prior_rx_gpio[uart_nr] );
		hw_serial->detachRx( prior_rx_gpio[uart_nr] );
	}
	if( prior_tx_gpio[uart_nr] != GPIO_NUM_36 ){
		ESP_LOGI(FNAME,"Detach tx:%d", prior_tx_gpio[uart_nr] );
		hw_serial->detachTx( prior_tx_gpio[uart_nr] );
	}
	if( cfg.tx == SM_MASTER ){
		ESP_LOGI(FNAME,"Attach tx %d", tx_gpio );
		gpio_set_direction(tx_gpio, GPIO_MODE_OUTPUT);
		hw_serial->attachTx( tx_gpio, cfg.pol != RS232_TTL );
		gpio_pullup_en( tx_gpio );
		prior_tx_gpio[uart_nr] = tx_gpio;
	}else{
		gpio_set_direction(tx_gpio, GPIO_MODE_INPUT);
		gpio_pullup_dis( tx_gpio );
	}
	ESP_LOGI(FNAME,"Attach rx %d", rx_gpio );
	hw_serial->attachRx( rx_gpio, cfg.pol != RS232_TTL );
	gpio_pullup_en( rx_gpio );
	prior_rx_gpio[uart_nr] = rx_gpio;
}

void SerialLine::loadProfile(e_profile profile){   // load defaults according to given profile
	ESP_LOGI(FNAME,"loadProfile: %d (1: Flarm, 2:Radio)", profile );
	cfg = sm_serial_config[profile];
	switch( uart_nr ){
	case UART_NUM_1:
		serial1_speed.set(cfg.baud);        // load settings from NVS
		serial1_tx_inverted.set(cfg.pol);
		serial1_tx_enable.set(cfg.tx);
		serial1_pins_twisted.set(cfg.pin);
		break;
	case UART_NUM_2:
		serial2_speed.set(cfg.baud);
		serial2_tx_inverted.set(cfg.pol);
		serial2_tx_enable.set(cfg.tx);
		serial2_pins_twisted.set(cfg.pin);
		break;
	}
}

void SerialLine::receive( const char *msg, int len, int port ){
	DataLink* dl = getDataLink( port );
	dl->process( msg, len );
}

void SerialLine::uartBegin(){
	int baudrate = baud[cfg.baud];
	if( baudrate ){
		ESP_LOGI(FNAME,"uartBegin UART:%d baud:%d, rx:%d, tx:%d, TTL:%d ", uart_nr, baudrate, rx_gpio, tx_gpio, cfg.pol );
		hw_serial->begin(baudrate,SERIAL_8N1,rx_gpio,tx_gpio, cfg.pol, cfg.pol );  // should be reentrant with auto end() at the begin
		hw_serial->setRxBufferSize(512);
	}
}

void SerialLine::configure(){
	ESP_LOGI(FNAME,"configure UART:%d", uart_nr );
	uartBegin();
	setPinSwap( cfg.pin );  // includes Master/Client role handling
	setLineInverse( cfg.pol );
	setBaud( cfg.baud );
}

void SerialLine::stop(){
	hw_serial->end();
}


