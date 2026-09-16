// PID::WIFI_WPS_PBC

#include "esp_wifi.h"
#include "esp_wps.h"

// --- State ------------------------------------------------------------------
enum WpsPbcState {
	WPBC_INIT,
	WPBC_STARTING,
	WPBC_RUNNING,
	WPBC_DONE,
	WPBC_FAILED,
	WPBC_UNSUPPORTED
};
static WpsPbcState wpbcState = WPBC_INIT;
static uint32_t wpbcTimer = 0;
static int wpbcCycles = 0;
static bool wpbcGotCreds = false;
static esp_err_t wpbcLastErr = ESP_OK;
static char wpbcSsid[33] = {0};
static char wpbcPass[65] = {0};

// --- Target info -------------------------------------------------------------
static uint8_t  wpbcTargetBssid[6];
static int      wpbcTargetChannel;
static String   wpbcTargetSsid;

// --- File --------------------------------------------------------------------
static String wpbcFilePath;
static bool wpbcUseLittleFS = false;
static bool wpbcFileWritten = false;

// --- WPS event handler -------------------------------------------------------
static esp_event_handler_instance_t wpbcHandlerInst;
static void wpbcEventHandler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data) {
	if (event_base != WIFI_EVENT) return;
	switch (event_id) {
	case WIFI_EVENT_STA_WPS_ER_SUCCESS:
		Serial.println("WPS_PBC: success");
		wpbcState = WPBC_DONE;
		{
			wifi_config_t conf;
			if (esp_wifi_get_config(WIFI_IF_STA, &conf) == ESP_OK) {
				strncpy(wpbcSsid, (const char*)conf.sta.ssid, 32);
				wpbcSsid[32] = 0;
				strncpy(wpbcPass, (const char*)conf.sta.password, 63);
				wpbcPass[63] = 0;
				Serial.printf("WPS_PBC: SSID=%s PSK=%s\n", wpbcSsid, wpbcPass);
				if (wpbcSsid[0] != 0) wpbcGotCreds = true;
			}
		}
		soundSuccess();
		break;
	case WIFI_EVENT_STA_WPS_ER_FAILED:
		Serial.println("WPS_PBC: failed");
		wpbcState = WPBC_FAILED;
		soundError();
		break;
	case WIFI_EVENT_STA_WPS_ER_TIMEOUT:
		Serial.println("WPS_PBC: timeout");
		wpbcState = WPBC_FAILED;
		break;
	case WIFI_EVENT_STA_WPS_ER_PIN:
		if (event_data) {
			Serial.printf("WPS_PBC: PIN=%s\n",
				((wifi_event_sta_wps_er_pin_t*)event_data)->pin_code);
		}
		break;
	}
}

// --- Save credentials to file ------------------------------------------------
static void wpbcSaveFile() {
	if (wpbcFileWritten) return;
	if (Storage::mountSD()) {
		wpbcUseLittleFS = false;
	} else {
		wpbcUseLittleFS = Storage::mountLittleFS();
		if (!wpbcUseLittleFS) return;
	}

	String safe = wpbcTargetSsid;
	safe.replace(" ", "_");
	safe.replace("/", "_"); safe.replace("\\", "_"); safe.replace(":", "_");
	safe.replace("*", "_"); safe.replace("?", "_"); safe.replace("\"", "_");
	safe.replace("<", "_"); safe.replace(">", "_"); safe.replace("|", "_");

	wpbcFilePath = Storage::uniquePath("/wps_pbc_" + safe, ".txt", wpbcUseLittleFS);
	File f = Storage::open(wpbcFilePath.c_str(), "w", wpbcUseLittleFS);
	if (!f) { Serial.println("WPS_PBC: file open failed"); return; }

	f.print("# WPS PBC result\n");
	f.print("SSID="); f.print(wpbcSsid); f.print("\n");
	f.print("PSK=");  f.print(wpbcPass); f.print("\n");
	f.flush(); f.close();
	wpbcFileWritten = true;
	Serial.printf("WPS_PBC: saved to %s\n", wpbcFilePath.c_str());
}

