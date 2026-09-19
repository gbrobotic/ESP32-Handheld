#include "alarm.h"
#include <Arduino.h>
#include <time.h>
#include "data_storage.h"
#define BUZZER_PIN D7

struct Alarm
{
    int hour;
    int minute;
    bool enabled;
    bool triggered;
};

static Alarm alarms[3] =
{
    {7, 0, false, false},
    {9, 30, false, false},
    {18, 0, false, false}
};

static bool alarmRinging = false;
static int ringingAlarm = -1;

void alarmInit()
{
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    for (int i = 0; i < 3; i++)
    {
        loadAlarmData(
            i,
            alarms[i].hour,
            alarms[i].minute,
            alarms[i].enabled
        );

        alarms[i].triggered = false;
    }

    alarmRinging = false;
    ringingAlarm = -1;
}

void alarmUpdate()
{
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);

    if (timeinfo == nullptr)
        return;

    for (int i = 0; i < 3; i++)
    {
        if (!alarms[i].enabled)
            continue;

        if (alarms[i].triggered)
            continue;

        if (timeinfo->tm_hour == alarms[i].hour &&
            timeinfo->tm_min == alarms[i].minute)
        {
            alarms[i].triggered = true;

            alarmRinging = true;
            ringingAlarm = i;

            Serial.print("ALARM ");
            Serial.print(i + 1);
            Serial.println(" TRIGGERED!");
        }
    }

    // Active buzzer
    if (alarmRinging)
    {
        digitalWrite(BUZZER_PIN, HIGH);
    }
    else
    {
        digitalWrite(BUZZER_PIN, LOW);
    }
    
}

bool isAlarmRinging()
{
    return alarmRinging;
}

void stopAlarm()
{
    alarmRinging = false;

    digitalWrite(BUZZER_PIN, LOW);

    if (ringingAlarm >= 0 && ringingAlarm < 3)
    {
        alarms[ringingAlarm].triggered = true;
    }

    ringingAlarm = -1;
}

bool isAlarmEnabled(int index)
{
    if (index < 0 || index >= 3)
        return false;

    return alarms[index].enabled;
}

bool isAlarmTriggered(int index)
{
    if (index < 0 || index >= 3)
        return false;

    return alarms[index].triggered;
}

int getAlarmHour(int index)
{
    if (index < 0 || index >= 3)
        return 0;

    return alarms[index].hour;
}

int getAlarmMinute(int index)
{
    if (index < 0 || index >= 3)
        return 0;

    return alarms[index].minute;
}

void setAlarmEnabled(int index, bool enabled)
{
    if (index < 0 || index >= 3)
        return;

    alarms[index].enabled = enabled;

    if (!enabled)
    {
        alarms[index].triggered = false;
    }

    saveAlarmData(
        index,
        alarms[index].hour,
        alarms[index].minute,
        alarms[index].enabled
    );
}

void setAlarmHour(int index, int hour)
{
    if (index < 0 || index >= 3)
        return;

    if (hour < 0)
        hour = 23;

    if (hour > 23)
        hour = 0;

    alarms[index].hour = hour;
alarms[index].triggered = false;

saveAlarmData(
    index,
    alarms[index].hour,
    alarms[index].minute,
    alarms[index].enabled
);
}

void setAlarmMinute(int index, int minute)
{
    if (index < 0 || index >= 3)
        return;

    if (minute < 0)
        minute = 59;

    if (minute > 59)
        minute = 0;

    alarms[index].minute = minute;
    alarms[index].triggered = false;

    saveAlarmData(
        index,
        alarms[index].hour,
        alarms[index].minute,
        alarms[index].enabled
    );
}