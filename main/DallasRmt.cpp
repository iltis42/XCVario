#include "DallasRmt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <esp_log.h>
#include <time.h>
#include <string.h>


// OneWire commands
#define STARTCONVO      0x44  // Tells device to take a temperature reading and put it on the scratchpad
#define COPYSCRATCH     0x48  // Copy EEPROM
#define READSCRATCH     0xBE  // Read EEPROM
#define WRITESCRATCH    0x4E  // Write to EEPROM
#define RECALLSCRATCH   0xB8  // Reload from last known
#define READPOWERSUPPLY 0xB4  // Determine if device needs parasite power
#define ALARMSEARCH     0xEC  // Query bus for devices with an alarm condition

// Scratchpad locations
#define TEMP_LSB        0
#define TEMP_MSB        1
#define HIGH_ALARM_TEMP 2
#define LOW_ALARM_TEMP  3
#define CONFIGURATION   4
#define INTERNAL_BYTE   5
#define COUNT_REMAIN    6
#define COUNT_PER_C     7
#define SCRATCHPAD_CRC  8

// Device resolution
#define TEMP_9_BIT  0x1F //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

DallasRmt::DallasRmt() : _ow(nullptr), _ownOnewire(false)
{

}

DallasRmt::DallasRmt(OnewireRmt* ow): _ownOnewire(false)
{
    setOneWire(ow);
}

DallasRmt::DallasRmt(uint8_t pin, uint8_t rmt_rx, uint8_t rmt_tx)
{
    _devices = 0;
    _parasite = false;
    _bitResolution = 9;
    _waitForConversion = true;
    _checkForConversion = true;
    _is_connected = false;
}

DallasRmt::~DallasRmt()
{
    if (_ownOnewire) {
        delete _ow;
    }
}

void DallasRmt::setOneWire(OnewireRmt* ow)
{
    if (_ownOnewire) {
        delete _ow;
        _ow = nullptr;
    }
    _ow = ow;
    _devices = 0;
    _parasite = false;
    _bitResolution = 9;
    _waitForConversion = true;
    _checkForConversion = true;
}
// initialise the bus

void DallasRmt::begin(void)
{
    DeviceAddress deviceAddress;

    _ow->reset_search();
    _devices = 0; // Reset the number of devices when we enumerate wire devices

    while (_ow->search(deviceAddress)) {
        if (validAddress(deviceAddress)) {
            if (!_parasite && readPowerSupply(deviceAddress)) {
                _parasite = true;
            }
            _bitResolution = MAX(_bitResolution, getResolution(deviceAddress));
            _devices++;
        }
    }
}
// returns true if address is valid

bool DallasRmt::validAddress(const uint8_t* deviceAddress)
{
    return (_ow->crc8(deviceAddress, 7) == deviceAddress[7]);
}

bool DallasRmt::validFamily(const uint8_t* deviceAddress)
{
    bool ret;
    switch (deviceAddress[0]) {
        case DS18S20MODEL:
        case DS18B20MODEL:
        case DS1822MODEL:
        case DS1825MODEL:
        case DS28EA00MODEL:
            ret = true;
            break;
        default:
            ret = false;
    }
    return ret;
}

// finds an address at a given index on the bus
// returns true if the device was found

bool DallasRmt::getAddress(uint8_t* deviceAddress, uint8_t index)
{
    uint8_t depth = 0;

    _ow->reset_search();

    while (depth <= index && _ow->search(deviceAddress)) {
        if (depth == index && validAddress(deviceAddress)) return true;
        depth++;
    }

    return false;
}

// attempt to determine if the device at the given address is connected to the bus

bool DallasRmt::isConnected(const uint8_t* deviceAddress)
{
    ScratchPad scratchPad;
    return isConnected(deviceAddress, scratchPad);
}

// attempt to determine if the device at the given address is connected to the bus
// also allows for updating the read scratchpad

bool DallasRmt::isConnected(const uint8_t* deviceAddress, uint8_t* scratchPad)
{
    bool b = readScratchPad(deviceAddress, scratchPad);
    bool crc = (_ow->crc8(scratchPad, 8) == scratchPad[SCRATCHPAD_CRC]);
    if( crc && b ){
    	if( _is_connected )   // suppress the first power value of 85 deg
    		return true;
    	else{
    		_is_connected = true;
    		return false;
    	}
    }
    else{
    	_is_connected = false;
    	return false;
    }
}

