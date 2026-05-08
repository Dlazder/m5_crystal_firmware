#include <time.h>

struct DeviceTime {
	int hours;
	int minutes;
	int seconds;
};

DeviceTime getDeviceTime() {
	DeviceTime t;
	#ifdef CARDPUTER
		struct timeval tv;
		gettimeofday(&tv, nullptr);
		struct tm timeinfo;
		localtime_r(&tv.tv_sec, &timeinfo);
		t.hours   = timeinfo.tm_hour;
		t.minutes = timeinfo.tm_min;
		t.seconds = timeinfo.tm_sec;
	#else
		auto dt = DEVICE.Rtc.getDateTime();
		t.hours   = dt.time.hours;
		t.minutes = dt.time.minutes;
		t.seconds = dt.time.seconds;
	#endif
	return t;
}

void setDeviceTime(int hours, int minutes, int seconds) {
	#ifdef CARDPUTER
		struct timeval tv;
		struct tm timeinfo = {};
		timeinfo.tm_hour = hours;
		timeinfo.tm_min  = minutes;
		timeinfo.tm_sec  = seconds;
		timeinfo.tm_year = 70;
		timeinfo.tm_mday = 1;
		tv.tv_sec = mktime(&timeinfo);
		tv.tv_usec = 0;
		settimeofday(&tv, nullptr);
	#else
		m5::rtc_datetime_t dt = DEVICE.Rtc.getDateTime();
		dt.time.hours   = hours;
		dt.time.minutes = minutes;
		dt.time.seconds = seconds;
		DEVICE.Rtc.setDateTime(&dt);
	#endif
}
