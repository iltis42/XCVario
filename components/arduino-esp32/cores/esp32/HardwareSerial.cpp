/**
 * HardwareSerial.cpp
 *
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
    _stopRouting = false;
    clearFlarmTx();
    clearFlarmRx();

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

void HardwareSerial::updateBaudRate(unsigned long baud)
{
	uartSetBaudRate(_uart, baud);
}

// enable uart RX interrupt
void HardwareSerial::enableInterrupt()
{
  // Make sure that the previous interrupt_info is not used anymore
  uartDisableInterrupt( _uart );
  uartEnableInterrupt( _uart );
}

// disable uart RX interupt
void HardwareSerial::disableInterrupt()
{
  uartDisableInterrupt( _uart );
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

int HardwareSerial::available(void)
{
    return uartAvailable(_uart);
}
int HardwareSerial::availableForWrite(void)
{
    return uartAvailableForWrite(_uart);
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

  return count;
}

uint8_t HardwareSerial::readCharFromQueue( uint8_t* c )
{
  return uartReadCharFromQueue( _uart, c );
}

void HardwareSerial::flush(void)
{
    uartFlush(_uart);
}

void HardwareSerial::flush(bool txOnly)
{
    uartFlushTxOnly(_uart, txOnly);
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

/*
 * Check the buffer, if the Flarm Exit command can be found. If yes, return
 * true and give back the sequence number of the command.
 */
bool HardwareSerial::checkFlarmTx( const char* buffer, int length, uint8_t* seq )
{
  for( int i=0; i < length; i++ ) {
      // add byte to shift register
      for( int j=1; j < sizeof(flarmTx); j++ ) {
          flarmTx[j-1] = flarmTx[j];
      }

      flarmTx[sizeof(flarmTx)-1] = buffer[i];

      // The Flarm binary exit command has the following content:
      // S: "73 08 00 01 2c 00  12 22 ba" Exit bin mode
      //     |  |---|    |----| |  |---|
      // Start  length   SEQ-NR MSG CRC
      if( flarmTx[0] == 0x73 && // Startframe
          flarmTx[1] == 8 &&    // length 0
          flarmTx[2] == 0 &&    // length 1
          flarmTx[6] == 0x12 ) { // Exit
          // Flarm Exit Sequence found
          seq[0] = flarmTx[4];
          seq[1] = flarmTx[5];
          clearFlarmTx();
          return true;
      }
  }
  return false;
}

/*
 * Check the buffer, if the Flarm Exit command is acknowledged. If yes, return
 * true and give back the buffer position, where the answer starts.
 */
bool HardwareSerial::checkFlarmRx( const char* buffer,
                                   int length,
                                   uint8_t* seq,
                                   int* start )
{
  for( int i=0; i < length; i++ ) {
      // add byte to shift register
      for( int j=1; j < sizeof(flarmRx); j++ ) {
          flarmRx[j-1] = flarmRx[j];
      }

      flarmRx[sizeof(flarmRx)-1] = buffer[i];

      // R: "73 0a 00 01 5e 12 a0 9e 06 2c 00" Quittung exit bin mode
      //     |  |---|          | |---| |---|
      // Start  length         MSG CRC SEQ-NR-von-S
      if( flarmRx[0] == 0x73 && // Startframe
          flarmRx[1] == 0xa &&  // length 0
          flarmRx[2] == 0 &&    // length 1
          flarmRx[6] == 0xa0 && // ACK
          flarmRx[9] == seq[0] &&
          flarmRx[10] == seq[1] ) {
          // Flarm positive ACK to Exit command found
          *start = i;
          clearFlarmRx();
          return true;
      }
  }
  return false;
}
