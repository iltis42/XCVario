#include "OnewireRmt.h"
#include "driver/rmt.h"
#include <logdef.h>


OnewireRmt::OnewireRmt(gpio_num_t pin, rmt_channel_t rmt_rx, rmt_channel_t rmt_tx)
{
	ESP_LOGI(FNAME,"OnewireRmt::OnewireRmt pin:%d rx:%d tx:%d", (int)pin, (int)rmt_rx, (int)rmt_tx );
	ow_rmt = {RMT_CHANNEL_MAX, RMT_CHANNEL_MAX, NULL, GPIO_NUM_MAX};
	_ow.pin = pin;
	owDefaultPower = 0;
	rmt_channel_t rx = rmt_rx; //mgos_sys_config_get_onewire_rmt_rx_channel();
	rmt_channel_t tx = rmt_tx; //mgos_sys_config_get_onewire_rmt_tx_channel();
	if (RMT_CHANNEL_MAX == rx || RMT_CHANNEL_MAX == tx) {
		ESP_LOGE(FNAME, "onewire_rmt could not start - rx and/or tx channel not set.");
	}
	bool driverOk = onewire_rmt_init(pin, tx, rx);
	if (false == driverOk) {
		ESP_LOGE(FNAME, "onewire_rmt could not start - rmt device could not be configured.");
	}
}


bool OnewireRmt::onewire_rmt_reset(struct mgos_rmt_onewire *ow) {
    rmt_item32_t tx_items[1];
    bool _presence = false;
    gpio_num_t gpio_num = (gpio_num_t)ow->pin;

    if (onewire_rmt_attach_pin(gpio_num) != true) {
    	ESP_LOGE(FNAME, "onewire_rmt_attach_pin returned false, abort reset");
        return false;
    }
    ESP_LOGD(FNAME,"onewire_rmt_reset() OW_DEPOWER( %d)", (int)gpio_num );
    OW_DEPOWER(gpio_num);

    tx_items[0].duration0 = OW_DURATION_RESET;
    tx_items[0].level0 = 0;
    tx_items[0].duration1 = 0;
    tx_items[0].level1 = 1;

    uint16_t old_rx_thresh;
    rmt_get_rx_idle_thresh(ow_rmt.rx, &old_rx_thresh);
    rmt_set_rx_idle_thresh(ow_rmt.rx, OW_DURATION_RESET + 60);

    ESP_LOGD(FNAME, "Now rmt_rx_start()");
    onewire_flush_rmt_rx_buf();
    rmt_rx_start(ow_rmt.rx, true);
    if (rmt_write_items(ow_rmt.tx, tx_items, 1, true) == ESP_OK) {
    	// OW_DEPOWER(gpio_num);
    	ESP_LOGD(FNAME, "rmt_write_items() ch:%d OK", (int)ow_rmt.tx );
    	size_t rx_size;
    	rmt_item32_t* rx_items = (rmt_item32_t *) xRingbufferReceive(ow_rmt.rb, &rx_size, 100 / portTICK_PERIOD_MS);
    	if (rx_items) {
    		ESP_LOGD(FNAME,"there are rx_items, size %d", rx_size );
    		if (rx_size >= 1 * sizeof ( rmt_item32_t)) {
    			for (int i = 0; i < rx_size / 4; i++) {
    				ESP_LOGD(FNAME, "item[%d] l0:%d d0:%d l1:%d d1:%d", i, rx_items[i].level0, rx_items[i].duration0, rx_items[i].level1, rx_items[i].duration1);
    			}
    			ESP_LOGD(FNAME,"Now parse signal and search for presence pulse l0:%d d0:%d l1:%d d1:%d", rx_items[0].level0, rx_items[0].duration0, rx_items[0].level1, rx_items[0].duration1 );
    			if ((rx_items[0].level0 == 0) && (rx_items[0].duration0 >= OW_DURATION_RESET - 20)) {
    				ESP_LOGD(FNAME,"OW duration0 > OW_DURATION_RESET (480) fine");
    				if ((rx_items[0].level1 == 1) && (rx_items[0].duration1 > 0)) {
    					ESP_LOGD(FNAME,"level1 == 1 && duration1 > 0");
    					if (rx_items[1].level0 == 0) {
    						ESP_LOGD(FNAME,"presence = true");
    						_presence = true;
    					}
    					else
    						ESP_LOGD(FNAME,"OW level 0 not reached");
    				}
    				else
    					ESP_LOGD(FNAME,"OW level 1 not reached");
    			}
    			else
    				ESP_LOGW(FNAME,"OW reset duration out of bounds");
    		}
    		else
    		{
    			ESP_LOGW(FNAME,"rx_items size failed to match expected size: %d", sizeof( rmt_item32_t) );
    		}
    		vRingbufferReturnItem(ow_rmt.rb, (void *) rx_items);
    	}
    	else {
    		// time out occurred, this indicates an unconnected / misconfigured bus
    		ESP_LOGE(FNAME, "rmt_write_items() time out occurred, this indicates an unconnected / misconfigured bus");
        	_presence = false;
        }

    }
    else {
    	ESP_LOGE(FNAME, "error in tx channel");
    	_presence = false;
    }

    rmt_rx_stop(ow_rmt.rx);
    rmt_set_rx_idle_thresh(ow_rmt.rx, old_rx_thresh);

    ESP_LOGD(FNAME, "onewire_rmt_reset returns: %d", (int)_presence );
    return _presence;
}