// --- Main loop ---------------------------------------------------------------
void wifiWpsPbcLoop() {
	static bool wpsStarted = false;

	if (isSetup()) {
		wpbcState = WPBC_INIT;
		wpbcTimer = millis();
		wpbcCycles = 0;
		wpbcGotCreds = false;
		wpbcFileWritten = false;
		memset(wpbcSsid, 0, sizeof(wpbcSsid));
		memset(wpbcPass, 0, sizeof(wpbcPass));
		wpsStarted = false;

		memcpy(wpbcTargetBssid, bssid, 6);
		wpbcTargetChannel = channel;
		wpbcTargetSsid = ssid;

		esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
			&wpbcEventHandler, NULL, &wpbcHandlerInst);
		WiFi.mode(WIFI_STA);

		centeredPrint(wpbcTargetSsid, SMALL_TEXT, true);
		drawSpinner();
		canvas.pushSprite(0, getStatusBarHeight());
	}

	uint32_t now = millis();

	switch (wpbcState) {
	case WPBC_INIT:
		if (now - wpbcTimer >= 1500) {
			wpbcState = WPBC_STARTING;
			wpbcTimer = now;
		}
		break;

	case WPBC_STARTING: {
		esp_wifi_wps_disable();
		WiFi.mode(WIFI_STA);
		delay(100);
		esp_wps_config_t cfg;
		memset(&cfg, 0, sizeof(cfg));
		cfg.wps_type = WPS_TYPE_PBC;
		strcpy(cfg.factory_info.manufacturer, "ESPRESSIF");
		strcpy(cfg.factory_info.model_number, "ESP32");
		strcpy(cfg.factory_info.model_name, "ESPRESSIF IOT");
		strcpy(cfg.factory_info.device_name, "ESP DEVICE");
		esp_err_t err = esp_wifi_wps_enable(&cfg);
		if (err != ESP_OK) {
			Serial.printf("WPS_PBC: unavailable (0x%x)\n", err);
			wpbcLastErr = err;
			wpbcState = WPBC_UNSUPPORTED;
			wpbcTimer = now;
		} else {
			esp_wifi_wps_start(0);
			wpbcState = WPBC_RUNNING;
			wpbcTimer = now;
			wpbcCycles++;
			wpsStarted = true;
			Serial.printf("WPS_PBC: started ch=%d\n", wpbcTargetChannel);
		}
		break;
	}

	case WPBC_RUNNING:
		if (now - wpbcTimer >= 30000) {
			Serial.println("WPS_PBC: timeout, retrying...");
			wpbcState = WPBC_FAILED;
		}
		break;

	case WPBC_DONE:
		// Stay here until user exits — don't auto-retry
		break;

	case WPBC_FAILED:
		if (now - wpbcTimer >= 3000) {
			wpbcState = WPBC_INIT;
			wpbcTimer = now;
			wpsStarted = false;
		}
		break;

	case WPBC_UNSUPPORTED:
		if (now - wpbcTimer >= 4000) {
			wpbcState = WPBC_INIT;
			wpbcTimer = now;
		}
		break;
	}

	// Manual retry
	if ((isKbEnterPressed() || isBtnAWasPressed()) && wpsStarted) {
		delay(50);
		wpbcState = WPBC_INIT;
		wpbcTimer = now;
		wpsStarted = false;
		soundBeep();
	}

	// Display
	{
		canvas.clear();
		canvas.setTextColor(FGCOLOR, BGCOLOR);
		canvas.setTextSize(SMALL_TEXT);

		const char* statusStr;
		switch (wpbcState) {
		case WPBC_INIT:         statusStr = "Init..."; break;
		case WPBC_STARTING:     statusStr = "Starting WPS..."; break;
		case WPBC_RUNNING:      statusStr = "Press WPS button on router"; break;
		case WPBC_DONE:         statusStr = wpbcGotCreds ? "WPS success! Got PSK!" : "Done"; break;
		case WPBC_FAILED:       statusStr = "Timeout / failed"; break;
		case WPBC_UNSUPPORTED:  statusStr = "WPS unavailable"; break;
		default:                statusStr = "..."; break;
		}

		String info[] = {
			wpbcTargetSsid,
			statusStr,
			"Attempts: " + String(wpbcCycles)
		};
		centeredPrintRows(info, 3, SMALL_TEXT, true, 35);

		drawSpinner();
		drawHintCustom("enter: retry", "A: retry");
	}

	// Exit
	if (checkExit()) {
		esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wpbcHandlerInst);

		if (!wpbcFileWritten && wpbcGotCreds) {
			wpbcSaveFile();
		}

		if (wpbcGotCreds) {
			String label = wpbcUseLittleFS ? L->TXT_SAVED_LFS : L->TXT_SAVED_SD;
			String lines[] = {
				label,
				wpbcFilePath,
				"SSID: " + String(wpbcSsid),
				"PSK: " + String(wpbcPass),
			};
			centeredPrintRows(lines, 4, SMALL_TEXT);
		}
		soundSuccess();
		delay(3000);
	}
}
