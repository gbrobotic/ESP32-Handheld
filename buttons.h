#pragma once

// ======================================================
// BUTTON PINS
// ======================================================

#define BTN_LEFT   D0
#define BTN_OK     D4
#define BTN_RIGHT  D5
#define BTN_HOME   D6


// ======================================================
// BUTTON EVENTS
// ======================================================

enum ButtonEvent
{
    BTN_NONE,

    BTN_LEFT_PRESSED,
    BTN_OK_PRESSED,
    BTN_RIGHT_PRESSED,
    BTN_HOME_PRESSED
};


// ======================================================
// FUNCTIONS
// ======================================================

void buttonsInit();

ButtonEvent buttonsUpdate();

void waitForRelease(int pin);