/**
 * HardwareSerial.cpp
 *
 * 01.01.2022 Axel Pauli: Flarm stuff moved to Flarm.
 *                        added readBufFromQueue() and made some single line
 *                        methods to inline.
 * 24.12.2021 Axel Pauli: added RX interrupt handling stuff.
 */
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cinttypes>

#include "pins_arduino.h"
#include "esp32-hal-uart.h"
#include "HardwareSerial.h"

#ifndef RX1
#define RX1 9
#endif

#ifndef TX1
#define TX1 10
#endif

#ifndef RX2
#define RX2 16
#endif

#ifndef TX2
#define TX2 17
#endif

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SERIAL)
// HardwareSerial Serial(0);
HardwareSerial Serial1(1);
HardwareSerial Serial2(2);
#endif

HardwareSerial::HardwareSerial(int uart_nr) : _uart_nr(uart_nr), _uart(nullptr) {}

void HardwareSerial::begin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, bool rxinvert, bool txinvert, unsigned long timeout_ms)
{
	if(0 > _uart_nr || _uart_nr > 2) {
		log_e("Serial number is invalid, please use 0, 1 or 2");
		return;
	}

	if(_uart) {
		end();
	}
	if(_uart_nr == 0 && rxPin < 0 && txPin < 0) {
		rxPin = 3;
		txPin = 1;
	}
	if(_uart_nr == 1 && rxPin < 0 && txPin < 0) {
		rxPin = RX1;
		txPin = TX1;
	}
	if(_uart_nr == 2 && rxPin < 0 && txPin < 0) {
		rxPin = RX2;
		txPin = TX2;
	}

	_uart = uartBegin(_uart_nr, baud ? baud : 9600, config, rxPin, txPin, 256, rxinvert, txinvert );
	_tx_pin = txPin;
	_rx_pin = rxPin;

	if(!baud) {
		uartStartDetectBaudrate(_uart);
		time_t startMillis = millis();
		unsigned long detectedBaudRate = 0;
		while(millis() - startMillis < timeout_ms && !(detectedBaudRate = uartDetectBaudrate(_uart))) {
			yield();
		}

		end();

		if(detectedBaudRate) {
			delay(100); // Give some time...
			_uart = uartBegin(_uart_nr, detectedBaudRate, config, rxPin, txPin, 256, rxinvert, txinvert );
		} else {
			log_e("Could not detect baudrate. Serial data at the port must be present within the timeout for detection to be possible");
			_uart = NULL;
			_tx_pin = 255;
			_rx_pin = 255;
		}
	}
}

// enable uart RX interrupt
void HardwareSerial::enableRxInterrupt()
{
	// Make sure that the previous interrupt_info is not used anymore
	if( _uart ){
		uartDisableInterrupt( _uart );
		uartEnableRxInterrupt( _uart );
	}
}

// disables all uart interupts
void HardwareSerial::disableInterrupt()
{
	if( _uart ){
		uartDisableInterrupt( _uart );
	}
}

void HardwareSerial::end()
{
	if(uartGetDebug() == _uart_nr) {
		uartSetDebug(0);
	}
	log_v("pins %d %d",_tx_pin, _rx_pin);
	uartEnd(_uart, _tx_pin, _rx_pin);
	_uart = 0;
}

size_t HardwareSerial::setRxBufferSize(size_t new_size) {
	return uartResizeRxBuffer(_uart, new_size);
}

void HardwareSerial::setDebugOutput(bool en)
{
	if(_uart == 0) {
		return;
	}
	if(en) {
		uartSetDebug(_uart);
	} else {
		if(uartGetDebug() == _uart_nr) {
			uartSetDebug(0);
		}
	}
}

int HardwareSerial::peek(void)
{
	if (available()) {
		return uartPeek(_uart);
	}
	return -1;
}

int HardwareSerial::read(void)
{
	if(available()) {
		return uartRead(_uart);
	}
	return -1;
}

// read characters into buffer
// terminates if size characters have been read, or no further are pending
// returns the number of characters placed in the buffer
// the buffer is NOT null terminated.
size_t HardwareSerial::read(uint8_t *buffer, size_t size)
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

// read serial interface until '\n' newline character via polling.
size_t HardwareSerial::readLine(uint8_t *buffer, size_t size)
{
	int count = 0;
	int timeout = 10;
	while( count < size ) {
		if( available() ){
			timeout = 10;
			char c = uartRead(_uart);
			*buffer = c;
			buffer++;
			count++;
			if( c == '\n' )
				break;
		}else{
			delay(5);  // wait 5 ms until next data avail check
			timeout--; // increase timeout
		}
		if( timeout == 0 ){
			break;    // do not block when data has stopped
			log_e("Timeout in readLine");
		}
	}
	return count;
}

// Read serial interface receiver queue until '\n' newline character or
// queue is empty.
size_t HardwareSerial::readLineFromQueue(uint8_t *buffer, size_t size)
{
	size_t count = 0;

	while( count < size ) {
		unsigned char c;

		uint8_t res = uartReadCharFromQueue( _uart, &c );

		if( res == 0 ) {
			// no more characters available
			return count;
		}

		*buffer = c;
		buffer++;
		count++;
		if( c == '\n' )
			break;
	}
	decNlCounter();
	return count;
}

size_t HardwareSerial::write(uint8_t c)
{
	uartWrite(_uart, c);
	return 1;
}

size_t HardwareSerial::write(const uint8_t *buffer, size_t size)
{
	uartWriteBuf(_uart, buffer, size);
	return size;
}
uint32_t  HardwareSerial::baudRate()

{
	return uartGetBaudRate(_uart);
}
HardwareSerial::operator bool() const
		{
	return true;
		}

void HardwareSerial::setRxInvert(bool invert)
{
	uartSetRxInvert(_uart, invert);
}

void HardwareSerial::setTxInvert(bool invert)
{
	uartSetTxInvert(_uart, invert);
}
