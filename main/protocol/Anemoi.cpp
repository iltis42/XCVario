#include "Anemoi.h"
#include "SetupNG.h"
#include "logdef.h"

// Anemoi protocol message id's and their message length
static const char *ANEMOI_IDS = "ADMSWadw";
static const int ANEMOI_LEN[] = {9, 14, 5, 5, 10, 9, 14, 10};

static uint8_t crc8ccitt(const void * data, size_t size);
static void anemoiCRC(int &crc, const char c);

dl_control_t Anemoi::nextBytes(const char* c, int len)
{
    char *ptr;
    int pos = _sm._frame.size() - 1; // c already in the buffer

    switch(_sm._state) {
    case START_TOKEN:
        if ( *c == '$' ) { //0x24
            _sm._state = HEADER;
            _sm.reset();
        }
        ESP_LOGD(FNAME, "ANEMOI START_TOKEN");
        break;
    case HEADER:
        ptr = strchr(ANEMOI_IDS, *c);
        if ( ptr != nullptr ) {
            expected_len = ANEMOI_LEN[int(ptr-ANEMOI_IDS)];
            _sm._state = PAYLOAD;
            ESP_LOGD(FNAME, "ANEMOI HEADER %c/%d", *c, expected_len);
        }
        else {
            _sm._state = START_TOKEN;
        }
        break;
    case PAYLOAD:
        if ( pos >= expected_len ) {
            // only start token plus payload
            _sm._state = COMPLETE;
        }
        ESP_LOGD(FNAME, "ANEMOI PAYLOAD");
        break;
    case STOP_TOKEN: // fixme
        if( *c == 0x0a ) {
            ESP_LOGD(FNAME, "ANEMOI STOP_TOKEN %x", *c);
        }
        _sm._state = COMPLETE;
        break;
    default:
        break;
    }
    if ( _sm._state == COMPLETE )
    {
        _sm._state = START_TOKEN;
        ESP_LOGD(FNAME, "ANEMOI COMPLETE %x", _sm._crc);
        if ( _sm._crc == 0 ) {
            // Only process status and wind
            // ESP_LOGI(FNAME, "Port S2 anemoi %c", _framebuffer[1]);
            switch (_sm._frame.at(1)) {
            case 'S':
                parseStatus();
                break;
            case 'W':
            case 'w':
            default:
                parseWind();
                break;
            }
        }
    }

    return NOACTION; // no routing wanted
}


/*
    $S, display rotation, health, firmware revision, 0x0a, CRC
    Health byte:
        [7]Indication mode
            0: Sensor health state upon entering the sensor health mode (active for 0.5 seconds)
            1: Sensor health state during the sensor health checks (active thereafter)
        [6]IMU health
            0: healthy, 1: faulty
        [5]Baro health
        [4]Temp health
        [3]GPS health
        [2]Airdata health
            pitot calibration factor out of bounds
        [1]AHRS health
            gyro bias out of bounds
        [0]Kalman health
            Math error

    e.g.: $S 1 0x88 0x14 0x0a 0xba
 */
void Anemoi::parseStatus()
{
    // extwind_status.set(_framebuffer[3]);
    // ESP_LOGI(FNAME,"Anemoi %d.%d DisplayOrient: %d, Status: %x", *(status+4)>>4, *(status+4)&0xF, *(status+2), *(status+3));
}

/*
    $W/w, B0 - B7, 0x0a, CRC
    ID:
        0x77(w): Page not active; 0x57(W): Page active
    B0-B7:
        B0Wind direction (live), MSB
        B1Wind direction (live), LSB
        B2Wind magnitude (live)
        B3Wind direction (avg), MSB
        B4Wind direction (avg), LSB
        B5Wind magnitude (avg)
        B6Heading, MSB
        B7Heading, LSB

    e.g.: $W ... 0x0a .
 */
void Anemoi::parseWind()
{
    // extwind_inst_dir.set((_framebuffer[2]<<8) + _framebuffer[3]);
    // extwind_inst_speed.set(_framebuffer[4]);
    // extwind_sptc_dir.set((_framebuffer[5]<<8) + _framebuffer[6]);
    // extwind_sptc_speed.set(_framebuffer[7]);
    // ESP_LOGI(FNAME,"WDir %.1f, Wvel %.1f", extwind_inst_dir.get(), extwind_inst_speed.get());
}



/*
 * crc8ccitt
 */
static constexpr const uint8_t CRC_TABLE[] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
    0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
    0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
    0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
    0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
    0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
    0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
    0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
    0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
    0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
    0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
    0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
    0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
    0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
    0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
    0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
    0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

uint8_t crc8ccitt(const void * data, size_t size) {
    uint8_t val = 0;
    uint8_t * pos = (uint8_t *)data;
    uint8_t * end = pos + size;
    while (pos < end) {
        val = CRC_TABLE[val ^ *pos];
        pos++;
    }
    return val;
}

void anemoiCRC(int &crc, const char c)
{
    crc = CRC_TABLE[crc ^ c];
}