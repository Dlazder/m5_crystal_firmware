int btnAWasPressed = false;
int btnBWasPressed = false;
int btnPWRWasPressed = false;

/**
 * Checks physical button B clicks 
 */
bool isBtnBWasPressed() {
  if (btnBWasPressed) {
    btnBWasPressed = false;
    return true;
  }
  if (DEVICE.BtnB.wasPressed()) return true;
  return false;
}

/**
 * Checks physical button A clicks
 */
bool isBtnAWasPressed() {
  if (btnAWasPressed) {
    btnAWasPressed = false;
    return true;
  }
  if (DEVICE.BtnA.wasPressed()) return true;
  return false;
}

/**
 * Checks physical power button clicks
 */
bool isBtnPWRWasPressed() {
  if (btnPWRWasPressed) {
    btnPWRWasPressed = false;
    return true;
  }
  if (M5.BtnPWR.wasClicked()) return true;
  return false;
}

/**
 * Emulates a button B press
 */
void pressBtnB() {
  btnBWasPressed = true;
}

/**
 * Emulates a button A press
 */
void pressBtnA() {
  btnAWasPressed = true;
}

/**
 * Emulates a button PWR press
 */
void pressBtnPWR() {
  btnPWRWasPressed = true;
}