#ifndef ONEWIRE_RMT_H
#define ONEWIRE_RMT_H
#include <stdbool.h>
#include <stdint.h>
#include "driver/gpio.h"
#include "onewire_rmt.h"

struct mgos_rmt_onewire* onewire_rmt_create(gpio_num_t pin, rmt_channel_t rmt_rx, rmt_channel_t rmt_tx);
void onewire_rmt_close(struct mgos_rmt_onewire *ow);

bool onewire_rmt_reset(struct mgos_rmt_onewire *ow);
uint8_t onewire_rmt_crc8(const uint8_t *rom, int len);
void onewire_rmt_target_setup(struct mgos_rmt_onewire *ow, const uint8_t family_code);

bool onewire_rmt_next(struct mgos_rmt_onewire *ow, uint8_t *rom, int mode);
void onewire_rmt_select(struct mgos_rmt_onewire *ow, const uint8_t *rom);
void onewire_rmt_skip(struct mgos_rmt_onewire *ow);
void onewire_rmt_search_clean(struct mgos_rmt_onewire *ow);

bool onewire_rmt_read_bit(struct mgos_rmt_onewire *ow);
uint8_t onewire_rmt_read(struct mgos_rmt_onewire *ow);
void onewire_rmt_read_bytes(struct mgos_rmt_onewire *ow, uint8_t *buf, int len);

void onewire_rmt_write_bit(struct mgos_rmt_onewire *ow, int bit);
void onewire_rmt_write(struct mgos_rmt_onewire *ow, const uint8_t data);
void onewire_rmt_write_bytes(struct mgos_rmt_onewire *ow, const uint8_t *buf, int len);

#endif
