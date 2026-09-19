#pragma once

void alarmInit();
void alarmUpdate();

bool isAlarmEnabled(int index);
bool isAlarmTriggered(int index);

int getAlarmHour(int index);
int getAlarmMinute(int index);

void setAlarmEnabled(int index, bool enabled);
void setAlarmHour(int index, int hour);
void setAlarmMinute(int index, int minute);

// Buzzer / ringing control
bool isAlarmRinging();
void stopAlarm();
int getAlarmCount();