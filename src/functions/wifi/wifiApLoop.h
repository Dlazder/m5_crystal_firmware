// pid 4

IPAddress AP_GATEWAY(172, 0, 0, 1);

void webServerSetup() {
  dnsServer.start(1, "*", AP_GATEWAY);
  webServer.on("/", []() {
    webServer.send(200, "text/html", mainHTML());
  });
  webServer.on("/style.css", []() {
    webServer.send(200, "text/css", mainCSS());
  });
  webServer.on("/index.js", []() {
    webServer.send(200, "text/javascript", mainJS());
  });
  webServer.on("/post", []() {
    String data = webServer.arg("data");
    if (data == "down") {
      webControlDown();
      webDataRequested = true;
      webServer.send(200, "text/plane", "ok");
    } else if (data == "up") {
      webControlUp();
      webDataRequested = true;
      webServer.send(200, "text/plane", "ok");
    } else if (data == "a") {
      pressBtnA();
      webDataRequested = true;
      webServer.send(200, "text/plane", "ok");
    } else if (data == "b") {
      pressBtnB();
      if (webDataType != "function") webDataRequested = true;
      webServer.send(200, "text/plane", "ok");
    } else if (data == "update")  {
      webDataRequested = true;
      webServer.send(200, "text/plane", "ok");
    } else if (data == "get") {
      webServer.send(200, "text/plane", webData);
    }
  });
  webServer.begin();
}


void wifiApLoop() {
  if (isSetup()) {
    if (!isWebInterfaceEnabled) {
      WiFi.mode(WIFI_AP);
      WiFi.softAP(wifiSsid, apWifiPassword);
      WiFi.softAPConfig(AP_GATEWAY, AP_GATEWAY, IPAddress(255, 255, 255, 0));
      webServerSetup();
      isWebInterfaceEnabled = true;
    }
    DISP.setTextSize(SMALL_TEXT);
    centeredPrint(L->TXT_WIFI_AP_ENABLED, SMALL_TEXT);
  }
  checkExit();
  if (isWebDataRequested()) {
    String res = generateFunctionElement(L->TXT_WIFI_AP_ENABLED, SMALL_TEXT, String("center"));
    webData = generateWebData("function", res);
  }
}