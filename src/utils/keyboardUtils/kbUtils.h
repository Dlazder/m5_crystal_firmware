// Shared keyboard state — used by both physical and virtual keyboard implementations.

#define KB_GYRO_MAX 0.8f
#define KB_GYRO_HYSTERESIS 0.08f

const int kbRowCount = 4;
const int kbColCounts[] = { 12, 12, 12, 12 };

const char kbNormal[4][12] = {
	{ '1','2','3','4','5','6','7','8','9','0','-','=' },
	{ 'q','w','e','r','t','y','u','i','o','p','[',']' },
	{ 'a','s','d','f','g','h','j','k','l',';','\'', '`'},
	{ 'z','x','c','v','b','n','m',',','.','?','/','\\'},
};

const char kbShifted[4][12] = {
	{ '!','@','#','$','%','^','&','*','(',')','_','+' },
	{ 'Q','W','E','R','T','Y','U','I','O','P','{','}' },
	{ 'A','S','D','F','G','H','J','K','L',':','"', '~'},
	{ 'Z','X','C','V','B','N','M','<','>','~','/', '|'},
};

const int kbSpecialRow = 0;
const int kbSpecialCount = 5;
const char* kbSpecialLabels[] = { "exit", "CAP", "ENT", "DEL", "SPC" };

// Keyboard state
char kbBuf[64];
int  kbLen = 0;
int  kbCursorPos = 0;
int  kbRow = 0;
int  kbCol = 0;
bool kbCapsOn = false;
int  kbBlinkCounter = 0;
bool kbCursorVisible = true;

void kbReset() {
	kbBuf[0] = '\0';
	kbLen = 0;
	kbCursorPos = 0;
	kbRow = 0;
	kbCol = 0;
	kbCapsOn = false;
	kbBlinkCounter = 0;
	kbCursorVisible = true;
	#if HAS_PHYSICAL_KB
	kbTextMode = true;
	#endif
}

void kbEnd() {
	#if HAS_PHYSICAL_KB
	kbTextMode = false;
	#endif
}

// drawKeyboardUi() and keyboardLoop() are implemented per-device:
#if HAS_PHYSICAL_KB
	#include "kbUtils_physical.h"
#else
	#include "kbUtils_virtual.h"
#endif
