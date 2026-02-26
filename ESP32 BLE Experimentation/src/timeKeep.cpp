#include <time.h>
#include <timeKeep.h>


// This function sync the onboard clock to the phone time 
void syncTime(String timestamp) {
    int h = 0; 
    int m = 0; 
    int s = 0;
   // Extract just the local time in hours and minutes from the timestamp string
    if (sscanf(timestamp.c_str(), "%*s %d:%d:%d", &h, &m, &s) == 3) {
        struct tm tm;
        tm.tm_hour = h;
        tm.tm_min = m;
        tm.tm_sec = s;
        // The year date and time does not matter
        tm.tm_year = 126; 
        tm.tm_mon = 1;  
        tm.tm_mday = 24;
        //creating a struct of current times
        time_t t = mktime(&tm);
        struct timeval now = { .tv_sec = t, .tv_usec = 0 };
        settimeofday(&now, NULL);
        
        Serial.printf("Clock synced to %02d:%02d:%02d\n", h, m, s);
    }
}

char globalTimeStr[6] = "00:00"; 


// This function  pulls the current time from the board to be used elsewhere
void updateGlobalTime() {
    struct tm now;
    if (getLocalTime(&now)) {
        // Format the RTC time into our global char array
        strftime(globalTimeStr, sizeof(globalTimeStr), "%H:%M", &now);
    }
}