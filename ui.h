#pragma once

#include <Arduino.h>
#include "buttons.h"

enum Screen
{
    SCREEN_HOME,
    SCREEN_MENU,
    SCREEN_TODO,
    SCREEN_FOCUS,
    SCREEN_HABITS,
    SCREEN_PROGRESS,
    SCREEN_CLOCK,
    SCREEN_ALARM,
    SCREEN_SYSTEM,
    SCREEN_ALARM_RINGING
};

void uiInit();
void uiHandleInput(ButtonEvent event);
void drawCurrentScreen();

void drawHome();
void drawMenu();
void drawTodo();
void drawFocus();
void drawHabits();
void drawProgress();
void drawClock();
void drawAlarm();
void drawSystem();

void openSelectedFeature();
void handleTodoInput(ButtonEvent event);
void uiUpdate();
void handleFocusInput(ButtonEvent event);
void drawFeatureHeader(const char* title);
void drawBottomHint(const char* text);

void drawTask(const char* name, int y, bool completed);
void drawHabit(const char* name, const char* streak, int y, bool done);
void drawProgressCard(const char* title, const char* value, int y, uint16_t color, int progress);
void drawAlarmRow(const char* alarmTime, const char* name, int y, bool enabled);
void drawSystemRow(const char* left, const char* right, int y, uint16_t rightColor);