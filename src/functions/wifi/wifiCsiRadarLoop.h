// PID::WIFI_CSI_RADAR

// CSI radar: plots every scanned AP on a circular radar by bearing and level.

#include "esp_wifi.h"

// Per-device antenna pattern offset (see devices/*.h): the heading of max
// reception points this many degrees away from the AP. 0 = max straight ahead.
#ifndef ANTENNA_BORESIGHT_DEG
#define ANTENNA_BORESIGHT_DEG 0.0f
#endif

#define CSR_MAX_APS 24
#define CSR_BINS 24 // 15-degree heading bins over 360
#define CSR_RING_SIZE 128
#define CSR_DWELL_MS 110 // per channel, just over one beacon interval (102.4 ms)
#define CSR_MIN_GAP_US 25000 // per-AP rate limit, so busy APs don't flood the ring
#define CSR_BIAS_MS 1200 // gyro bias averaging while the device is still
#define CSR_STILL_DPS 9.0f // gyro magnitude above this restarts the bias phase
#define CSR_CAL_ANGLE 360.0f
#define CSR_FULL_BINS 18 // occupied bins (of 24) for the profile-peak bearing
// Max turn rate: every AP's channel should be visited at least once per two
// heading bins (the profile smoothing spans +-2 bins), so the limit depends on
// how many channels are hopped: 30 deg / (channels * dwell), clamped.
#define CSR_MAX_RATE_MIN 45.0f // deg/s
#define CSR_MAX_RATE_MAX 120.0f // deg/s
#define CSR_FORGET 0.99f // per-sample decay of an AP's bins in radar mode
#define CSR_BIN_SAT 1.0f // bin weight w counts as w / (w + SAT) in the fit
#define CSR_MIN_CONF 2.5f // front/back difference in standard errors
#define CSR_STALE_MS 6000 // AP drawn dimmed when not heard for this long
#define CSR_LEVEL_NEAR -35.0f // dBm mapped to the radar center
#define CSR_LEVEL_FAR -95.0f // dBm mapped to the radar edge
#define CSR_PROMISC_WEIGHT 128 // sample weight (of 255) for non-CSI frames

struct CsrAp {
	uint8_t bssid[6];
	char ssid[18];
	uint8_t channel;
	float binW[CSR_BINS]; // summed sample weight per heading bin
	float binY[CSR_BINS]; // summed weight * level per heading bin
	float level; // fitted mean level, dBm
	float bearing; // heading of the strongest reception, degrees (CCW+)
	float depth; // front/back difference (peak to mean), dB
	float conf; // depth / standard error of depth
	bool fitted;
	uint32_t lastSeen;
	uint16_t samples;
};

struct CsrSample {
	uint8_t ap;
	int8_t rssi;
	uint8_t weight; // 0..255
};

enum { CSR_BIAS, CSR_CALIBRATE, CSR_RADAR };

static CsrAp _csrAps[CSR_MAX_APS];
static int _csrApCount = 0;
static uint8_t _csrChannels[14];
static int _csrChannelCount = 0;
static int _csrChannelIdx = 0;
static uint32_t _csrHopAt = 0;

// Ring buffer RX callbacks (WiFi task) -> main loop
static CsrSample _csrRing[CSR_RING_SIZE];
static volatile int _csrHead = 0;
static volatile int _csrTail = 0;
static volatile uint32_t _csrLastRxUs[CSR_MAX_APS];

static bool _csrActive = false;
static int _csrPhase = CSR_BIAS;
static uint32_t _csrPhaseStart = 0;
static float _csrBias[3];
static int _csrBiasN = 0;
static float _csrHeading = 0; // integrated yaw, degrees, CCW positive
static float _csrRate = 0; // smoothed |yaw rate|, deg/s
static uint32_t _csrLastImuUs = 0;
static int _csrSelected = 0;
static bool _csrSingle = false; // tracking only Wifi::selected

