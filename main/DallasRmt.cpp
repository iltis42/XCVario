#include "DallasRmt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <esp_log.h>
#include <time.h>
#include <string.h>
#include <logdef.h>


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


DallasRmt::DallasRmt( OnewireRmt* ow )
{
	_ow = ow;
	_parasite = false;
	_bitResolution = 12;
	_waitForConversion = true;
	_checkForConversion = true;
}

DallasRmt::~DallasRmt()
{
	if (_ow) {
		delete _ow;
	}
}

// initialise the bus

void DallasRmt::begin(void)
{
	ESP_LOGD(FNAME, "begin");
	_ow->reset_search();
	_devices = 0; // Reset the number of devices when we enumerate wire devices
	memset( scratchPad,0,9 );
	memset( deviceAddress,0,8 );
	if( getAddress(0) ){
		setResolution( deviceAddress, 12, true );
	}
	errors=0;
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

bool DallasRmt::getAddress(uint8_t index)
{
	uint8_t depth = 0;
	_ow->reset_search();
	while (depth <= index && _ow->search(deviceAddress)) {
		if (depth == index && validAddress(deviceAddress))
			// ESP_LOGI(FNAME,"T Sensor device found %X", *deviceAddress );
		return true;
		depth++;
	}
	// ESP_LOGI(FNAME,"T Sensor device NOT found");
	return false;
}

// attempt to determine if the device at the given address is connected to the bus

bool DallasRmt::isConnected(const uint8_t* deviceAddress)
{
	return isConnected(deviceAddress, scratchPad);
}

// attempt to determine if the device at the given address is connected to the bus
// also allows for updating the read scratchpad


bool DallasRmt::isConnected(const uint8_t* deviceAddress, uint8_t* scratchPad)
{
	bool readok = readScratchPad(deviceAddress, scratchPad);
	bool crc = true;
	bool ret = true;
	if( readok ) {
		int cs = _ow->crc8(scratchPad, 8);
		// ESP_LOG_BUFFER_HEXDUMP(FNAME,scratchPad,9, ESP_LOG_INFO);
		if( scratchPad[SCRATCHPAD_CRC] != cs ) {
			crc = false;
			ESP_LOGW(FNAME,"T Sensor checksum error %d != %d consecutive errors: %d", cs, scratchPad[SCRATCHPAD_CRC], errors );
		}
	}
	if( crc && readok ){  // crc okay
		if( errors )
			errors = 0;
		    _devices = 1;
	}
	else{ // crc error
		errors++;
		if( errors > 5 || _devices == 0 ){
			_is_connected = false;
			ret = false;
			_devices = 0;
		}
	}
	return ret;
}

bool DallasRmt::readScratchPad(const uint8_t* deviceAddress, uint8_t* scratchPad)
{
	// ESP_LOGI(FNAME,"readScratchPad");
	// ESP_LOG_BUFFER_HEXDUMP(FNAME,deviceAddress,8, ESP_LOG_INFO);
	// send the reset command and fail fast
	bool ret = _ow->reset();
	if( !ret ){
		// ESP_LOGW(FNAME,"T Sensor reset failed");
		return false;
	}
	_ow->select(deviceAddress);
	ret = _ow->write(READSCRATCH);
	if( !ret ){
		// ESP_LOGW(FNAME,"T Sensor write failed");
		return false;
	}
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
	ret = _ow->read_bytes(scratchPad, 9);
	if( !ret ){
		// ESP_LOGW(FNAME,"T Sensor read failed");
		return false;
	}
	// ESP_LOG_BUFFER_HEXDUMP(FNAME,scratchPad,9, ESP_LOG_INFO);
	return true;
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
	ESP_LOGI(FNAME, "setResolution: %d", newResolution);
	_bitResolution = (newResolution < 9) ? 9 : (newResolution > 12 ? 12 : newResolution);
	setResolution(deviceAddress, _bitResolution, true);
}

// set resolution of a device to 9, 10, 11, or 12 bits
// if new resolution is out of range, 9 bits is used.

bool DallasRmt::setResolution(const uint8_t* deviceAddress, uint8_t newResolution, bool skipGlobalBitResolutionCalculation)
{
	// ensure same behavior as setResolution(uint8_t newResolution)
	ESP_LOGI(FNAME, "setResolution: %d", newResolution);
	newResolution = (newResolution < 9) ? 9 : (newResolution > 12 ? 12 : newResolution);

	// return when stored value == new value
	if (getResolution(deviceAddress) == newResolution) {
		return true;
	}
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
			_bitResolution = std::max(_bitResolution, newResolution);
			if (!skipGlobalBitResolutionCalculation && (_bitResolution > newResolution)) {
				_bitResolution = newResolution;
				_bitResolution = std::max(_bitResolution, getResolution(deviceAddress));
			}
		}
		return true; // new value set
	}

	return false;
}

// sends command for all devices on the bus to perform a temperature conversion

void DallasRmt::requestTemperatures()
{
	// ESP_LOGI(FNAME,"requestTemperatures()  wait: %d  br:%d", _waitForConversion, _bitResolution );
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
	return requestTemperaturesByAddress(deviceAddress);
}

// returns temperature in 1/128 degrees C or DEVICE_DISCONNECTED_RAW if the
// device's scratch pad cannot be read successfullget_time_since_booty.
// the numeric value of DEVICE_DISCONNECTED_RAW is defined in
// DallasRmt.h. It is a large negative number outside the
// operating range of the device

int16_t DallasRmt::getTemp(const uint8_t* deviceAddress)
{
	if (isConnected(deviceAddress, scratchPad)) {
		return calculateTemperature(deviceAddress, scratchPad);
	}
	getAddress(0);
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
	return getTempC((uint8_t*) deviceAddress);
}

// Fetch temperature for device index

float DallasRmt::getTempFByIndex(uint8_t deviceIndex)
{
	return getTempF((uint8_t*) deviceAddress);
}

bool DallasRmt::isConversionComplete()
{
	return( _ow->read_bit() );
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
	// ESP_LOGD(FNAME,"blockTillConversionComplete() %d",bitResolution );
	vTaskDelay(  millisToWaitForConversion(bitResolution) / portTICK_PERIOD_MS );
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
