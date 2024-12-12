#include "SerialLine.h"
#include <HardwareSerial.h>
#include <esp_log.h>
#include <SetupNG.h>
#include "Serial.h"


t_serial_cfg sm_serial_config[] = {
		{ SM_FLARM,       BAUD_19200, RS232_TTL, RJ45_3TX_4RX, SM_MASTER },
		{ SM_RADIO,       BAUD_9600,  RS232_TTL, RJ45_3TX_4RX, SM_MASTER },
		{ SM_XCTNAV_S3,   BAUD_19200, RS232_TTL, RJ45_3TX_4RX, SM_MASTER },
		{ SM_OPENVARIO,   BAUD_19200, RS232_TTL, RJ45_3RX_4TX, SM_MASTER },
		{ SM_XCFLARMVIEW, BAUD_57600, RS232_TTL, RJ45_3RX_4TX, SM_MASTER }
};

gpio_num_t SerialLine::prior_S1_tx_gpio = GPIO_NUM_36;
gpio_num_t SerialLine::prior_S1_rx_gpio = GPIO_NUM_36;
gpio_num_t SerialLine::prior_S2_tx_gpio = GPIO_NUM_36;
gpio_num_t SerialLine::prior_S2_rx_gpio = GPIO_NUM_36;

SerialLine::SerialLine(uart_port_t uart){
	uart_nr = uart;
	switch( uart_nr ){
	case UART_NUM_1:
		cfg.baud = (e_baud)serial1_speed.get();        // load settings from NVS
		cfg.pol = (e_polarity)serial1_tx_inverted.get();
		cfg.tx = (e_tx)serial1_tx_enable.get();
		cfg.pin = (e_pin)serial1_pins_twisted.get();
		break;
	case UART_NUM_2:
		cfg.baud = (e_baud)serial2_speed.get();
		cfg.pol = (e_polarity)serial2_tx_inverted.get();
		cfg.tx = (e_tx)serial2_tx_enable.get();
		cfg.pin = (e_pin)serial2_pins_twisted.get();
		break;
	}
	getGPIOPins();
	ESP_LOGI(FNAME,"CONSTR. UART:%d (new) RX:%d TX:%d baud:%d pol:%d swap:%d tx:%d", uart_nr, rx_gpio, tx_gpio, baud[cfg.baud], cfg.pol, cfg.pin, cfg.tx );
};

void SerialLine::ConfigureIntf(int cfg){
	loadProfile( (_e_profile)cfg );
	configure();
};



int SerialLine::Send(const char *msg, int len, int port){

	RingBufCPP<SString>* q = NULL;
	if( port == 1 )
		q = &s2_tx_q;
	else if( port == 2 )
		q = &s2_tx_q;
	else
		return 0;
	if( !q->isFull() ) {
		q->add( SString( msg) );
		return 0;
	}
	return len; // fixme better heuristic neede here
};


void SerialLine::setBaud(e_baud abaud, bool coldstart ){
	ESP_LOGI(FNAME,"setBaud: UART: %d baud:%d", uart_nr, baud[abaud]);
	if( abaud == 0 ){
		Serial::taskStop( uart_nr );
		return;
	}
	uart_set_baudrate(uart_nr, baud[abaud] );
	switch( uart_nr ){
	case UART_NUM_1:
		if( !Serial::taskStarted( uart_nr ) ){
			Serial::taskStartS1();
		}
		break;
	case UART_NUM_2:
		if( !Serial::taskStarted( uart_nr ) ){
			Serial::taskStartS2();
		}
		break;
	}
	cfg.baud = abaud;
};

void SerialLine::setLineInverse(e_polarity apol){
	ESP_LOGI(FNAME,"setLineInverse: UART: %d %d (0: Norm, 1:Invers)", uart_nr, apol);
	cfg.pol = apol;
	switch( uart_nr ){
	case UART_NUM_1:
		if( cfg.pol == RS232_TTL ){
			Serial1.setRxInvert( true );  // RS232 TTL is inverted
			Serial1.setTxInvert( true);
		}
		else{
			Serial1.setRxInvert( false );
			Serial1.setTxInvert( false );
		}
		break;
	case UART_NUM_2:
		if( cfg.pol == RS232_TTL ){
			Serial2.setRxInvert( true );
			Serial2.setTxInvert( true );
		}
		else{
			Serial2.setRxInvert( false );
			Serial2.setTxInvert( false );
		}
		break;
	}
};

void SerialLine::setSlaveRole( e_tx tx ){
	ESP_LOGI(FNAME,"setRole: UART:%d (0: Slave, 1:Master)", tx);
	cfg.tx = tx;
	setPinSwap( cfg.pin );
}