static int _csrFindAp(const uint8_t* mac) {
	for (int i = 0; i < _csrApCount; i++) {
		if (memcmp(mac, _csrAps[i].bssid, 6) == 0) return i;
	}
	return -1;
}

// Both RX callbacks feed the same ring; both run in the WiFi task, so there
// is a single producer.
static void _csrPush(int ap, int8_t rssi, uint8_t weight, uint32_t rxUs) {
	if (rxUs - _csrLastRxUs[ap] < CSR_MIN_GAP_US) return;
	int next = (_csrHead + 1) % CSR_RING_SIZE;
	if (next == _csrTail) return; // full, drop
	_csrLastRxUs[ap] = rxUs;
	_csrRing[_csrHead].ap = ap;
	_csrRing[_csrHead].rssi = rssi;
	_csrRing[_csrHead].weight = weight;
	_csrHead = next;
}

// CSI callback — OFDM frames only. Runs in WiFi task context.
static void _csrCsiCb(void* ctx, wifi_csi_info_t* info) {
	if (!info || !info->buf || info->len < 8) return;
	int ap = _csrFindAp(info->mac);
	if (ap < 0) return;

	// Subcarrier amplitude flatness: mean^2 / E[amp^2], 1.0 = flat channel.
	// Buffer is (imag, real) int8 pairs; null/guard subcarriers are zero.
	float sum = 0, sum2 = 0;
	int n = 0;
	for (int i = info->first_word_invalid ? 4 : 0; i + 1 < info->len; i += 2) {
		int im = info->buf[i];
		int re = info->buf[i + 1];
		if (im == 0 && re == 0) continue;
		float a = sqrtf((float)(im * im + re * re));
		sum += a;
		sum2 += a * a;
		n++;
	}
	if (n < 8 || sum2 <= 0) return;
	float mean = sum / n;
	float weight = (mean * mean) / (sum2 / n);

	_csrPush(ap, info->rx_ctrl.rssi, (uint8_t)constrain(weight * 255.0f, 1.0f, 255.0f), info->rx_ctrl.timestamp);
}

// Promiscuous callback — any frame transmitted by a tracked AP (addr2 = TA).
static void _csrPromiscCb(void* buf, wifi_promiscuous_pkt_type_t type) {
	if (type != WIFI_PKT_MGMT && type != WIFI_PKT_DATA) return;
	const wifi_promiscuous_pkt_t* pkt = (const wifi_promiscuous_pkt_t*)buf;
	if (pkt->rx_ctrl.sig_len < 24) return;
	const uint8_t* frame = pkt->payload;

	if (type == WIFI_PKT_DATA) {
		bool fromDS = (frame[1] >> 1) & 0x1;
		bool toDS = frame[1] & 0x1;
		if (!fromDS || toDS) return; // only AP -> client
	}

	int ap = _csrFindAp(frame + 10);
	if (ap < 0) return;
	_csrPush(ap, pkt->rx_ctrl.rssi, CSR_PROMISC_WEIGHT, pkt->rx_ctrl.timestamp);
}

// Copy one AP into the tracked list (BSSID pointers from the scan die on the
// next scan) and add its channel to the hop set.
static void _csrAddAp(const uint8_t* bssid, String name, const String& mac, int ch, int rssi) {
	if (_csrApCount >= CSR_MAX_APS || ch < 1 || ch > 14 || !bssid) return;

	CsrAp& ap = _csrAps[_csrApCount++];
	memset(&ap, 0, sizeof(ap));
	memcpy(ap.bssid, bssid, 6);
	if (name.length() == 0) name = mac; // hidden network
	strlcpy(ap.ssid, name.c_str(), sizeof(ap.ssid));
	ap.channel = ch;
	ap.level = rssi;

	for (int c = 0; c < _csrChannelCount; c++) {
		if (_csrChannels[c] == ch) return;
	}
	_csrChannels[_csrChannelCount++] = ch;
}

