/**
 * Allows you to handle exit from your program.
 * If the user pressed button B it will go back to the previous process or you can pass the process id manually.
 * @param proc [optional] process id (pid)
 */
bool checkExit(int proc = previousProcess) {
  if (isBtnBWasPressed() || isKbEscPressed()) {
    clearKbFlags();
    btnAWasPressed = false;
    btnBWasPressed = false;
    process = proc;
    Serial.printf("Switching to %d process\n", process);
    DISP.clear();
    cursorOnTop();
    isSwitching = true;
    return true;
  }
  return false;
}

/**
 * Ensures that code within a condition is executed only once when the program starts.
 */
bool isSetup() {
  if (isSwitching) {
    isSwitching = false;
    return true;
  } else {
    return false;
  }
}

/**
 * @param proc process id (PID)
 * Switches to the process passed in parameters
 */
void changeProcess(int proc) {
  previousProcess = process;
  process = proc;
  isSwitching = true;
  clearKbFlags();
  btnAWasPressed = false;
  btnBWasPressed = false;
  Serial.printf("Switching to %d process\n", process);
}