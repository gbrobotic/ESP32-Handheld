#include "clock.h"
#include <Arduino.h>
#include <time.h>

#define IST_OFFSET_SECONDS 19800
#define NTP_SERVER "pool.ntp.org"

static char dayString[12] = "---";
static char dateString[20] = "-- --- ----";
static char timeString[10] = "--:--";

void clockInit()
{
    // Start NTP synchronization
    // DO NOT wait here.
    configTime(
        IST_OFFSET_SECONDS,
        0,
        NTP_SERVER
    );

    Serial.println("NTP synchronization started...");
}

void clockUpdate()
{
    struct tm timeinfo;

    // Check whether valid time is available
    if (!getLocalTime(&timeinfo, 0))
    {
        // Time not synchronized yet
        // Keep the placeholder values.
        return;
    }

    // Update day
    strftime(
        dayString,
        sizeof(dayString),
        "%A",
        &timeinfo
    );

    // Update date
    strftime(
        dateString,
        sizeof(dateString),
        "%d %b %Y",
        &timeinfo
    );

    // Update time
    strftime(
        timeString,
        sizeof(timeString),
        "%H:%M",
        &timeinfo
    );
}

const char* getDayName()
{
    return dayString;
}

const char* getDateString()
{
    return dateString;
}

const char* getTimeString()
{
    return timeString;
}