// All APs from the last scan.
static void _csrLoadAps() {
	_csrApCount = 0;
	_csrChannelCount = 0;
	for (int i = 0; i < wifiCount && _csrApCount < CSR_MAX_APS; i++) {
		_csrAddAp(WiFi.BSSID(i), WiFi.SSID(i), WiFi.BSSIDstr(i), WiFi.channel(i), WiFi.RSSI(i));
	}
}

// Only the network picked in the selected-wifi menu (single channel, no hopping).
static void _csrLoadSelected() {
	_csrApCount = 0;
	_csrChannelCount = 0;
	const Wifi::SelectedNetwork& sel = Wifi::selected;
	_csrAddAp(sel.bssid, sel.ssid, sel.mac, sel.channel, sel.rssi);
}

static void _csrStartCalibration() {
	for (int i = 0; i < _csrApCount; i++) {
		CsrAp& ap = _csrAps[i];
		memset(ap.binW, 0, sizeof(ap.binW));
		memset(ap.binY, 0, sizeof(ap.binY));
		ap.fitted = false;
		ap.depth = 0;
		ap.conf = 0;
		ap.samples = 0;
		ap.lastSeen = 0;
	}
	_csrTail = _csrHead; // drop samples tagged with the old heading frame
	_csrPhase = CSR_BIAS;
	_csrPhaseStart = millis();
	_csrBias[0] = _csrBias[1] = _csrBias[2] = 0;
	_csrBiasN = 0;
	_csrHeading = 0;
	_csrRate = 0;
	_csrLastImuUs = micros();
}

static void _csrStart() {
	WiFi.mode(WIFI_STA);
	delay(50);
	memset((void*)_csrLastRxUs, 0, sizeof(_csrLastRxUs));
	_csrChannelIdx = 0;
	esp_wifi_set_channel(_csrChannels[0], WIFI_SECOND_CHAN_NONE);
	_csrHopAt = millis() + CSR_DWELL_MS;
	esp_wifi_set_promiscuous_rx_cb(&_csrPromiscCb);
	esp_wifi_set_promiscuous(true);

	wifi_csi_config_t cfg = {};
	cfg.lltf_en = true;
	cfg.htltf_en = false;
	cfg.stbc_htltf2_en = false;
	cfg.ltf_merge_en = true;
	cfg.channel_filter_en = false;
	cfg.manu_scale = false;
	cfg.shift = 0;
	esp_wifi_set_csi_config(&cfg);
	esp_wifi_set_csi_rx_cb(&_csrCsiCb, nullptr);
	esp_wifi_set_csi(true);
	_csrActive = true;
}

static void _csrStop() {
	if (!_csrActive) return;
	esp_wifi_set_csi(false);
	esp_wifi_set_csi_rx_cb(nullptr, nullptr);
	esp_wifi_set_promiscuous(false);
	esp_wifi_set_promiscuous_rx_cb(nullptr);
	_csrActive = false;
}

// Integrate yaw: gyro projected onto the gravity vector, so the result does
// not depend on how the device is held (upright, flat, landscape...).
static void _csrUpdateHeading() {
	uint32_t now = micros();
	float dt = (now - _csrLastImuUs) * 1e-6f;
	_csrLastImuUs = now;
	if (dt > 0.2f) dt = 0; // long stall (screen switch etc.) — skip, don't jump

	float ax, ay, az, gx, gy, gz;
	DEVICE.Imu.getAccelData(&ax, &ay, &az);
	DEVICE.Imu.getGyroData(&gx, &gy, &gz);

	if (_csrPhase == CSR_BIAS) {
		if (sqrtf(gx * gx + gy * gy + gz * gz) > CSR_STILL_DPS) {
			// moved — restart averaging
			_csrBias[0] = _csrBias[1] = _csrBias[2] = 0;
			_csrBiasN = 0;
			_csrPhaseStart = millis();
			return;
		}
		_csrBias[0] += gx;
		_csrBias[1] += gy;
		_csrBias[2] += gz;
		_csrBiasN++;
		if (millis() - _csrPhaseStart >= CSR_BIAS_MS && _csrBiasN > 10) {
			for (int i = 0; i < 3; i++) _csrBias[i] /= _csrBiasN;
			_csrPhase = CSR_CALIBRATE;
			_csrPhaseStart = millis();
			soundBeep();
		}
		return;
	}

	gx -= _csrBias[0];
	gy -= _csrBias[1];
	gz -= _csrBias[2];
	float g = sqrtf(ax * ax + ay * ay + az * az);
	if (g < 0.3f) return; // free fall / bad read
	float yawRate = (gx * ax + gy * ay + gz * az) / g;
	_csrHeading += yawRate * dt;
	// Slow smoothing (~0.5 s): short hand jerks shouldn't trigger the warning.
	float k = constrain(dt * 2.0f, 0.0f, 1.0f);
	_csrRate += k * (fabsf(yawRate) - _csrRate);
}