rmt_item32_t OnewireRmt::onewire_encode_write_slot(uint8_t val) {
    rmt_item32_t item;

    item.level0 = 0;
    item.level1 = 1;
    if (val) {
        // write "1" slot
        item.duration0 = OW_DURATION_1_LOW;
        item.duration1 = OW_DURATION_1_HIGH;
    } else {
        // write "0" slot
        item.duration0 = OW_DURATION_0_LOW;
        item.duration1 = OW_DURATION_0_HIGH;
    }

    return item;
}

rmt_item32_t OnewireRmt::onewire_encode_read_slot(void) {
    rmt_item32_t item;

    // construct pattern for a single read time slot
    item.level0 = 0;
    item.duration0 = OW_DURATION_1_LOW; // shortly force 0
    item.level1 = 1;
    item.duration1 = OW_DURATION_1_HIGH; // release high and finish slot

    return item;
}

void OnewireRmt::onewire_flush_rmt_rx_buf(void) {
	void *p;
	size_t s;

	while ((p = xRingbufferReceive(ow_rmt.rb, &s, 0))) {
		vRingbufferReturnItem(ow_rmt.rb, p);
	}
}

bool OnewireRmt::onewire_read_bits(gpio_num_t gpio_num, uint8_t *data, uint8_t num) {
    rmt_item32_t tx_items[num + 1];
    uint8_t read_data = 0;
    int res = true;

    if (num > 8) {
        return false;
    }

    if (onewire_rmt_attach_pin(gpio_num) != true) {
        return false;
    }

    OW_DEPOWER(gpio_num);

    // generate requested read slots
    for (int i = 0; i < num; i++) {
        tx_items[i] = onewire_encode_read_slot();
    }
    // end marker
    tx_items[num].level0 = 1;
    tx_items[num].duration0 = 0;

    onewire_flush_rmt_rx_buf();
    rmt_rx_start(ow_rmt.rx, true);
    if (rmt_write_items(ow_rmt.tx, tx_items, num + 1, true) == ESP_OK) {
        size_t rx_size;
        rmt_item32_t* rx_items = (rmt_item32_t *) xRingbufferReceive(ow_rmt.rb, &rx_size, portMAX_DELAY);

        if (rx_items) {
            for (int i = 0; i < rx_size / 4; i++) {
                ESP_LOGD(FNAME, "level: %d, duration %d", rx_items[i].level0, rx_items[i].duration0);
                ESP_LOGD(FNAME, "level: %d, duration %d", rx_items[i].level1, rx_items[i].duration1);
            }

            if (rx_size >= num * sizeof ( rmt_item32_t)) {
                for (int i = 0; i < num; i++) {
                    read_data >>= 1;
                    // parse signal and identify logical bit
                    if (rx_items[i].level1 == 1) {
                        if ((rx_items[i].level0 == 0) && (rx_items[i].duration0 < OW_DURATION_SAMPLE)) {
                            // rising edge occured before 15us -> bit 1
                            read_data |= 0x80;
                        }
                    }
                }
                read_data >>= 8 - num;
            }

            vRingbufferReturnItem(ow_rmt.rb, (void *) rx_items);
        } else {
            // time out occurred, this indicates an unconnected / misconfigured bus
            res = false;
        }

    } else {
        // error in tx channel
        res = false;
    }
    rmt_rx_stop(ow_rmt.rx);
    *data = read_data;
    return res;
}

