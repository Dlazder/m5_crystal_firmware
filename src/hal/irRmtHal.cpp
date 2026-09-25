/*
 * irRmtHal.cpp — RMT-based IR (ESP-IDF 4.x, driver/rmt.h)
 *
 * Only compiled when IR_USE_RMT is defined (see platformio.ini, M5StickS3 env).
 * Other boards use the IRremote-based path in the IR loop files.
 */

#ifdef IR_USE_RMT

#include "irRmtHal.h"

volatile uint8_t irTX = 0;
volatile uint8_t irRX = 0;
static volatile bool irTaskStop = false;
static TaskHandle_t rxTaskHandle = NULL;
static rmt_channel_t rxChannel = (rmt_channel_t)-1;
static rmt_channel_t txChannel = (rmt_channel_t)-1;

const ir_protocol_t proto[PROTO_COUNT] = {
	[UNK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, "UNK" },
	[NEC] = { 9000, 4500, 560, 1690, 560, 560, 560, 0, 38000, "NEC" },
	[SONY] = { 2400, 600, 1200, 600, 600, 600, 0, 0, 40000, "SONY" },
	[SAM] = { 4500, 4500, 560, 1690, 560, 560, 560, 0, 38000, "SAM" },
	[RC5] = { 0, 0, 889, 889, 889, 889, 0, 0, 38000, "RC5" },
};

static const uint8_t bitMargin = 200; // wider tolerance for real-world remotes

// ── Decoders ──

bool checkbit(rmt_symbol_word_t &item, uint16_t high, uint16_t low) {
	if (item.level0 != 0 || item.level1 == 0) return false;
	if (item.duration0 > (high + bitMargin) ||
		item.duration0 < (high - bitMargin)) return false;
	// When low==0 (e.g. NEC footer), skip the space-duration check
	// to avoid unsigned underflow: (0 - bitMargin) wraps to 65336
	if (low > 0) {
		if (item.duration1 > (low + bitMargin) ||
			item.duration1 < (low - bitMargin)) return false;
	}
	return true;
}

bool rc5_bit(uint32_t d, uint32_t v) {
	return (d < (v + bitMargin)) && (d > (v - bitMargin));
}

uint32_t nec_check(rmt_symbol_word_t *item, size_t &len) {
	if (len < 34) return 0;
	const uint32_t m = 0x80000000;
	uint32_t code = 0;
	for (uint8_t i = 0; i < 34; i++) {
		if (i == 0) {
			if (!checkbit(item[i], proto[NEC].header_high, proto[NEC].header_low)) return 0;
		} else if (i == 33) {
			if (!checkbit(item[i], proto[NEC].footer_high, proto[NEC].footer_low)) return 0;
		} else if (checkbit(item[i], proto[NEC].one_high, proto[NEC].one_low)) {
			code |= (m >> (i - 1));
		} else if (!checkbit(item[i], proto[NEC].zero_high, proto[NEC].zero_low)) {
			return 0;
		}
	}
	return code; // transmission order: [addr, ~addr, cmd, ~cmd] each byte LSB-first
}

uint32_t sam_check(rmt_symbol_word_t *item, size_t &len) {
	if (len < 34) return 0;
	const uint32_t m = 0x80000000;
	uint32_t code = 0;
	for (uint8_t i = 0; i < 34; i++) {
		if (i == 0) {
			if (!checkbit(item[i], proto[SAM].header_high, proto[SAM].header_low)) return 0;
		} else if (i == 33) {
			if (!checkbit(item[i], proto[SAM].footer_high, proto[SAM].footer_low)) return 0;
		} else if (checkbit(item[i], proto[SAM].one_high, proto[SAM].one_low)) {
			code |= (m >> (i - 1));
		} else if (!checkbit(item[i], proto[SAM].zero_high, proto[SAM].zero_low)) {
			return 0;
		}
	}
	return code;
}