static float _csrWrap180(float deg) {
	while (deg > 180.0f) deg -= 360.0f;
	while (deg <= -180.0f) deg += 360.0f;
	return deg;
}

static int _csrBinOf(float headingDeg) {
	float h = fmodf(headingDeg, 360.0f);
	if (h < 0) h += 360.0f;
	int bin = (int)(h * CSR_BINS / 360.0f);
	return constrain(bin, 0, CSR_BINS - 1);
}

static void _csrAddSample(CsrAp& ap, float y, float w, float headingDeg, bool forget) {
	if (forget) {
		for (int i = 0; i < CSR_BINS; i++) {
			ap.binW[i] *= CSR_FORGET;
			ap.binY[i] *= CSR_FORGET;
		}
	}
	int bin = _csrBinOf(headingDeg);
	ap.binW[bin] += w;
	ap.binY[bin] += w * y;
}

// Weighted least squares over the heading bins (each bin contributes its mean
// level; its weight saturates so a bin the user lingered on can't dominate).
// When the occupied bins are too clustered for a stable fit, only the level is
// updated and the previous bearing is kept.
static void _csrSolve(CsrAp& ap) {
	float m00 = 0, m01 = 0, m02 = 0, m11 = 0, m12 = 0, m22 = 0;
	float v0 = 0, v1 = 0, v2 = 0;
	float binMean[CSR_BINS], binU[CSR_BINS], binC[CSR_BINS], binS[CSR_BINS];
	int occupied = 0;

	for (int i = 0; i < CSR_BINS; i++) {
		binU[i] = 0;
		if (ap.binW[i] < 0.05f) continue;
		float h = (i + 0.5f) * (TWO_PI / CSR_BINS);
		float c = cosf(h), s = sinf(h);
		float y = ap.binY[i] / ap.binW[i];
		float u = ap.binW[i] / (ap.binW[i] + CSR_BIN_SAT);
		binMean[i] = y; binU[i] = u; binC[i] = c; binS[i] = s;
		occupied++;

		m00 += u; m01 += u * c; m02 += u * s;
		m11 += u * c * c; m12 += u * c * s; m22 += u * s * s;
		v0 += u * y; v1 += u * y * c; v2 += u * y * s;
	}
	if (m00 <= 0) return;

	float det = m00 * (m11 * m22 - m12 * m12)
		- m01 * (m01 * m22 - m12 * m02)
		+ m02 * (m01 * m12 - m11 * m02);

	// det / m00^3 is the determinant of the normalized moment matrix:
	// 0.25 for a full circle, ~0.047 for 180 degrees, 0 for a single heading.
	if (occupied < 5 || det <= 0.01f * m00 * m00 * m00) {
		ap.level = v0 / m00;
		return;
	}

	float a = (v0 * (m11 * m22 - m12 * m12)
		- m01 * (v1 * m22 - m12 * v2)
		+ m02 * (v1 * m12 - m11 * v2)) / det;
	float b = (m00 * (v1 * m22 - v2 * m12)
		- v0 * (m01 * m22 - m12 * m02)
		+ m02 * (m01 * v2 - v1 * m02)) / det;
	float c = (m00 * (m11 * v2 - m12 * v1)
		- m01 * (m01 * v2 - v1 * m02)
		+ v0 * (m01 * m12 - m11 * m02)) / det;

	// Residual scatter of the bin means around the fit -> standard error of
	// the front/back amplitude (~ sigma * sqrt(2 / bins)).
	float rss = 0, uSum = 0;
	for (int i = 0; i < CSR_BINS; i++) {
		if (binU[i] <= 0) continue;
		float e = binMean[i] - (a + b * binC[i] + c * binS[i]);
		rss += binU[i] * e * e;
		uSum += binU[i];
	}
	float sigma = sqrtf(rss / uSum * occupied / (occupied - 3));
	float depth = sqrtf(b * b + c * c);
	float se = max(0.3f, sigma) * sqrtf(2.0f / occupied); // floor: RSSI is 1 dB quantized

	ap.level = a;
	ap.depth = depth;
	ap.conf = depth / se;
	ap.bearing = _csrWrap180(atan2f(c, b) * RAD_TO_DEG + ANTENNA_BORESIGHT_DEG);
	ap.fitted = true;

	if (occupied < CSR_FULL_BINS) return;

	// Whole circle measured: take the strongest direction of the profile,
	// smoothed with a triangular kernel over +-2 bins (+-30 degrees) to ride
	// over single noisy bins, then refined with a parabola through the peak.
	float prof[CSR_BINS];
	bool valid[CSR_BINS];
	int best = -1;
	for (int i = 0; i < CSR_BINS; i++) {
		float num = 0, den = 0;
		for (int k = -2; k <= 2; k++) {
			int j = (i + k + CSR_BINS) % CSR_BINS;
			if (binU[j] <= 0) continue;
			float kw = (3 - abs(k)) * binU[j];
			num += kw * binMean[j];
			den += kw;
		}
		valid[i] = den > 0;
		prof[i] = valid[i] ? num / den : 0;
		if (valid[i] && (best < 0 || prof[i] > prof[best])) best = i;
	}
	if (best < 0) return;

	float offset = 0;
	int lo = (best + CSR_BINS - 1) % CSR_BINS;
	int hi = (best + 1) % CSR_BINS;
	if (valid[lo] && valid[hi]) {
		float curv = prof[lo] - 2 * prof[best] + prof[hi];
		if (curv < 0) offset = constrain(0.5f * (prof[lo] - prof[hi]) / curv, -0.5f, 0.5f);
	}
	ap.bearing = _csrWrap180((best + 0.5f + offset) * (360.0f / CSR_BINS) + ANTENNA_BORESIGHT_DEG);
}