bool DallasRmt::readScratchPad(const uint8_t* deviceAddress, uint8_t* scratchPad)
{

    // send the reset command and fail fast
    int b = _ow->reset();
    if (b == 0) return false;

    _ow->select(deviceAddress);
    _ow->write(READSCRATCH);

    // Read all registers in a simple loop
    // byte 0: temperature LSB
    // byte 1: temperature MSB
    // byte 2: high alarm temp
    // byte 3: low alarm temp
    // byte 4: DS18S20: store for crc
    //         DS18B20 & DS1822: configuration register
    // byte 5: internal use & crc
    // byte 6: DS18S20: COUNT_REMAIN
    //         DS18B20 & DS1822: store for crc
    // byte 7: DS18S20: COUNT_PER_C
    //         DS18B20 & DS1822: store for crc
    // byte 8: SCRATCHPAD_CRC
    _ow->read_bytes(scratchPad, 9);

    b = _ow->reset();
    return (b == 1);
}

void DallasRmt::writeScratchPad(const uint8_t* deviceAddress, const uint8_t* scratchPad)
{

    _ow->reset();
    _ow->select(deviceAddress);
    _ow->write(WRITESCRATCH);
    _ow->write(scratchPad[HIGH_ALARM_TEMP]); // high alarm temp
    _ow->write(scratchPad[LOW_ALARM_TEMP]); // low alarm temp

    // DS1820 and DS18S20 have no configuration register
    if (deviceAddress[0] != DS18S20MODEL) {
        _ow->write(scratchPad[CONFIGURATION]);
    }

    //_ow->reset();

    // save the newly written values to eeprom
    //_ow->select(deviceAddress);
    //_ow->write(COPYSCRATCH, parasite);
    //delay(20); // <--- added 20ms delay to allow 10ms long EEPROM write operation (as specified by datasheet)

    //if (parasite) delay(10); // 10ms delay
    _ow->reset();
}

bool DallasRmt::readPowerSupply(const uint8_t* deviceAddress)
{
    bool ret = false;
    _ow->reset();
    _ow->select(deviceAddress);
    _ow->write(READPOWERSUPPLY);
    if (_ow->read_bit() == 0) {
        ret = true;
    }
    _ow->reset();
    return ret;

}

// returns the current resolution of the device, 9-12
// returns 0 if device not found

uint8_t DallasRmt::getResolution(const uint8_t* deviceAddress)
{
    // DS1820 and DS18S20 have no resolution configuration register
    if (deviceAddress[0] == DS18S20MODEL) {
        return 12;
    }

    ScratchPad scratchPad;
    if (isConnected(deviceAddress, scratchPad)) {
        switch (scratchPad[CONFIGURATION]) {
            case TEMP_12_BIT:
                return 12;
            case TEMP_11_BIT:
                return 11;
            case TEMP_10_BIT:
                return 10;
            case TEMP_9_BIT:
                return 9;
        }
    }
    return 0;
}
// set resolution of all devices to 9, 10, 11, or 12 bits
// if new resolution is out of range, it is constrained.

void DallasRmt::setResolution(uint8_t newResolution)
{
    _bitResolution = (newResolution < 9) ? 9 : (newResolution > 12 ? 12 : newResolution);
    DeviceAddress deviceAddress;
    for (int i = 0; i < _devices; i++) {
        getAddress(deviceAddress, i);
        setResolution(deviceAddress, _bitResolution, true);
    }
}

// set resolution of a device to 9, 10, 11, or 12 bits
// if new resolution is out of range, 9 bits is used.