bool OnewireRmt::onewire_write_bits(gpio_num_t gpio_num, uint8_t data, uint8_t num, uint8_t power) {
    rmt_item32_t tx_items[num + 1];

    if (num > 8) {
        return false;
    }

    if (onewire_rmt_attach_pin(gpio_num) != true) {
        return false;
    }

    if (power) {
        // apply strong driver to power the bus
        OW_POWER(gpio_num);
    } else {
        // switch to open-drain mode, bus is powered by external pull-up
        OW_DEPOWER(gpio_num);
    }

    // write requested bits as pattern to TX buffer
    for (int i = 0; i < num; i++) {
        tx_items[i] = onewire_encode_write_slot(data & 0x01);
        data >>= 1;
    }
    // end marker
    tx_items[num].level0 = 1;
    tx_items[num].duration0 = 0;

    if (rmt_write_items(ow_rmt.tx, tx_items, num + 1, true) == ESP_OK) {
        return true;
    } else {
        return false;
    }
}


bool OnewireRmt::onewire_rmt_attach_pin(gpio_num_t gpio_num) {
	ESP_LOGD(FNAME, "onewire_rmt_attach_pin %d", gpio_num );
    if (ow_rmt.tx < 0 || ow_rmt.rx < 0)
        return false;

    if (gpio_num != ow_rmt.gpio) {
        // attach GPIO to previous pin
        if (gpio_num < 32) {
            GPIO.enable_w1ts = (0x1 << gpio_num);
        } else {
            GPIO.enable1_w1ts.data = (0x1 << (gpio_num - 32));
        }
        if (ow_rmt.gpio >= 0) {
            gpio_matrix_out(ow_rmt.gpio, SIG_GPIO_OUT_IDX, 0, 0);
        }

        // attach RMT channels to new gpio pin
        // ATTENTION: set pin for rx first since gpio_output_disable() will
        //            remove rmt output signal in matrix!
        rmt_set_pin(ow_rmt.rx, RMT_MODE_RX, gpio_num);
        rmt_set_pin(ow_rmt.tx, RMT_MODE_TX, gpio_num);
        // force pin direction to input to enable path to RX channel
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[gpio_num]);

        ow_rmt.gpio = gpio_num;
    }

    return true;
}


