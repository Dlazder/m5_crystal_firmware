// dead code

String generateMenuString(MENU menu[], int size) {
  String string = "";
  for (int i = 0; i < size; i++) {
    String name_str = menu[i].name;
    String command_str = String(menu[i].command);
    string += name_str + "," + command_str + ";";
  }
  return string;
}

String generateFunctionElement(String text, int textSize, String align) {
  String elementString = text + "," + String(textSize) + "," + align + ";";
  return elementString;
}

String generateWebData(String type, String otherData) {
  String res = "";
  if (type == "menu") {
    res = type + ";" + cursor + ";";
  } else if (type == "function") {
    res = type + ";";
  }
  res += otherData;
  return res;
}

bool isWebDataRequested() {
  if (webDataRequested) {
    webDataRequested = false;
    return true;
  } else return false;
}