uint32_t sony_check(rmt_symbol_word_t *item, size_t &len) {
	uint8_t i = 0;
	if (len < 12 || !checkbit(item[i], proto[SONY].header_high, proto[SONY].header_low))
		return 0;
	i++;
	uint32_t m = 0x80000000;
	uint32_t code = 0;
	uint8_t maxData = 20;
	if (len < maxData) maxData = 15;
	if (len < maxData) maxData = 12;
	for (int j = 0; j < maxData - 1; j++) {
		if (checkbit(item[i], proto[SONY].one_high, proto[SONY].one_low)) {
			code |= (m >> j);
		} else if (checkbit(item[i], proto[SONY].zero_high, proto[SONY].zero_low)) {
		} else if (item[i].duration1 > 15000) {
			break;
		} else {
			return 0;
		}
		i++;
	}
	code = code >> (32 - i);
	len = i + 1;
	return code;
}

uint32_t rc5_check(rmt_symbol_word_t *item, size_t &len) {
	if (len < 13 || len > 30) return 0;
	const uint16_t RC5_High = proto[RC5].one_high + proto[RC5].one_low;
	uint32_t code = 0;
	bool c = false;
	for (uint8_t i = 0; i < len; i++) {
		uint32_t d0 = item[i].duration0;
		uint32_t d1 = item[i].duration1;
		if (rc5_bit(d0, proto[RC5].one_low)) {
			code = (code << 1) | c;
			c = rc5_bit(d1, RC5_High) ? !c : c;
		} else if (rc5_bit(d0, RC5_High)) {
			code = (code << 2) | (item[i].level0 << 1) | !item[i].level0;
			c = rc5_bit(d1, proto[RC5].one_low) ? !c : c;
		} else {
			return 0;
		}
	}
	return code;
}

// ── Probe for first usable RMT channel ──

static rmt_channel_t rmt_find_free_channel(rmt_mode_t mode) {
	for (int ch = 0; ch < 8; ch++) {
		rmt_config_t probe;
		memset(&probe, 0, sizeof(probe));
		probe.rmt_mode = mode;
		probe.channel = (rmt_channel_t)ch;
		probe.gpio_num = (mode == RMT_MODE_RX) ? (gpio_num_t)irRmtRxPin
												: (gpio_num_t)irTxPin;
		probe.clk_div = 80;
		probe.mem_block_num = 1;

		if (mode == RMT_MODE_RX) {
			probe.rx_config.idle_threshold = 12000;
			probe.rx_config.filter_ticks_thresh = 100;
			probe.rx_config.filter_en = true;
		} else {
			probe.tx_config.carrier_freq_hz = 38000;
			probe.tx_config.carrier_duty_percent = 50;
			probe.tx_config.carrier_en = false;
			probe.tx_config.loop_en = false;
			probe.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
			probe.tx_config.idle_output_en = true;
		}

		esp_err_t err = rmt_config(&probe);
		if (err == ESP_OK) {
			// Try to install driver too (this is what really allocates resources)
			int ringbuf = (mode == RMT_MODE_RX) ? 1000 : 0;
			err = rmt_driver_install(probe.channel, ringbuf, 0);
			if (err == ESP_OK) {
				Serial.printf("IR: found free ch %d for %s\n",
					ch, mode == RMT_MODE_RX ? "RX" : "TX");
				return probe.channel;
			}
		}
	}
	Serial.printf("IR: no free RMT channel for %s!\n",
		mode == RMT_MODE_RX ? "RX" : "TX");
	return (rmt_channel_t)-1;
}

// ── Receiver task ──

