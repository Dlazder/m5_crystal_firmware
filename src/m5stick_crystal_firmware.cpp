#include "./system/globals.h"
#include "./system/showStartupScreen.h"
#include "./system/utils.h"
#include "./system/functions.h"
#include "./system/switcher.h"
#include "./system/loadPreferences.h"

void setup() {
  auto cfg = M5.config();
  DEVICE.begin(cfg);
  Serial.begin(115200);
  preferences.begin("storage", false);
  loadPreferences();
  showStartupScreen();

  Wire.begin(G32, G33);
  Wire.setClock(10000);
  Wire.setTimeout(100);
  delay(1000);
  // When starting the firmware, the state of the pressed btnPWR is stuck, which is why the menu scrolls. We update the state of the buttons to avoid this.
  DEVICE.update();

  canvas.createSprite(DISP.width(), DISP.height());
  canvas.setTextColor(FGCOLOR);
  canvas.setTextSize(SMALL_TEXT);

  statusBarCanvas.createSprite(DISP.width(), 20);
  statusBarCanvas.setTextColor(FGCOLOR);

  if (startupSound) {
    DEVICE.Speaker.tone(1500, 200);
    delay(200);
    DEVICE.Speaker.tone(2000, 200);
    delay(200);
    DEVICE.Speaker.tone(2500, 200);
  }

  // Warm up speaker DMA buffers before WiFi takes memory.
  DEVICE.Speaker.begin();
  DEVICE.Speaker.end();

  // Initialize WiFi driver once at boot so its memory is allocated before BLE.
  // This prevents ESP_ERR_NO_MEM when switching between WiFi and BLE at runtime.
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(false);

  cursorOnTop();
  DISP.clear();
}

void loop() {
  globalTimer = millis();

  if (statusBar) {
    statusBarLoop();
  }
  
  /* global process functions switcher */
  switcher();

  // If webDataRequested = true, this means that the process function has not yet updated the response variable.
  // Once the value is updated, the variable will become false. Until this happens, we do not process requests so as not to send outdated information to the client.
  if (isWebInterfaceEnabled && !webDataRequested) {
    webServer.handleClient();
  }
}