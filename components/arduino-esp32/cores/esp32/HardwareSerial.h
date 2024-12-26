/*
 HardwareSerial.h - Hardware serial library for Wiring
 Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 Modified 28 September 2010 by Mark Sproul
 Modified 14 August 2012 by Alarus
 Modified 3 December 2013 by Matthijs Kooijman
 Modified 18 December 2014 by Ivan Grokhotkov (esp8266 platform support)
 Modified 31 March 2015 by Markus Sattler (rewrite the code for UART0 + UART1 support in ESP8266)
 Modified 25 April 2015 by Thomas Flayols (add configuration different from 8N1 in ESP8266)
 Modified 13 October 2018 by Jeroen Döll (add baudrate detection)
 Baudrate detection example usage (detection on Serial1):
   void setup() {
     Serial.begin(115200);
     delay(100);
     Serial.println();

     Serial1.begin(0, SERIAL_8N1, -1, -1, true, 11000UL);  // Passing 0 for baudrate to detect it, the last parameter is a timeout in ms

     unsigned long detectedBaudRate = Serial1.baudRate();
     if(detectedBaudRate) {
       Serial.printf("Detected baudrate is %lu\n", detectedBaudRate);
     } else {
       Serial.println("No baudrate detected, Serial1 will not work!");
     }
   }

 Pay attention: the baudrate returned by baudRate() may be rounded, eg 115200 returns 115201

 02.01.2022 Axel Pauli: method number() added.
 01.01.2022 Axel Pauli: Flarm stuff moved to Flarm.
                        added readBufFromQueue() and made some single line
                        methods to inline.
 24.12.2021 Axel Pauli: added RX interrupt handling stuff.
 */

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <inttypes.h>

#include "Stream.h"
#include "esp32-hal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

class HardwareSerial: public Stream
{
public:
    HardwareSerial(int uart_nr);

    void begin(unsigned long baud, uint32_t config=SERIAL_8N1, int8_t rxPin=-1, int8_t txPin=-1, bool rxinvert=false, bool txinvert=false, unsigned long timeout_ms = 20000UL);
    void end();
    void updateBaudRate(unsigned long baud)
    {
      uartSetBaudRate(_uart, baud);
    }
    int available(void)
    {
      return uartAvailable(_uart);
    }
    int availableForWrite(void)
    {
      return uartAvailableForWrite(_uart);
    }
    int peek(void);
    int read(void);
    size_t read(uint8_t *buffer, size_t size);
    inline size_t read(char * buffer, size_t size)
    {
        return read((uint8_t*) buffer, size);
    }
    size_t readLine(uint8_t *buffer, size_t size );
    inline size_t readLine(char * buffer, size_t size )
    {
    	return readLine((uint8_t*) buffer, size );
    }
    size_t readLineFromQueue(uint8_t *buffer, size_t size);

    uint8_t readCharFromQueue( uint8_t* c )
    {
      return uartReadCharFromQueue( _uart, c );
    }
    uint16_t readBufFromQueue( uint8_t* buffer, const size_t len)
    {
      return uartReadBufFromQueue( _uart, buffer, len);
    }
    void flush(void)
    {
      uartFlush(_uart);
    }
    void flush( bool txOnly)
    {
      uartFlushTxOnly(_uart, txOnly);
    }
    size_t write(uint8_t);
    size_t write(const uint8_t *buffer, size_t size);
    inline size_t write(const char * buffer, size_t size)
    {
        return write((uint8_t*) buffer, size);
    }
    inline size_t write(const char * s)
    {
        return write((uint8_t*) s, strlen(s));
    }
    inline size_t write(unsigned long n)
    {
        return write((uint8_t) n);
    }
    inline size_t write(long n)
    {
        return write((uint8_t) n);
    }
    inline size_t write(unsigned int n)
    {
        return write((uint8_t) n);
    }
    inline size_t write(int n)
    {
        return write((uint8_t) n);
    }
    uint32_t baudRate();
    operator bool() const;

    size_t setRxBufferSize(size_t);
    void setDebugOutput(bool);
    
    void setRxInvert(bool);
    void setTxInvert(bool);

    void attachRx(gpio_num_t pin, bool invert);
    void attachTx(gpio_num_t pin, bool invert);
    void detachRx(gpio_num_t pin);
    void detachTx(gpio_num_t pin);

    // enable uart RX interrupt
    void enableRxInterrupt();

    // disable uart RX interupt
    void disableInterrupt();

    // Event group handler to signal RX events to clients waiting for characters.
    static void setRxNotifier( EventGroupHandle_t egh )
    {
      uartRxEventHandler( egh );
    }

    // Functions for handling of newline counter
    void incNlCounter()
    {
      uartIncNlCounter( _uart );
    }

    void decNlCounter()
    {
      uartDecNlCounter( _uart );
    }

    void clearNlCounter()
    {
      uartClearNlCounter( _uart );
    }

    uint16_t getNlCounter()
    {
      return uartGetNlCounter( _uart );
    }

    // return the number of the Uart.
    int number() const
    {
      return _uart_nr;
    }

protected:
    int _uart_nr;
    uart_t* _uart;
    uint8_t _tx_pin;
    uint8_t _rx_pin;
};

extern void serialEventRun(void) __attribute__((weak));

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SERIAL)
extern HardwareSerial Serial;
extern HardwareSerial Serial1;
extern HardwareSerial Serial2;
#endif

#endif // HardwareSerial_h