static void _csrDrainSamples() {
	uint32_t now = millis();
	while (_csrTail != _csrHead) {
		CsrSample smp = _csrRing[_csrTail];
		_csrTail = (_csrTail + 1) % CSR_RING_SIZE;
		if (smp.ap >= _csrApCount) continue;

		CsrAp& ap = _csrAps[smp.ap];
		_csrAddSample(ap, smp.rssi, smp.weight / 255.0f, _csrHeading, _csrPhase == CSR_RADAR);
		if (ap.samples < 0xFFFF) ap.samples++;
		ap.lastSeen = now;
		if (_csrPhase == CSR_RADAR) _csrSolve(ap);
	}
}

static void _csrHopChannel() {
	if (_csrChannelCount < 2 || millis() < _csrHopAt) return;
	_csrChannelIdx = (_csrChannelIdx + 1) % _csrChannelCount;
	esp_wifi_set_channel(_csrChannels[_csrChannelIdx], WIFI_SECOND_CHAN_NONE);
	_csrHopAt = millis() + CSR_DWELL_MS;
}

static bool _csrConfident(const CsrAp& ap) {
	return ap.fitted && ap.conf >= CSR_MIN_CONF;
}

// Radar scope: rings, crosshair, "forward" marker at the top.
static void _csrDrawScope(int cx, int cy, int r) {
	canvas.drawCircle(cx, cy, r / 3, TFT_DARKGREY);
	canvas.drawCircle(cx, cy, r * 2 / 3, TFT_DARKGREY);
	canvas.drawFastHLine(cx - r, cy, r * 2, TFT_DARKGREY);
	canvas.drawFastVLine(cx, cy - r, r * 2, TFT_DARKGREY);
	canvas.drawCircle(cx, cy, r, FGCOLOR);
	canvas.fillTriangle(cx, cy - r - 1, cx - 3, cy - r - 6, cx + 3, cy - r - 6, FGCOLOR);
}