bool DallasRmt::setResolution(const uint8_t* deviceAddress, uint8_t newResolution, bool skipGlobalBitResolutionCalculation)
{
    // ensure same behavior as setResolution(uint8_t newResolution)
    newResolution = (newResolution < 9) ? 9 : (newResolution > 12 ? 12 : newResolution);

    // return when stored value == new value
    if (getResolution(deviceAddress) == newResolution) {
        return true;
    }

    ScratchPad scratchPad;
    if (isConnected(deviceAddress, scratchPad)) {
        // DS1820 and DS18S20 have no resolution configuration register
        if (deviceAddress[0] != DS18S20MODEL) {
            switch (newResolution) {
                case 12:
                    scratchPad[CONFIGURATION] = TEMP_12_BIT;
                    break;
                case 11:
                    scratchPad[CONFIGURATION] = TEMP_11_BIT;
                    break;
                case 10:
                    scratchPad[CONFIGURATION] = TEMP_10_BIT;
                    break;
                case 9:
                default:
                    scratchPad[CONFIGURATION] = TEMP_9_BIT;
                    break;
            }
            writeScratchPad(deviceAddress, scratchPad);

            // without calculation we can always set it to max
            _bitResolution = MAX(_bitResolution, newResolution);
            if (!skipGlobalBitResolutionCalculation && (_bitResolution > newResolution)) {
                _bitResolution = newResolution;
                DeviceAddress deviceAddr;
                for (int i = 0; i < _devices; i++) {
                    getAddress(deviceAddr, i);
                    _bitResolution = MAX(_bitResolution, getResolution(deviceAddr));
                }
            }
        }
        return true; // new value set
    }

    return false;
}

// sends command for all devices on the bus to perform a temperature conversion

void DallasRmt::requestTemperatures()
{
    _ow->reset();
    _ow->skip();
    _ow->write(STARTCONVO, _parasite);

    // ASYNC mode?
    if (!_waitForConversion) {
        return;
    }
    blockTillConversionComplete(_bitResolution);
}

// sends command for one device to perform a temperature by address
// returns FALSE if device is disconnected
// returns TRUE  otherwise

bool DallasRmt::requestTemperaturesByAddress(const uint8_t* deviceAddress)
{
    uint8_t bitResolution = getResolution(deviceAddress);
    if (bitResolution == 0) {
        return false; //Device disconnected
    }

    _ow->reset();
    _ow->select(deviceAddress);
    _ow->write(STARTCONVO, _parasite);

    // ASYNC mode?
    if (!_waitForConversion) {
        return true;
    }

    blockTillConversionComplete(bitResolution);
    return true;
}

// sends command for one device to perform a temp conversion by index

bool DallasRmt::requestTemperaturesByIndex(uint8_t deviceIndex)
{
    DeviceAddress deviceAddress;
    getAddress(deviceAddress, deviceIndex);

    return requestTemperaturesByAddress(deviceAddress);
}

// returns temperature in 1/128 degrees C or DEVICE_DISCONNECTED_RAW if the
// device's scratch pad cannot be read successfullget_time_since_booty.
// the numeric value of DEVICE_DISCONNECTED_RAW is defined in
// DallasRmt.h. It is a large negative number outside the
// operating range of the device

int16_t DallasRmt::getTemp(const uint8_t* deviceAddress)
{
    ScratchPad scratchPad;
    memset( scratchPad,0,9 );
    if (isConnected(deviceAddress, scratchPad)) {
        return calculateTemperature(deviceAddress, scratchPad);
    }
    return DEVICE_DISCONNECTED_RAW;
}
// returns temperature in degrees C or DEVICE_DISCONNECTED_C if the
// device's scratch pad cannot be read successfully.
// the numeric value of DEVICE_DISCONNECTED_C is defined in
// DallasRmt.h. It is a large negative number outside the
// operating range of the device

float DallasRmt::getTempC(const uint8_t* deviceAddress)
{
    return rawToCelsius(getTemp(deviceAddress));
}

// returns temperature in degrees F or DEVICE_DISCONNECTED_F if the
// device's scratch pad cannot be read successfully.
// the numeric value of DEVICE_DISCONNECTED_F is defined in
// DallasRmt.h. It is a large negative number outside the
// operating range of the device

float DallasRmt::getTempF(const uint8_t* deviceAddress)
{
    return rawToFahrenheit(getTemp(deviceAddress));
}
// Fetch temperature for device index

float DallasRmt::getTempCByIndex(uint8_t deviceIndex)
{
    DeviceAddress deviceAddress;
    if (!getAddress(deviceAddress, deviceIndex)) {
    	_is_connected = false;
        return DEVICE_DISCONNECTED_C;
    }

    return getTempC((uint8_t*) deviceAddress);
}

