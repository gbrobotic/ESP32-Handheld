#include "buttons.h"

#include <Arduino.h>


// ======================================================
// INITIALIZE BUTTONS
// ======================================================

void buttonsInit()
{
    pinMode(BTN_LEFT, INPUT_PULLUP);
    pinMode(BTN_OK, INPUT_PULLUP);
    pinMode(BTN_RIGHT, INPUT_PULLUP);
    pinMode(BTN_HOME, INPUT_PULLUP);
}


// ======================================================
// READ BUTTONS
// ======================================================

ButtonEvent buttonsUpdate()
{
    if (digitalRead(BTN_LEFT) == LOW)
    {
        waitForRelease(BTN_LEFT);
        return BTN_LEFT_PRESSED;
    }


    if (digitalRead(BTN_RIGHT) == LOW)
    {
        waitForRelease(BTN_RIGHT);
        return BTN_RIGHT_PRESSED;
    }


    if (digitalRead(BTN_OK) == LOW)
    {
        waitForRelease(BTN_OK);
        return BTN_OK_PRESSED;
    }


    if (digitalRead(BTN_HOME) == LOW)
    {
        waitForRelease(BTN_HOME);
        return BTN_HOME_PRESSED;
    }


    return BTN_NONE;
}


// ======================================================
// WAIT FOR BUTTON RELEASE
// ======================================================

void waitForRelease(int pin)
{
    delay(50);

    while (digitalRead(pin) == LOW)
    {
        delay(10);
    }

    delay(50);
}