static void _csrDrawCalibration(int cx, int cy, int r, int panelX) {
	_csrDrawScope(cx, cy, r);

	int seen = 0;
	for (int i = 0; i < _csrApCount; i++) {
		if (_csrAps[i].samples > 0) seen++;
	}

	canvas.setTextColor(FGCOLOR, BGCOLOR);
	canvas.setTextSize(SMALL_TEXT);
	int y = 6;
	int lineH = canvas.fontHeight() + 4;

	if (_csrPhase == CSR_BIAS) {
		canvas.drawString(L->TXT_CSI_RADAR_HOLD_STILL, panelX, y);
		y += lineH;
		uint32_t elapsed = millis() - _csrPhaseStart;
		int pct = min(100, (int)(elapsed * 100 / CSR_BIAS_MS));
		canvas.drawRect(panelX, y, canvas.width() - panelX - 4, 8, TFT_DARKGREY);
		canvas.fillRect(panelX + 1, y + 1, (canvas.width() - panelX - 6) * pct / 100, 6, FGCOLOR);
		y += 8 + 6;
	} else {
		// Swept angle as a filled arc starting at "forward" (12 o'clock).
		float swept = min(CSR_CAL_ANGLE, fabsf(_csrHeading));
		if (swept >= 1.0f) {
			// fillArc angles: 0 = 3 o'clock, clockwise. Turning CCW (heading +)
			// sweeps the arc to the left of the forward marker.
			if (_csrHeading > 0) {
				canvas.fillArc(cx, cy, r - 2, r * 2 / 3 + 2, 270 - swept, 270, FGCOLOR);
			} else {
				canvas.fillArc(cx, cy, r - 2, r * 2 / 3 + 2, 270, 270 + swept, FGCOLOR);
			}
		}

		canvas.drawString(L->TXT_CSI_RADAR_TURN, panelX, y);
		y += lineH;
		canvas.setTextSize(MEDIUM_TEXT);
		canvas.drawString(String((int)swept) + "/" + String((int)CSR_CAL_ANGLE), panelX, y);
		y += canvas.fontHeight() + 4;
		canvas.setTextSize(SMALL_TEXT);
		float maxRate = 2.0f * (360.0f / CSR_BINS) * 1000.0f / (max(1, _csrChannelCount) * CSR_DWELL_MS);
		maxRate = constrain(maxRate, CSR_MAX_RATE_MIN, CSR_MAX_RATE_MAX);
		if (_csrRate > maxRate) {
			canvas.setTextColor(TFT_RED, BGCOLOR);
			canvas.drawString(L->TXT_CSI_RADAR_SLOWER, panelX, y);
			canvas.setTextColor(FGCOLOR, BGCOLOR);
		}
		y += lineH;
	}

	canvas.setTextSize(TINY_TEXT);
	canvas.setTextColor(TFT_DARKGREY, BGCOLOR);
	canvas.drawString("AP " + String(seen) + "/" + String(_csrApCount) + "  ch " + String(_csrChannels[_csrChannelIdx]), panelX, y);
	canvas.setTextColor(FGCOLOR, BGCOLOR);
}