bool OnewireRmt::onewire_rmt_init(gpio_num_t gpio_num, rmt_channel_t tx_channel, rmt_channel_t rx_channel) {
    ow_rmt.tx = tx_channel;
    ow_rmt.rx = rx_channel;
    // acquire an RMT module for TX and RX each
    ESP_LOGD(FNAME, "RMT TX channel: %d", ow_rmt.tx);
    ESP_LOGD(FNAME, "RMT RX channel: %d", ow_rmt.rx);
    ESP_LOGD(FNAME, "RMT TX channel: %d, RX channel: %d", ow_rmt.tx, ow_rmt.rx);
    rmt_config_t rmt_tx;
    rmt_tx.channel = ow_rmt.tx;
    rmt_tx.gpio_num = gpio_num;
    rmt_tx.mem_block_num = 1;
    rmt_tx.clk_div = 80;
    rmt_tx.tx_config.loop_en = false;
    rmt_tx.tx_config.carrier_en = false;
    rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_HIGH;
    rmt_tx.tx_config.idle_output_en = true;
    rmt_tx.rmt_mode = RMT_MODE_TX;
    if (rmt_config(&rmt_tx) == ESP_OK) {
        if (rmt_driver_install(rmt_tx.channel, 0, ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_SHARED) == ESP_OK) {
            rmt_config_t rmt_rx;
            rmt_rx.channel = ow_rmt.rx;
            rmt_rx.gpio_num = gpio_num;
            rmt_rx.clk_div = 80;
            rmt_rx.mem_block_num = 1;
            rmt_rx.rmt_mode = RMT_MODE_RX;
            rmt_rx.rx_config.filter_en = true;
            rmt_rx.rx_config.filter_ticks_thresh = 30;
            rmt_rx.rx_config.idle_threshold = OW_DURATION_RX_IDLE;
            if (rmt_config(&rmt_rx) == ESP_OK) {
                if (rmt_driver_install(rmt_rx.channel, 512, ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_SHARED) == ESP_OK) {
                    rmt_get_ringbuf_handle(ow_rmt.rx, &ow_rmt.rb);
                    // don't set ow_rmt.gpio here
                    // -1 forces a full pin set procedure in first call to onewire_rmt_attach_pin()
                    return true;
                }
            }
            rmt_driver_uninstall(rmt_tx.channel);
        }
    }
    return false;
}