void recvIR(void* param) {

	rmt_channel_t rx_ch = rmt_find_free_channel(RMT_MODE_RX);
	if ((int)rx_ch < 0) {
		vTaskDelete(NULL);
		return;
	}

	// Reconfigure properly with full parameters
	rmt_driver_uninstall(rx_ch); // free the probe allocation

	rmt_config_t rx_conf;
	memset(&rx_conf, 0, sizeof(rx_conf));
	rx_conf.rmt_mode = RMT_MODE_RX;
	rx_conf.channel = rx_ch;
	rx_conf.gpio_num = static_cast<gpio_num_t>(irRmtRxPin);
	rx_conf.clk_div = 80;
	rx_conf.mem_block_num = 2;
	rx_conf.rx_config.idle_threshold = 12000;
	rx_conf.rx_config.filter_ticks_thresh = 100;
	rx_conf.rx_config.filter_en = true;

	ESP_ERROR_CHECK(rmt_config(&rx_conf));
	ESP_ERROR_CHECK(rmt_driver_install(rx_conf.channel, 1000, 0));

	RingbufHandle_t rb = nullptr;
	ESP_ERROR_CHECK(rmt_get_ringbuf_handle(rx_conf.channel, &rb));
	ESP_ERROR_CHECK(rmt_rx_start(rx_conf.channel, true));

	rxChannel = rx_ch;
	rxTaskHandle = xTaskGetCurrentTaskHandle();

	Serial.printf("IR: RMT RX ready on pin %d ch %d\n", irRmtRxPin, (int)rx_ch);

	uint32_t lastHeartbeat = 0;
	for (;;) {
		if (irTaskStop) {
			break;
		}
		if (irTX) {
			vTaskDelay(300 / portTICK_PERIOD_MS);
			continue;
		}
		irRX = 1;

		size_t rx_size = 0;
		rmt_item32_t *items =
			(rmt_item32_t *)xRingbufferReceive(rb, &rx_size, pdMS_TO_TICKS(1000));

		if (items) {
			size_t len = rx_size / sizeof(rmt_item32_t);

			if (len > 11) {
				uint32_t rcode = 0;
				irproto rproto = UNK;

				if ((rcode = nec_check(items, len))) rproto = NEC;
				else if ((rcode = sony_check(items, len))) rproto = SONY;
				else if ((rcode = sam_check(items, len))) rproto = SAM;
				else if ((rcode = rc5_check(items, len))) rproto = RC5;

				if (rproto != UNK) {
					irReceived(rproto, rcode, len, items);
				} else {
					// Debug: show raw header + levels for unrecognized signals
					Serial.printf("IR: raw len=%d hdr=[%d,%d] lvl=[%d,%d]\n",
						(int)len, items[0].duration0, items[0].duration1,
						items[0].level0, items[0].level1);
				}
			} else if (len > 0) {
				Serial.printf("IR: short raw len=%d hdr=[%d,%d] lvl=[%d,%d]\n",
					(int)len, items[0].duration0, items[0].duration1,
					items[0].level0, items[0].level1);
			}

			vRingbufferReturnItem(rb, (void *)items);
		}

		// Heartbeat every 10 seconds
		if (millis() - lastHeartbeat > 10000) {
			Serial.printf("IR: RX alive, ch %d, pin %d\n", (int)rx_ch, irRmtRxPin);
			lastHeartbeat = millis();
		}

		irRX = 0;
	}

	rmt_rx_stop(rx_conf.channel);
	rmt_driver_uninstall(rx_conf.channel);
	vTaskDelete(NULL);
}

// Force-stop the RX task and free its RMT resources
void irStopReceiver() {
	if (rxTaskHandle == NULL) return;
	irTaskStop = true;
	// Kill the task (it may be blocked in xRingbufferReceive)
	vTaskDelete(rxTaskHandle);
	rxTaskHandle = NULL;
	// Free RMT resources from this context
	if ((int)rxChannel >= 0) {
		rmt_rx_stop(rxChannel);
		rmt_driver_uninstall(rxChannel);
		rxChannel = (rmt_channel_t)-1;
	}
	// Also free TX driver
	if ((int)txChannel >= 0) {
		rmt_driver_uninstall(txChannel);
		txChannel = (rmt_channel_t)-1;
	}
	irRX = 0;
	irTaskStop = false;
}

// ── Transmitter ──

