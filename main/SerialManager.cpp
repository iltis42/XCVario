#include "SerialManager.h"
#include <HardwareSerial.h>
#include <esp_log.h>
#include <SetupNG.h>


t_serial_cfg sm_serial_config[] = {
		{ SM_FLARM,       BAUD_19200, RS232_TTL, RJ45_3TX_4RX, SM_MASTER },
		{ SM_RADIO,       BAUD_9600,  RS232_TTL, RJ45_3TX_4RX, SM_MASTER },
		{ SM_XCTNAV_S3,   BAUD_19200, RS232_TTL, RJ45_3TX_4RX, SM_MASTER },
		{ SM_OPENVARIO,   BAUD_19200, RS232_TTL, RJ45_3RX_4TX, SM_MASTER },
		{ SM_XCFLARMVIEW, BAUD_57600, RS232_TTL, RJ45_3RX_4TX, SM_MASTER }
};

SerialManager::SerialManager(uart_port_t uart){
	uart_nr = uart;
	tx_gpio = GPIO_NUM_36;  // dummy port
	rx_gpio = GPIO_NUM_36;
	if( serial2_protocol.get() > 0 )
		cfg = sm_serial_config[ serial2_protocol.get() ];
};

void SerialManager::setBaud(e_baud abaud){
	cfg.baud = abaud;
	switch( uart_nr ){
	case UART_NUM_1:
		serial1_speed.set(cfg.baud);
		break;
	case UART_NUM_2:
		serial2_speed.set(cfg.baud);
		break;
	}
	uart_set_baudrate(uart_nr, baud[abaud] );
};

void SerialManager::setLineInverse(e_polarity apol){
	cfg.pol = apol;
	switch( uart_nr ){
	case UART_NUM_1:
		serial1_rx_inverted.set(cfg.pol);  // save both, one can be dropped later
		serial1_tx_inverted.set(cfg.pol);
		if( cfg.pol == RS232_TTL ){
			Serial1.setRxInvert( true );
			Serial1.setTxInvert( true );
		}
		else{
			Serial1.setRxInvert( false );
			Serial1.setTxInvert( false );
		}
		break;
	case UART_NUM_2:
		serial2_rx_inverted.set(cfg.pol);
		serial2_tx_inverted.set(cfg.pol);
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

void SerialManager::setSlaveRole( e_tx tx ){
	cfg.tx = tx;
	switch( uart_nr ){
	case UART_NUM_1:
		serial1_tx_enable.set(cfg.tx);  // Master = 1 = TX enable
		break;
	case UART_NUM_2:
		serial2_tx_enable.set(cfg.tx);
		break;
	}
	setPinSwap( cfg.pin );
}


void SerialManager::setPinSwap( e_pin pinmode ){
	switch( uart_nr ){
		case UART_NUM_1:
			serial1_pins_twisted.set(pinmode);
			Serial1.detachRx( rx_gpio );
			Serial1.detachTx( tx_gpio );
			tx_gpio=GPIO_NUM_36;
			switch( pinmode ){
			case RJ45_3TX_4RX:            // normal RX:16, TX:17
				rx_gpio=GPIO_NUM_16;
				if(cfg.tx == SM_MASTER){
					tx_gpio=GPIO_NUM_17;
				}
				break;
			case RJ45_3RX_4TX:
				rx_gpio=GPIO_NUM_17;
				if(cfg.tx == SM_MASTER){
					tx_gpio=GPIO_NUM_16;
				}
				break;
			}
			if( tx_gpio != GPIO_NUM_36 ){
				gpio_set_direction(tx_gpio, GPIO_MODE_OUTPUT);
				Serial1.attachTx( tx_gpio, cfg.pol != RS232_TTL );
				gpio_pullup_en( tx_gpio );
			}else{
				gpio_set_direction(tx_gpio, GPIO_MODE_INPUT);
				gpio_pullup_dis( tx_gpio );
			}
			Serial1.attachRx( rx_gpio, cfg.pol != RS232_TTL );
			gpio_pullup_en( rx_gpio );

			break;
	case UART_NUM_2:
			serial2_pins_twisted.set(pinmode);
		    Serial2.detachRx( rx_gpio );
			Serial2.detachTx( tx_gpio );
			tx_gpio = GPIO_NUM_36;
			switch( pinmode ){
			case RJ45_3TX_4RX:      // Normal RX:18, TX:4
				rx_gpio=GPIO_NUM_18;
				if(cfg.tx == SM_MASTER){
					tx_gpio=GPIO_NUM_4;
				}
				break;
			case RJ45_3RX_4TX:
				rx_gpio=GPIO_NUM_4;
				if(cfg.tx == SM_MASTER){
					tx_gpio=GPIO_NUM_18;
				}
				break;
			}
			if( tx_gpio != GPIO_NUM_36 ){
				Serial2.attachTx( tx_gpio, cfg.pol != RS232_TTL );
				gpio_set_direction(tx_gpio, GPIO_MODE_OUTPUT);
				gpio_pullup_en( tx_gpio );
			}else{
				gpio_set_direction(tx_gpio, GPIO_MODE_INPUT);
				gpio_pullup_dis( tx_gpio );
			}
			Serial2.attachRx( rx_gpio, cfg.pol != RS232_TTL );
			gpio_pullup_en( rx_gpio );
			break;
	}
	cfg.pin = pinmode;
}



void SerialManager::configure(e_profile profile){
	cfg = sm_serial_config[profile];
	setPinSwap( cfg.pin );  // includes Master/Client role handling
	setSlaveRole( cfg.tx );
	setLineInverse( cfg.pol );
	setBaud( cfg.baud );
}

void SerialManager::start(){
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
}

void SerialManager::stop(){
	switch( uart_nr ){
	case UART_NUM_1:
		// Serial1.end();   looks like HardwareSerial is managing this!
		break;
	case UART_NUM_2:
		// Serial2.end();
		break;
	}
	// delay(100);
}