uint8_t OnewireRmt::reset(void)
{
	ESP_LOGD(FNAME,"OnewireRmt::reset()");
	rmt_item32_t tx_items[1];
	bool _presence = false;
	gpio_num_t gpio_num = (gpio_num_t)_ow.pin;

	if (onewire_rmt_attach_pin(gpio_num) != true) {
		ESP_LOGE(FNAME, "onewire_rmt_attach_pin returned false, abort reset");
		return false;
	}
	ESP_LOGD(FNAME,"onewire_rmt_reset() OW_DEPOWER( %d)", (int)gpio_num );
	OW_DEPOWER(gpio_num);

	tx_items[0].duration0 = OW_DURATION_RESET;
	tx_items[0].level0 = 0;
	tx_items[0].duration1 = 0;
	tx_items[0].level1 = 1;

	uint16_t old_rx_thresh;
	rmt_get_rx_idle_thresh(ow_rmt.rx, &old_rx_thresh);
	rmt_set_rx_idle_thresh(ow_rmt.rx, OW_DURATION_RESET + 60);

	ESP_LOGD(FNAME, "Now rmt_rx_start()");
	onewire_flush_rmt_rx_buf();
	rmt_rx_start(ow_rmt.rx, true);
	if (rmt_write_items(ow_rmt.tx, tx_items, 1, true) == ESP_OK) {
		// OW_DEPOWER(gpio_num);
		ESP_LOGD(FNAME, "rmt_write_items() ch:%d OK", (int)ow_rmt.tx );
		size_t rx_size;
		rmt_item32_t* rx_items = (rmt_item32_t *) xRingbufferReceive(ow_rmt.rb, &rx_size, 100 / portTICK_PERIOD_MS);
		if (rx_items) {
			ESP_LOGD(FNAME,"there are rx_items, size %d", rx_size );
			if (rx_size >= 1 * sizeof ( rmt_item32_t)) {
				for (int i = 0; i < rx_size / 4; i++) {
					ESP_LOGD(FNAME, "item[%d] l0:%d d0:%d l1:%d d1:%d", i, rx_items[i].level0, rx_items[i].duration0, rx_items[i].level1, rx_items[i].duration1);
				}
				ESP_LOGD(FNAME,"Now parse signal and search for presence pulse l0:%d d0:%d l1:%d d1:%d", rx_items[0].level0, rx_items[0].duration0, rx_items[0].level1, rx_items[0].duration1 );
				if ((rx_items[0].level0 == 0) && (rx_items[0].duration0 >= OW_DURATION_RESET - 20)) {
					ESP_LOGD(FNAME,"OW duration0 > OW_DURATION_RESET (480) fine");
					if ((rx_items[0].level1 == 1) && (rx_items[0].duration1 > 0)) {
						ESP_LOGD(FNAME,"level1 == 1 && duration1 > 0");
						if (rx_items[1].level0 == 0) {
							ESP_LOGD(FNAME,"presence = true");
							_presence = true;
						}
						else
							ESP_LOGD(FNAME,"OW level 0 not reached");
					}
					else
						ESP_LOGD(FNAME,"OW level 1 not reached");
				}
				else
					ESP_LOGW(FNAME,"OW reset duration out of bounds");
			}
			else
			{
				ESP_LOGW(FNAME,"rx_items size failed to match expected size: %d", sizeof( rmt_item32_t) );
			}
			vRingbufferReturnItem(ow_rmt.rb, (void *) rx_items);
		}
		else {
			// time out occurred, this indicates an unconnected / misconfigured bus
			ESP_LOGE(FNAME, "rmt_write_items() time out occurred, this indicates an unconnected / misconfigured bus");
			_presence = false;
		}

	}
	else {
		// error in tx channel
		// res = false;
		ESP_LOGE(FNAME, "error in tx channel");
		_presence = false;
	}

	rmt_rx_stop(ow_rmt.rx);
	rmt_set_rx_idle_thresh(ow_rmt.rx, old_rx_thresh);

	ESP_LOGD(FNAME, "reset returns: %d", (int)_presence );
	return _presence;
}

void OnewireRmt::select(const uint8_t rom[8])
{
	onewire_write_bits(_ow.pin, 0x55, 8, owDefaultPower);
	for (int i = 0; i < 8; i++) {
		onewire_write_bits(_ow.pin, rom[i], 8, owDefaultPower);
	}
}

void OnewireRmt::skip(void)
{
	onewire_write_bits(_ow.pin, 0xCC, 8, owDefaultPower);
}

void OnewireRmt::write(uint8_t data, uint8_t power)
{
	onewire_write_bits(_ow.pin, data, 8, power);
}

void OnewireRmt::write_bytes(const uint8_t *buf, uint16_t count, bool power)
{
	for (uint16_t i = 0; i < count; i++) {
		if (onewire_write_bits(_ow.pin, buf[i], 8, power) != true) {
			ESP_LOGE(FNAME, "OnewireRmt::write_bytes error");
			break;
		}
	}
}

uint8_t OnewireRmt::read(void)
{
	uint8_t res = 0;
	if (onewire_read_bits(_ow.pin, &res, 8) != true) {
		return 0;
	}
	return res;
}

void OnewireRmt::read_bytes(uint8_t *buf, uint16_t count)
{

	for (uint16_t i = 0; i < count; i++) {
		if (onewire_read_bits(_ow.pin, buf, 8) != true) {
			ESP_LOGE(FNAME, "OnewireRmt::write_bytes error");
			break;
		}
		buf++;
	}
}

void OnewireRmt::write_bit(uint8_t bit)
{
	uint8_t data = 0x01 & bit;
	onewire_write_bits(_ow.pin, data, 1, owDefaultPower);
}

uint8_t OnewireRmt::read_bit(void)
{
	uint8_t bit = 0;
	if (onewire_read_bits(_ow.pin, &bit, 1)) {
		return bit & 0x01;
	}
	return false;
}

