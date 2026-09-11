# Adding and editing localization

> [!NOTE]
> Если некоторые буквы отображаются не корректно, поменяйте стандартный шрифт в настройках.

## Editing localization

Localization files are located in `src/system/locale`.

Open the file with the desired language, you will see a lot of key-value pairs in front of you.
Example:

```cpp
.MENU_BACK   = "back",
.MENU_SCAN   = "scan",
.MENU_RESCAN = "rescan",
```

Without touching the keys, change the necessary values.
Also note that you cannot swap lines; the order of the keys in the localization file must be identical to the locale.h file

## Adding new language

If you want to add a new language, create a file in `src/system/locale` with the same name and your language code, copy the contents of locale_en.cpp into it, change the name of the structure to match the file name, and then add it to the languages ​​array in globals.h here:

```cpp
const Locale* locales[] = { &LANG_EN, &LANG_ES, &LANG_IT, &LANG_ID };
const char* localeNames[] = { "English", "Espanol", "Italiano", "Indonesia" };
```

After this, the new language will appear in the firmware settings, you can start replacing the values ​​with the translation of your language.