void SerialLine::getGPIOPins(){
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
	switch( uart_nr ){
		case UART_NUM_1:
			if( prior_S1_rx_gpio != GPIO_NUM_36 ){
				ESP_LOGI(FNAME,"Detach rx:%d", prior_S1_rx_gpio );
				Serial1.detachRx( prior_S1_rx_gpio );
			}
			if( prior_S1_tx_gpio != GPIO_NUM_36 ){
				ESP_LOGI(FNAME,"Detach tx:%d", prior_S1_tx_gpio );
				Serial1.detachTx( prior_S1_tx_gpio );
			}
			if( cfg.tx == SM_MASTER ){
				ESP_LOGI(FNAME,"Attach tx %d", tx_gpio );
				gpio_set_direction(tx_gpio, GPIO_MODE_OUTPUT);
				Serial1.attachTx( tx_gpio, cfg.pol != RS232_TTL );
				gpio_pullup_en( tx_gpio );
				prior_S1_tx_gpio = tx_gpio;
			}else{
				gpio_set_direction(tx_gpio, GPIO_MODE_INPUT);
				gpio_pullup_dis( tx_gpio );
			}
			ESP_LOGI(FNAME,"Attach rx %d", rx_gpio );
			Serial1.attachRx( rx_gpio, cfg.pol != RS232_TTL );
			gpio_pullup_en( rx_gpio );
			prior_S1_rx_gpio = rx_gpio;

			break;
	case UART_NUM_2:
			if( prior_S2_rx_gpio != GPIO_NUM_36 ){
				ESP_LOGI(FNAME,"Detach rx %d", prior_S2_rx_gpio );
				Serial2.detachRx( prior_S2_rx_gpio );
			}
			if( prior_S2_tx_gpio != GPIO_NUM_36 ){
				ESP_LOGI(FNAME,"Detach tx %d", prior_S2_tx_gpio );
				Serial2.detachTx( prior_S2_tx_gpio );
			}
			if( cfg.tx == SM_MASTER ){
				ESP_LOGI(FNAME,"Attach tx %d", tx_gpio );
				gpio_set_direction(tx_gpio, GPIO_MODE_OUTPUT);
				Serial2.attachTx( tx_gpio, cfg.pol != RS232_TTL );
				gpio_pullup_en( tx_gpio );
				prior_S2_tx_gpio = tx_gpio;
			}else{
				gpio_set_direction(tx_gpio, GPIO_MODE_INPUT);
				gpio_pullup_dis( tx_gpio );
			}
			ESP_LOGI(FNAME,"Attach rx %d", rx_gpio );
			Serial2.attachRx( rx_gpio, cfg.pol != RS232_TTL );
			gpio_pullup_en( rx_gpio );
			prior_S2_rx_gpio = rx_gpio;
			break;
	}
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

void SerialLine::uartBegin(){
	int baudrate = baud[cfg.baud];
	if( baudrate ){
		switch( uart_nr ){
		case UART_NUM_1:
			ESP_LOGI(FNAME,"uartBegin UART:%d baud:%d, rx:%d, tx:%d, TTL:%d ", uart_nr, baudrate, rx_gpio, tx_gpio, cfg.pol );
			Serial1.begin(baudrate,SERIAL_8N1,rx_gpio,tx_gpio, cfg.pol, cfg.pol );  // should be reentrant with auto end() at the begin
			Serial1.setRxBufferSize(512);
			break;
		case UART_NUM_2:
			ESP_LOGI(FNAME,"uartBegin UART:%d baud:%d, rx:%d, tx:%d, TTL:%d ", uart_nr, baudrate, rx_gpio, tx_gpio, cfg.pol );
			Serial2.begin(baudrate,SERIAL_8N1,rx_gpio,tx_gpio, cfg.pol, cfg.pol );
			Serial2.setRxBufferSize(512);
			break;
		}
	}
}


void SerialLine::configure(){
	ESP_LOGI(FNAME,"configure UART:%d", uart_nr );
	s1_tx_q.setSize(2);
	s2_tx_q.setSize(2);
	uartBegin();
	setPinSwap( cfg.pin );  // includes Master/Client role handling
	setLineInverse( cfg.pol );
	setBaud( cfg.baud );
}

void SerialLine::start(){
	configure();
	/*
	switch( uart_nr ){
	case UART_NUM_1:
		Serial1.begin(baud[serial1_speed.get()],SERIAL_8N1,rx_gpio,tx_gpio,serial1_rx_inverted.get(),serial1_tx_inverted.get());
		Serial1.setRxBufferSize(512);
		break;
	case UART_NUM_2:
		Serial2.begin(baud[serial2_speed.get()],SERIAL_8N1,rx_gpio,tx_gpio,serial2_rx_inverted.get(),serial2_tx_inverted.get());
		Serial2.setRxBufferSize(512);
		break;
	}
	*/
}

void SerialLine::stop(){
	switch( uart_nr ){
	case UART_NUM_1:
		Serial1.end();
		break;
	case UART_NUM_2:
		Serial2.end();
		break;
	}
}


