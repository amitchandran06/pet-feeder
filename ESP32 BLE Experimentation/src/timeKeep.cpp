#include <time.h>
#include <timeKeep.h>

void syncTime(String timestamp) {
   int h = 0; 
    int m = 0; 
    int s = 0;
    // Extracting just the time part from "2026-02-24 19:08:14"
    // We skip the date using %*s (which means "read but ignore")
    if (sscanf(timestamp.c_str(), "%*s %d:%d:%d", &h, &m, &s) == 3) {
        struct tm tm;
        tm.tm_hour = h;
        tm.tm_min = m;
        tm.tm_sec = s;
        tm.tm_year = 126; // 2026 - 1900
        tm.tm_mon = 1;   // Feb
        tm.tm_mday = 24;

        time_t t = mktime(&tm);
        struct timeval now = { .tv_sec = t, .tv_usec = 0 };
        settimeofday(&now, NULL);
        
        Serial.printf("Clock synced to %02d:%02d:%02d\n", h, m, s);
    }
}

char globalTimeStr[6] = "00:00"; 

void updateGlobalTime() {
    struct tm now;
    if (getLocalTime(&now)) {
        // Format the RTC time into our global char array
        strftime(globalTimeStr, sizeof(globalTimeStr), "%H:%M", &now);
    }
}