void sendIR(irproto brand, uint32_t code, uint8_t bits,
			uint8_t burst, uint8_t repeat) {

	if (brand >= PROTO_COUNT || proto[brand].frequency == 0) return;

	irTX = 1;
	unsigned long waitStart = millis();
	while (irRX) {
		if (millis() - waitStart > 500) break;
		vTaskDelay(2 / portTICK_PERIOD_MS);
	}

	// Find and configure TX channel (once, then reuse)
	bool newChannel = ((int)txChannel < 0);
	if (newChannel) {
		txChannel = rmt_find_free_channel(RMT_MODE_TX);
		if ((int)txChannel < 0) {
			Serial.println("IR: sendIR no free TX channel");
			irTX = 0;
			return;
		}
		rmt_driver_uninstall(txChannel); // free probe allocation
	}

	rmt_config_t tx_conf;
	memset(&tx_conf, 0, sizeof(tx_conf));
	tx_conf.rmt_mode = RMT_MODE_TX;
	tx_conf.channel = txChannel;
	tx_conf.gpio_num = static_cast<gpio_num_t>(irTxPin);
	tx_conf.clk_div = 80;
	tx_conf.mem_block_num = 1;
	tx_conf.tx_config.carrier_freq_hz = proto[brand].frequency;
	tx_conf.tx_config.carrier_duty_percent = (brand == SONY) ? 40 : 50;
	tx_conf.tx_config.carrier_en = true;
	tx_conf.tx_config.loop_en = false;
	tx_conf.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
	tx_conf.tx_config.idle_output_en = true;

	esp_err_t err = rmt_config(&tx_conf);
	if (err != ESP_OK) {
		Serial.printf("IR: sendIR rmt_config(TX) failed: %s\n", esp_err_to_name(err));
		txChannel = (rmt_channel_t)-1;
		irTX = 0;
		return;
	}
	if (newChannel) {
		err = rmt_driver_install(tx_conf.channel, 0, 0);
		if (err != ESP_OK) {
			Serial.printf("IR: sendIR rmt_driver_install(TX) failed: %s\n", esp_err_to_name(err));
			txChannel = (rmt_channel_t)-1;
			irTX = 0;
			return;
		}
	}

	Serial.printf("IR: sendIR %s code=0x%08lX bits=%d on ch %d pin %d\n",
		proto[brand].name, (unsigned long)code, bits, (int)txChannel, irTxPin);

	// Build RMT items
	rmt_item32_t items[128];
	memset(items, 0, sizeof(items));
	size_t idx = 0;
	bool rc5mode = (brand == RC5);

	if (proto[brand].header_high > 0) {
		items[idx].level0 = rc5mode ? 1 : 0;
		items[idx].duration0 = proto[brand].header_high;
		items[idx].level1 = rc5mode ? 0 : 1;
		items[idx].duration1 = proto[brand].header_low;
		idx++;
	}

	for (uint8_t i = 0; i < bits; i++) {
		bool bit = (code >> (bits - 1 - i)) & 1;
		if (bit) {
			items[idx].level0 = rc5mode ? 1 : 0;
			items[idx].duration0 = proto[brand].one_high;
			items[idx].level1 = rc5mode ? 0 : 1;
			items[idx].duration1 = proto[brand].one_low;
		} else {
			items[idx].level0 = 0;
			items[idx].duration0 = proto[brand].zero_high;
			items[idx].level1 = 1;
			items[idx].duration1 = proto[brand].zero_low;
		}
		idx++;
	}

	if (proto[brand].footer_high > 0) {
		items[idx].level0 = 0;
		items[idx].duration0 = proto[brand].footer_high;
		items[idx].level1 = 1;
		items[idx].duration1 = proto[brand].footer_low;
		idx++;
	}

	uint8_t rptgap = (brand == SONY) ? 24 : 100;

	for (uint8_t j = 0; j < repeat; j++) {
		for (uint8_t i = 0; i < burst; i++) {
			err = rmt_write_items(tx_conf.channel, items, idx, true);
			if (err != ESP_OK) {
				Serial.printf("IR: rmt_write_items failed: %s\n", esp_err_to_name(err));
			}
			vTaskDelay(rptgap / portTICK_PERIOD_MS);
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}

	// Keep TX driver installed; rmt_write_items with wait=true already finished
	irTX = 0;
}

#endif // IR_USE_RMT