static void _csrDrawRadar(int cx, int cy, int r, int panelX) {
	_csrDrawScope(cx, cy, r);

	// Cosmetic sweep beam, one turn per 3 s
	float beam = (millis() % 3000) * (TWO_PI / 3000.0f);
	canvas.drawLine(cx, cy, cx + (int)(sinf(beam) * (r - 1)), cy - (int)(cosf(beam) * (r - 1)), FGCOLOR);

	uint32_t now = millis();
	int sureCount = 0;
	for (int i = 0; i < _csrApCount; i++) {
		CsrAp& ap = _csrAps[i];
		if (!ap.fitted) continue;
		bool sure = _csrConfident(ap);
		if (sure) sureCount++;

		// Bearing relative to where the device points now; CCW (left) positive.
		float rel = (ap.bearing - _csrHeading) * DEG_TO_RAD;
		float lvl = constrain(ap.level, CSR_LEVEL_FAR, CSR_LEVEL_NEAR);
		float dist = (CSR_LEVEL_NEAR - lvl) / (CSR_LEVEL_NEAR - CSR_LEVEL_FAR); // 0 near .. 1 far
		float pr = (0.15f + 0.85f * dist) * (r - 3);
		int x = cx - (int)(sinf(rel) * pr);
		int y = cy - (int)(cosf(rel) * pr);

		bool stale = (now - ap.lastSeen) > CSR_STALE_MS;
		uint16_t color = (stale || !sure) ? TFT_DARKGREY : FGCOLOR;		int dotR = (i == _csrSelected) ? 3 : 2;
		if (sure) canvas.fillCircle(x, y, dotR, color);
		else canvas.drawCircle(x, y, dotR, color); // hollow: bearing uncertain
		if (i == _csrSelected) canvas.drawCircle(x, y, 6, FGCOLOR);
	}

	// Info panel for the selected AP
	CsrAp& sel = _csrAps[_csrSelected];
	int panelW = canvas.width() - panelX - 2;
	int y = 4;

	canvas.setTextColor(FGCOLOR, BGCOLOR);
	canvas.setTextSize(SMALL_TEXT);
	String name = sel.ssid;
	while (name.length() > 1 && canvas.textWidth(name.c_str()) > panelW) {
		name.remove(name.length() - 1);
	}
	canvas.drawString(name, panelX, y);
	y += canvas.fontHeight() + 4;

	canvas.setTextSize(TINY_TEXT);
	int lineH = canvas.fontHeight() + 3;
	canvas.drawString("ch " + String(sel.channel) + "  " + String((int)roundf(sel.level)) + " dBm", panelX, y);
	y += lineH;

	if (sel.fitted) {
		int rel = (int)roundf(_csrWrap180(sel.bearing - _csrHeading));
		String dir;
		if (abs(rel) <= 10) dir = "^ " + String(abs(rel));
		else if (rel > 0) dir = "< " + String(rel);
		else dir = String(-rel) + " >";
		if (!_csrConfident(sel)) dir += "?";
		canvas.setTextSize(MEDIUM_TEXT);
		canvas.setTextColor(_csrConfident(sel) ? FGCOLOR : TFT_DARKGREY, BGCOLOR);
		canvas.drawString(dir, panelX, y);
		y += canvas.fontHeight() + 3;
		canvas.setTextSize(TINY_TEXT);
		canvas.setTextColor(TFT_DARKGREY, BGCOLOR);
		canvas.drawString("diff " + String(sel.depth * 2.0f, 1) + "dB q" + String(sel.conf, 1), panelX, y);
	} else {
		canvas.setTextColor(TFT_DARKGREY, BGCOLOR);
		canvas.drawString("no fit (" + String(sel.samples) + " smp)", panelX, y);
	}
	y += lineH;

	canvas.setTextColor(TFT_DARKGREY, BGCOLOR);
	canvas.drawString(String(_csrSelected + 1) + "/" + String(_csrApCount) + "  ok " + String(sureCount), panelX, y);
	canvas.setTextColor(FGCOLOR, BGCOLOR);
}

