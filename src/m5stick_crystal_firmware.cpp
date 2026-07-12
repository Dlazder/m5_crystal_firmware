#include "./system/globals.h"
#include "./system/utils.h"
#include "./system/functions.h"
#include "./system/switcher.h"
#include "./system/loadPreferences.h"
#include "./system/serialCommandHandler.h"

void setup() {
  deviceInit();
  hasImu = (DEVICE.Imu.getType() != m5::imu_none);
  Serial.begin(115200);
  Serial.printf("[DEBUG] Detected board: %d (14=M5Cardputer, 24=M5CardputerADV)\n", (int)DEVICE.getBoard());
  Serial.printf("[DEBUG] SDA=%d  SCL=%d\n", SDA, SCL);
  preferences.begin("storage", false);
  loadPreferences();

  canvas.createSprite(DISP.width(), DISP.height() - getStatusBarHeight());
  canvas.setTextColor(FGCOLOR, BGCOLOR);
  canvas.setTextSize(MEDIUM_TEXT);
  canvas.setTextWrap(false);

  statusBarCanvas.createSprite(DISP.width(), 20);
  statusBarCanvas.setTextColor(FGCOLOR, BGCOLOR);

  DEVICE.Speaker.begin();
  drawStartupScreen();
  Wire.setClock(10000);
  Wire.setTimeout(100);
  delay(1000);
  // When starting the firmware, the state of the pressed btnPWR is stuck, which is why the menu scrolls. We update the state of the buttons to avoid this.
  DEVICE.update();

  soundStartup();

  // Warm up speaker DMA buffers before WiFi takes memory.
  DEVICE.Speaker.begin();
  DEVICE.Speaker.end();

  // Initialize WiFi driver once at boot so its memory is allocated before BLE.
  // This prevents ESP_ERR_NO_MEM when switching between WiFi and BLE at runtime.
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(false);

  DISP.clear();
}

void loop() {
  globalTimer = millis();
  deviceUpdate();

  handleSerialCommands();

  dimmerUpdate();

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