// Fetch temperature for device index

float DallasRmt::getTempFByIndex(uint8_t deviceIndex)
{
    DeviceAddress deviceAddress;
    if (!getAddress(deviceAddress, deviceIndex)) {
        return DEVICE_DISCONNECTED_F;
    }

    return getTempF((uint8_t*) deviceAddress);
}

bool DallasRmt::isConversionComplete()
{
    uint8_t b = _ow->read_bit();
    return (b == 1);
}

// reads scratchpad and returns fixed-point temperature, scaling factor 2^-7

int16_t DallasRmt::calculateTemperature(const uint8_t* deviceAddress, uint8_t* scratchPad)
{
    int16_t fpTemperature = (((int16_t) scratchPad[TEMP_MSB]) << 11) | (((int16_t) scratchPad[TEMP_LSB]) << 3);

    /*
    DS1820 and DS18S20 have a 9-bit temperature register.
    Resolutions greater than 9-bit can be calculated using the data from
    the temperature, and COUNT REMAIN and COUNT PER °C registers in the
    scratchpad.  The resolution of the calculation depends on the model.
    While the COUNT PER °C register is hard-wired to 16 (10h) in a
    DS18S20, it changes with temperature in DS1820.
    After reading the scratchpad, the TEMP_READ value is obtained by
    truncating the 0.5°C bit (bit 0) from the temperature data. The
    extended resolution temperature can then be calculated using the
    following equation:
                                    COUNT_PER_C - COUNT_REMAIN
    TEMPERATURE = TEMP_READ - 0.25 + --------------------------
                                            COUNT_PER_C
    Hagai Shatz simplified this to integer arithmetic for a 12 bits
    value for a DS18S20, and James Cameron added legacy DS1820 support.
    See - http://myarduinotoy.blogspot.co.uk/2013/02/12bit-result-from-ds18s20.html
     */

    if (deviceAddress[0] == DS18S20MODEL) {
        fpTemperature = ((fpTemperature & 0xfff0) << 3) - 16 +
                (((scratchPad[COUNT_PER_C] - scratchPad[COUNT_REMAIN]) << 7) / scratchPad[COUNT_PER_C]);
    }

    return fpTemperature;
}
// Continue to check if the IC has responded with a temperature
/* In components/newlib/time.c. Returns a monotonic microsecond counter. */
// extern "C"
// uint64_t get_time_since_boot();
// }

void DallasRmt::blockTillConversionComplete(uint8_t bitResolution)
{
    uint32_t delms = 1000 * millisToWaitForConversion(bitResolution);
    if (_checkForConversion && !_parasite) {
        uint64_t now = xTaskGetTickCount(); // get_time_since_boot(); //millis();
        while (!isConversionComplete() && (xTaskGetTickCount() - delms < now));
    } else {
    	 vTaskDelay( delms/1000 / portTICK_PERIOD_MS);
    }
}

// returns number of milliseconds to wait till conversion is complete (based on IC datasheet)

int16_t DallasRmt::millisToWaitForConversion(uint8_t bitResolution)
{
    switch (bitResolution) {
        case 9:
            return 94;
        case 10:
            return 188;
        case 11:
            return 375;
        default:
            return 750;
    }
}
// convert from raw to Celsius

float DallasRmt::rawToCelsius(int16_t raw)
{
    if ( raw <= DEVICE_DISCONNECTED_RAW /* || (raw == 0x2a80) */) {   // after plugin 85C default temp gets reported
    	                                                              // we supress this in isConnected for the fist readout
        return DEVICE_DISCONNECTED_C;
    }
    return (float) raw * 0.0078125;
}

// convert from raw to Fahrenheit

float DallasRmt::rawToFahrenheit(int16_t raw)
{
    if (raw <= DEVICE_DISCONNECTED_RAW) {
        return DEVICE_DISCONNECTED_F;
    }
    // C = RAW/128
    // F = (C*1.8)+32 = (RAW/128*1.8)+32 = (RAW*0.0140625)+32
    return ((float) raw * 0.0140625) + 32;

}
