#pragma once

// Selects the device implementation based on the build flag set in platformio.ini.
// To add a new device:
//   1. Create src/devices/mydevice.h (copy cardputer.h as a template)
//   2. Add -DDEVICE_MYDEVICE to build_flags in platformio.ini

#if defined(CARDPUTER)
    #include "cardputer.h"
#else
    #include "m5stickc_plus2.h"
#endif

