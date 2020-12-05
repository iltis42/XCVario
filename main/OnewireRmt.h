#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "driver/gpio.h"
#include "driver/rmt.h"

// *****************************************************************************
// Onewire platform interface

// bus reset: duration of low phase [us]
#define OW_DURATION_RESET 480
// overall slot duration
#define OW_DURATION_SLOT 75
// write 1 slot and read slot durations [us]
#define OW_DURATION_1_LOW    2
#define OW_DURATION_1_HIGH (OW_DURATION_SLOT - OW_DURATION_1_LOW)
// write 0 slot durations [us]
#define OW_DURATION_0_LOW   65
#define OW_DURATION_0_HIGH (OW_DURATION_SLOT - OW_DURATION_0_LOW)
// sample time for read slot
#define OW_DURATION_SAMPLE  (15-2)
// RX idle threshold
// needs to be larger than any duration occurring during write slots
#define OW_DURATION_RX_IDLE (OW_DURATION_SLOT + 2)

// Strong pull-up aka power mode is implemented by the pad's push-pull driver.
// Open-drain configuration is used for normal operation.
// power bus by disabling open-drain:
#define OW_POWER(g) GPIO.pin[g].pad_driver = 0
// de-power bus by enabling open-drain:
#define OW_DEPOWER(g) GPIO.pin[g].pad_driver = 1
// ; gpio_set_pull_mode(g, GPIO_PULLUP_ONLY)

// grouped information for RMT management



typedef struct {
    uint8_t power;
    uint8_t LastDeviceFlag;
    uint8_t LastDiscrepancy;
    uint8_t LastFamilyDiscrepancy;
    unsigned char ROM_NO[8];
} platform_onewire_bus_t;

typedef struct mgos_rmt_onewire {
    gpio_num_t pin;
    uint8_t *res_rom;
    //struct onewire_search_state sst;
    platform_onewire_bus_t sst;
} mgos_ro_t;


typedef struct {
	rmt_channel_t tx, rx;
    RingbufHandle_t rb;
    gpio_num_t gpio;
} ow_rmt_t;


class OnewireRmt
{
public:
    OnewireRmt(gpio_num_t pin, rmt_channel_t rmt_rx,rmt_channel_t rmt_tx);
    ~OnewireRmt() {};

    // Perform a 1-Wire reset cycle. Returns 1 if a device responds
    // with a presence pulse.  Returns 0 if there is no device or the
    // bus is shorted or otherwise held low for more than 250uS
    uint8_t reset(void);

    // Issue a 1-Wire rom select command, you do the reset first.
    void select(const uint8_t rom[8]);

    // Issue a 1-Wire rom skip command, to address all on bus.
    void skip(void);

    // Write a byte. If 'power' is one then the wire is held high at
    // the end for parasitically powered devices. You are responsible
    // for eventually depowering it by calling depower() or doing
    // another read or write.
    void write(uint8_t v, uint8_t power = 0);
    void write_bytes(const uint8_t *buf, uint16_t count, bool power = 0);

    // Read a byte.
    uint8_t read(void);
    void read_bytes(uint8_t *buf, uint16_t count);

    // Write a bit. The bus is always left powered at the end, see
    // note in write() about that.
    void write_bit(uint8_t v);

    // Read a bit.
    uint8_t read_bit(void);

    // Stop forcing power onto the bus. You only need to do this if
    // you used the 'power' flag to write() or used a write_bit() call
    // and aren't about to do another read or write. You would rather
    // not leave this powered if you don't have to, just in case
    // someone shorts your bus.
    void depower(void);

    // Clear the search state so that if will start from the beginning again.
    void reset_search();

    // Setup the search to find the device type 'family_code' on the next call
    // to search(*newAddr) if it is present.
    void target_search(uint8_t family_code);

    // Look for the next device. Returns 1 if a new address has been
    // returned. A zero might mean that the bus is shorted, there are
    // no devices, or you have already retrieved all of them.  It
    // might be a good idea to check the CRC to make sure you didn't
    // get garbage.  The order is deterministic. You will always get
    // the same devices in the same order.
    uint8_t search(uint8_t *newAddr, bool search_mode = true);

    // Compute a Dallas Semiconductor 8 bit CRC, these are used in the
    // ROM and scratchpad registers.
    static uint8_t crc8(const uint8_t *addr, uint8_t len);
private:
    ow_rmt_t   ow_rmt;
    mgos_ro_t  _ow;
    uint8_t owDefaultPower;
    bool onewire_rmt_init(gpio_num_t gpio_num, rmt_channel_t tx_channel, rmt_channel_t rx_channel);
    bool onewire_rmt_attach_pin(gpio_num_t gpio_num);
    void onewire_flush_rmt_rx_buf(void);
    bool onewire_write_bits(gpio_num_t gpio_num, uint8_t data, uint8_t num, uint8_t power);
    bool onewire_read_bits(gpio_num_t gpio_num, uint8_t *data, uint8_t num);
    rmt_item32_t onewire_encode_write_slot(uint8_t val);
    rmt_item32_t onewire_encode_read_slot(void);
    bool onewire_rmt_reset(struct mgos_rmt_onewire *ow);

};
