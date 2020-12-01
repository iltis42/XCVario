#include "OnewireRmt.h"
#include "driver/rmt.h"
#include "onewire_rmt.h"
#include <logdef.h>


OnewireRmt::OnewireRmt(gpio_num_t pin, rmt_channel_t rmt_rx, rmt_channel_t rmt_tx)
{
	ESP_LOGI(FNAME,"OnewireRmt::OnewireRmt");
    _ow = onewire_rmt_create(pin,rmt_rx,rmt_tx);
}

OnewireRmt::~OnewireRmt()
{
    onewire_rmt_close(_ow);
}

uint8_t OnewireRmt::reset(void)
{
	ESP_LOGD(FNAME,"OnewireRmt::reset()");
    return onewire_rmt_reset(_ow);
}

void OnewireRmt::select(const uint8_t rom[8])
{
    onewire_rmt_select(_ow, rom);
}

void OnewireRmt::skip(void)
{
    onewire_rmt_skip(_ow);
}

void OnewireRmt::write(uint8_t v, uint8_t power)
{
    onewire_rmt_write(_ow, v);
    (void) power;
}

void OnewireRmt::write_bytes(const uint8_t *buf, uint16_t count, bool power)
{
    onewire_rmt_write_bytes(_ow, buf, count);
    (void) power;
}

uint8_t OnewireRmt::read(void)
{
    return onewire_rmt_read(_ow);
}

void OnewireRmt::read_bytes(uint8_t *buf, uint16_t count)
{
    onewire_rmt_read_bytes(_ow, buf, count);
}

void OnewireRmt::write_bit(uint8_t v)
{
    onewire_rmt_write_bit(_ow, v);
}

uint8_t OnewireRmt::read_bit(void)
{
    return (uint8_t) onewire_rmt_read_bit(_ow);
}

void OnewireRmt::depower(void)
{

}

void OnewireRmt::reset_search()
{
    onewire_rmt_search_clean(_ow);
}

void OnewireRmt::target_search(uint8_t family_code)
{
    onewire_rmt_target_setup(_ow, family_code);
}

uint8_t OnewireRmt::search(uint8_t *newAddr, bool search_mode)
{
	ESP_LOGD(FNAME,"search( %08x, %d )", (unsigned int)newAddr, search_mode );
	uint8_t searchret = onewire_rmt_next(_ow, newAddr, !search_mode);
	ESP_LOGD(FNAME,"search returned %d", searchret);
    return searchret;
}

uint8_t OnewireRmt::crc8(const uint8_t *addr, uint8_t len)
{
    return onewire_rmt_crc8(addr, len);
}



