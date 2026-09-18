#pragma once

#include <Arduino.h>
#include "buttons.h"

// ======================================================
// SCREEN STATES
// ======================================================

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
    SCREEN_SYSTEM
};


// ======================================================
// UI INITIALIZATION
// ======================================================

void uiInit();

void uiHandleInput(ButtonEvent event);

void drawCurrentScreen();


// ======================================================
// MAIN SCREENS
// ======================================================

void drawHome();

void drawMenu();

void drawTodo();

void drawFocus();

void drawHabits();

void drawProgress();

void drawClock();

void drawAlarm();

void drawSystem();


// ======================================================
// NAVIGATION
// ======================================================

void openSelectedFeature();


// ======================================================
// UI HELPER FUNCTIONS
// ======================================================

void drawFeatureHeader(
    const char* title
);

void drawBottomHint(
    const char* text
);


// ======================================================
// TODO
// ======================================================

void drawTask(
    const char* name,
    int y,
    bool completed
);


// ======================================================
// HABITS
// ======================================================

void drawHabit(
    const char* name,
    const char* streak,
    int y,
    bool done
);


// ======================================================
// PROGRESS
// ======================================================

void drawProgressCard(
    const char* title,
    const char* value,
    int y,
    uint16_t color,
    int progress
);


// ======================================================
// ALARM
// ======================================================

void drawAlarmRow(
    const char* alarmTime,
    const char* name,
    int y,
    bool enabled
);


// ======================================================
// SYSTEM
// ======================================================

void drawSystemRow(
    const char* left,
    const char* right,
    int y,
    uint16_t rightColor
);