static void _csrLogFits() {
	Serial.println("CSI radar fit:");
	for (int i = 0; i < _csrApCount; i++) {
		const CsrAp& ap = _csrAps[i];
		Serial.printf("  %-17s ch%2d smp=%3u lvl=%6.1f bearing=%6.1f diff=%4.1fdB q=%4.1f%s\n",
			ap.ssid, ap.channel, ap.samples, ap.level, ap.bearing, ap.depth * 2.0f, ap.conf,
			ap.fitted ? "" : " (no fit)");
	}
}

// Shared body of both radar processes. `single`: track only Wifi::selected.
static void _csrRun(bool single) {
	if (!hasImu) {
		if (isSetup()) centeredPrint("no gyroscope", MEDIUM_TEXT);
		checkExit();
		return;
	}

	static bool _csrNeedsSetup = false;
	if (isSetup()) _csrNeedsSetup = true;
	if (!single && _csrNeedsSetup && !ensureWifiScan()) {
		checkExit();
		return;
	}

	if (_csrNeedsSetup) {
		_csrNeedsSetup = false;
		_csrSelected = 0;
		_csrSingle = single;
		_csrApCount = 0;
		if (wifiCount > 0) {
			if (single) _csrLoadSelected();
			else _csrLoadAps();
		}
		if (_csrApCount == 0) {
			centeredPrint(L->TXT_WIFI_NO_SCAN_DATA, MEDIUM_TEXT);
		} else {
			_csrStart();
			_csrStartCalibration();
		}
	}

	if (_csrApCount == 0) {
		checkExit();
		return;
	}

	if (checkExit()) {
		_csrStop();
		// Don't leave the radar frame on screen for the next process.
		canvas.fillSprite(BGCOLOR);
		canvas.pushSprite(0, getStatusBarHeight());
		return;
	}

	// Sensor + radio work every tick, drawing at ~25 fps.
	_csrUpdateHeading();
	_csrDrainSamples();
	_csrHopChannel();

	if (_csrPhase == CSR_CALIBRATE && fabsf(_csrHeading) >= CSR_CAL_ANGLE) {
		_csrPhase = CSR_RADAR;
		for (int i = 0; i < _csrApCount; i++) _csrSolve(_csrAps[i]);
		_csrLogFits();
		soundSuccess();
	}

	// Stick: A = next AP, PWR = recalibrate.
	// Cardputer: arrows switch APs (left/up = prev, right/down = next), enter = recalibrate.
	if (_csrPhase == CSR_RADAR && !_csrSingle) {
		if (isBtnAWasPressed() || isKbRightPressed() || isKbDownPressed()) {
			_csrSelected = (_csrSelected + 1) % _csrApCount;
		}
		if (isKbLeftPressed() || isKbUpPressed()) {
			_csrSelected = (_csrSelected + _csrApCount - 1) % _csrApCount;
		}
	}
	if (isBtnPWRWasPressed() || isKbEnterPressed()) {
		_csrStartCalibration();
	}

	if (!checkTimer(40)) return;

	dimmerWake(); // no button presses while turning — keep the screen on

	const int hintH = 14;
	int r = (canvas.height() - hintH) / 2 - 4;
	int cx = r + 6;
	int cy = r + 6;
	int panelX = cx + r + 10;

	canvas.fillSprite(BGCOLOR);
	if (_csrPhase == CSR_RADAR) {
		_csrDrawRadar(cx, cy, r, panelX);
		if (_csrSingle) drawHintCustom("enter: recal", "PWR: recal");
		else drawHintCustom("__LEFT__ __RIGHT__: AP  enter: recal", "A: next  PWR: recal");
	} else {
		_csrDrawCalibration(cx, cy, r, panelX);
		drawHintCustom("enter: restart", "PWR: restart");
	}
	canvas.pushSprite(0, getStatusBarHeight());
}

// PID::WIFI_CSI_RADAR — every AP from the last scan.
void wifiCsiRadarLoop() {
	_csrRun(false);
}

// PID::WIFI_CSI_RADAR_SELECTED — only the network picked in the selected-wifi menu.
void wifiCsiRadarSelectedLoop() {
	_csrRun(true);
}
