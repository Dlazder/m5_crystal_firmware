# How to add a new feature?

Add a menu item wherever you want.
You must specify the name and process id that will be launched. Add the process to the `src/system/processes.h`.

*functions/mainMenuLoop.h*

```cpp
MENU mainMenu[] = {
  // other items ...
  {PID::HELLO_WORLD, "Hello world"},
};
```

<img src="../images/example-1.png" width="500" alt="Menu demonstration"/>

After that, create a function for the process, let's create a new file for this in the *functions* directory

*functions/helloWorldLoop.h*

```cpp
void helloWorldLoop() {
  if (isSetup()) {
    centeredPrint("Hello world!", MEDIUM_TEXT);
    soundSuccess();
  }

  checkExit();
}
```

In this code, we used functions, one of the “utilities”, necessary to make the code easier and cleaner. They are all located in `src/utils/`.
Utilities described in the [utilities documentation](./utils.md).

Add an include of this function next to the others.

*system/functions.h*

```cpp
// other includes...
#include "../functions/helloWorldLoop.h"
```

## ✨ Result

<img src="../images/example-2.png" width="500" alt="Program demonstration"/>