void OnewireRmt::depower(void)
{

}

void OnewireRmt::reset_search()
{
	memset(&_ow.sst, 0, sizeof (_ow.sst));
}

void OnewireRmt::target_search(uint8_t family_code)
{
	// set the search state to find SearchFamily type devices
	_ow.sst.ROM_NO[0] = family_code;
	uint8_t i;
	for (i = 1; i < 8; i++) {
		_ow.sst.ROM_NO[i] = 0;
	}
	_ow.sst.LastDiscrepancy = 64;
	_ow.sst.LastFamilyDiscrepancy = 0;
	_ow.sst.LastDeviceFlag = false;
}

uint8_t OnewireRmt::search(uint8_t *rom, bool search_mode)
{
	ESP_LOGD(FNAME,"search( %08x, %d )", (unsigned int)rom, search_mode );
	uint8_t id_bit_number;
	uint8_t last_zero, rom_byte_number, search_result;
	uint8_t id_bit, cmp_id_bit;

	unsigned char rom_byte_mask, search_direction;

	// initialize for search
	id_bit_number = 1;
	last_zero = 0;
	rom_byte_number = 0;
	rom_byte_mask = 1;
	search_result = 0;

	ESP_LOGD(FNAME,"Check if the last call was not the last one");
	if (!_ow.sst.LastDeviceFlag) {
		// 1-Wire reset
		ESP_LOGD(FNAME,"Nope, now do the one wire reset");
		//uint8_t presence;
		if (onewire_rmt_reset(&_ow) != true) {
			ESP_LOGD(FNAME,"The one wire reset returned false, abort");
			// reset the search
			_ow.sst.LastDiscrepancy = 0;
			_ow.sst.LastDeviceFlag = false;
			_ow.sst.LastFamilyDiscrepancy = 0;
			return false;
		}
		ESP_LOGD(FNAME,"one wire reset fine, now search pin %d", (int)_ow.pin );
		// issue the search command
		onewire_write_bits( (gpio_num_t)_ow.pin, 0xF0, 8, owDefaultPower);

		// loop to do the search
		do {
			// read a bit and its complement
			if (onewire_read_bits((gpio_num_t)_ow.pin, &id_bit, 1) != true) {
				ESP_LOGW(FNAME,"onewire_read_bits() bit returned false");
				break;
			}
			if (onewire_read_bits((gpio_num_t)_ow.pin, &cmp_id_bit, 1) != true) {
				ESP_LOGW(FNAME,"onewire_read_bits() complement returned false");
				break;
			}

			// check for no devices on 1-wire
			if ((id_bit == 1) && (cmp_id_bit == 1)) {
				ESP_LOGW(FNAME,"both bit are one, maybe no device there, break");
				break;
			}
			else {
				ESP_LOGD(FNAME,"all devices coupled have 0 or 1");
				if (id_bit != cmp_id_bit) {
					search_direction = id_bit; // bit write value for search
					ESP_LOGV(FNAME,"id_bit != cmp_id_bit");
				}
				else {
					ESP_LOGD(FNAME,"if this discrepancy if before the Last Discrepancy");
					// on a previous next then pick the same as last time
					if (id_bit_number < _ow.sst.LastDiscrepancy) {
						ESP_LOGD(FNAME,"id_bit_number < _ow.sst.LastDiscrepancy");
						search_direction = ((_ow.sst.ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
					}
					else {
						ESP_LOGD(FNAME,"pick 1");
						search_direction = (id_bit_number == _ow.sst.LastDiscrepancy);
					}
					// if 0 was picked then record its position in LastZero
					if (search_direction == 0) {
						ESP_LOGD(FNAME,"pick 0");
						last_zero = id_bit_number;

						// check for Last discrepancy in family
						if (last_zero < 9) {
							ESP_LOGD(FNAME,"last_zero <9");
							_ow.sst.LastFamilyDiscrepancy = last_zero;
						}
					}
				}

				// set or clear the bit in the ROM byte rom_byte_number
				// with mask rom_byte_mask
				if (search_direction == 1) {
					ESP_LOGD(FNAME,"search_direction == 1");
					_ow.sst.ROM_NO[rom_byte_number] |= rom_byte_mask;
				} else {
					ESP_LOGD(FNAME,"search_direction == 0");
					_ow.sst.ROM_NO[rom_byte_number] &= ~rom_byte_mask;
				}

				// serial number search direction write bit
				onewire_write_bits(_ow.pin, search_direction, 1, owDefaultPower);

				// increment the byte counter id_bit_number
				// and shift the mask rom_byte_mask
				id_bit_number++;
				ESP_LOGD(FNAME,"id_bit_number++ %d", id_bit_number );
				rom_byte_mask <<= 1;

				// if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
				if (rom_byte_mask == 0) {
					ESP_LOGD(FNAME,"mask is 0 then go to new SerialNum byte rom_byte_number and reset mask");
					rom_byte_number++;
					rom_byte_mask = 1;
				}
			}
		} while (rom_byte_number < 8); // loop until through all ROM bytes 0-7

		// if the search was successful then
		if (!(id_bit_number < 65)) {
			// search successful so set LastDiscrepancy,LastDeviceFlag,search_result
			_ow.sst.LastDiscrepancy = last_zero;

			// check for last device
			if (_ow.sst.LastDiscrepancy == 0) {
				_ow.sst.LastDeviceFlag = true;
			}
			ESP_LOGD(FNAME,"set search_result = true");
			search_result = true;
		}
	}

	// if no device found then reset counters so next 'search' will be like a first
	if (!search_result || !_ow.sst.ROM_NO[0]) {
		_ow.sst.LastDiscrepancy = 0;
		_ow.sst.LastDeviceFlag = false;
		_ow.sst.LastFamilyDiscrepancy = 0;
		search_result = false;
		ESP_LOGD(FNAME,"search_result = false, reset counters");
	} else {
		ESP_LOGD(FNAME,"search_result = true, dump rom bytes:");
		for (rom_byte_number = 0; rom_byte_number < 8; rom_byte_number++) {
			rom[rom_byte_number] = _ow.sst.ROM_NO[rom_byte_number];
			ESP_LOGD(FNAME,"rom_byte[%d]=%02X", rom_byte_number, _ow.sst.ROM_NO[rom_byte_number] );
		}
	}
	ESP_LOGD(FNAME,"search returned %d", search_result);
	return search_result;
}

static const uint8_t crc_table[] = {
    0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31,
    65, 157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96,
    130, 220, 35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128,
    222, 60, 98, 190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158,
    29, 67, 161, 255, 70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56,
    102, 229, 187, 89, 7, 219, 133, 103, 57, 186, 228, 6, 88, 25, 71,
    165, 251, 120, 38, 196, 154, 101, 59, 217, 135, 4, 90, 184, 230, 167,
    249, 27, 69, 198, 152, 122, 36, 248, 166, 68, 26, 153, 199, 37, 123,
    58, 100, 134, 216, 91, 5, 231, 185, 140, 210, 48, 110, 237, 179, 81,
    15, 78, 16, 242, 172, 47, 113, 147, 205, 17, 79, 173, 243, 112, 46,
    204, 146, 211, 141, 111, 49, 178, 236, 14, 80, 175, 241, 19, 77, 206,
    144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238, 50, 108, 142, 208,
    83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115, 202, 148, 118,
    40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139, 87, 9,
    235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22, 233,
    183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
    116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107,
    53
};


uint8_t OnewireRmt::crc8(const uint8_t *addr, uint8_t len)
{
	uint8_t res = 0x00;
	while (len-- > 0) {
		res = crc_table[res ^ *addr++];
	}
	return res;
}



