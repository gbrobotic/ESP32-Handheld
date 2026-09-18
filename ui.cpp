#include "ui.h"
#include "display.h"
#include "colors.h"

#include <Arduino.h>


// ======================================================
// MENU
// ======================================================

const char* menuItems[] =
{
    "Todo",
    "Focus",
    "Habits",
    "Progress",
    "Clock",
    "Alarm",
    "System"
};

const int menuCount = 7;


// ======================================================
// UI STATE
// ======================================================

Screen currentScreen = SCREEN_HOME;

int selected = 0;


// ======================================================
// INITIALIZE UI
// ======================================================

void uiInit()
{
    currentScreen = SCREEN_HOME;
    selected = 0;

    drawCurrentScreen();
}


// ======================================================
// INPUT HANDLING
// ======================================================

void uiHandleInput(ButtonEvent event)
{
    switch (event)
    {
        // ==================================================
        // LEFT
        // ==================================================

        case BTN_LEFT_PRESSED:

            if (
                currentScreen == SCREEN_HOME ||
                currentScreen == SCREEN_MENU
            )
            {
                selected--;

                if (selected < 0)
                    selected = menuCount - 1;

                drawCurrentScreen();
            }

            break;


        // ==================================================
        // RIGHT
        // ==================================================

        case BTN_RIGHT_PRESSED:

            if (
                currentScreen == SCREEN_HOME ||
                currentScreen == SCREEN_MENU
            )
            {
                selected++;

                if (selected >= menuCount)
                    selected = 0;

                drawCurrentScreen();
            }

            break;


        // ==================================================
        // OK
        // ==================================================

        case BTN_OK_PRESSED:

            if (currentScreen == SCREEN_HOME)
            {
                currentScreen = SCREEN_MENU;

                drawCurrentScreen();

                Serial.println("OPEN MENU");
            }

            else if (currentScreen == SCREEN_MENU)
            {
                openSelectedFeature();
            }

            break;


        // ==================================================
        // HOME
        // ==================================================

        case BTN_HOME_PRESSED:

            currentScreen = SCREEN_HOME;

            drawCurrentScreen();

            Serial.println("HOME");

            break;


        default:
            break;
    }
}


// ======================================================
// OPEN SELECTED FEATURE
// ======================================================

void openSelectedFeature()
{
    switch (selected)
    {
        case 0:
            currentScreen = SCREEN_TODO;
            break;

        case 1:
            currentScreen = SCREEN_FOCUS;
            break;

        case 2:
            currentScreen = SCREEN_HABITS;
            break;

        case 3:
            currentScreen = SCREEN_PROGRESS;
            break;

        case 4:
            currentScreen = SCREEN_CLOCK;
            break;

        case 5:
            currentScreen = SCREEN_ALARM;
            break;

        case 6:
            currentScreen = SCREEN_SYSTEM;
            break;
    }

    drawCurrentScreen();

    Serial.print("OPEN: ");
    Serial.println(menuItems[selected]);
}


// ======================================================
// DRAW CURRENT SCREEN
// ======================================================

void drawCurrentScreen()
{
    switch (currentScreen)
    {
        case SCREEN_HOME:
            drawHome();
            break;

        case SCREEN_MENU:
            drawMenu();
            break;

        case SCREEN_TODO:
            drawTodo();
            break;

        case SCREEN_FOCUS:
            drawFocus();
            break;

        case SCREEN_HABITS:
            drawHabits();
            break;

        case SCREEN_PROGRESS:
            drawProgress();
            break;

        case SCREEN_CLOCK:
            drawClock();
            break;

        case SCREEN_ALARM:
            drawAlarm();
            break;

        case SCREEN_SYSTEM:
            drawSystem();
            break;
    }
}


// ======================================================
// HOME SCREEN
// ======================================================

void drawHome()
{
    lcd.fillScreen(BG_COLOR);

    lcd.setTextDatum(top_left);

    // Header
    lcd.setTextColor(SUBTEXT_COLOR);
    lcd.setTextSize(1);

    lcd.drawString("10:42", 18, 12);

    lcd.drawString("Wi-Fi", 174, 12);

    lcd.fillCircle(216, 16, 3, ACCENT_COLOR);
    lcd.fillCircle(225, 16, 3, SUBTEXT_COLOR);
    lcd.fillCircle(234, 16, 3, SUBTEXT_COLOR);

    lcd.drawLine(15, 32, 225, 32, LINE_COLOR);


    // ==================================================
    // BIG TIME
    // ==================================================

    lcd.setTextDatum(middle_center);

    lcd.setTextColor(TEXT_COLOR);
    lcd.setTextSize(4);

    lcd.drawString("10:42", 120, 70);


    // ==================================================
    // DATE
    // ==================================================

    lcd.setTextColor(SUBTEXT_COLOR);
    lcd.setTextSize(1);

    lcd.drawString(
        "Thursday, Sep 18",
        120,
        103
    );


    // ==================================================
    // TASK CARD
    // ==================================================

    lcd.fillRoundRect(
        15,
        125,
        210,
        65,
        10,
        CARD_COLOR
    );

    lcd.setTextDatum(top_left);

    lcd.setTextSize(1);
    lcd.setTextColor(SUBTEXT_COLOR);

    lcd.drawString(
        "Today's Tasks",
        28,
        138
    );

    lcd.setTextColor(ACCENT_COLOR);
    lcd.setTextSize(2);

    lcd.drawString(
        "3/7",
        28,
        158
    );


    // Progress bar

    lcd.fillRoundRect(
        115,
        158,
        90,
        6,
        3,
        DARK_BAR
    );

    lcd.fillRoundRect(
        115,
        158,
        38,
        6,
        3,
        GREEN_COLOR
    );


    // ==================================================
    // NEXT TASK
    // ==================================================

    lcd.setTextColor(SUBTEXT_COLOR);
    lcd.setTextSize(1);

    lcd.drawString(
        "Next Task",
        18,
        207
    );

    lcd.setTextColor(ACCENT_COLOR);
    lcd.setTextSize(2);

    lcd.drawString(
        "Design review",
        18,
        224
    );


    // ==================================================
    // CONTROLS
    // ==================================================

    lcd.setTextDatum(middle_center);

    lcd.setTextColor(SUBTEXT_COLOR);
    lcd.setTextSize(1);

    lcd.drawString(
        "LEFT / RIGHT",
        120,
        260
    );

    lcd.drawString(
        "OK = MENU",
        120,
        278
    );

    lcd.drawLine(
        15,
        295,
        225,
        295,
        LINE_COLOR
    );

    lcd.drawString(
        "HOME",
        120,
        307
    );
}


// ======================================================
// MENU SCREEN
// ======================================================

void drawMenu()
{
    lcd.fillScreen(BG_COLOR);

    // Header

    lcd.setTextDatum(top_left);

    lcd.setTextColor(SUBTEXT_COLOR);
    lcd.setTextSize(1);

    lcd.drawString("10:42", 18, 10);

    lcd.drawString("Wi-Fi", 178, 10);

    lcd.fillCircle(218, 14, 3, ACCENT_COLOR);
    lcd.fillCircle(226, 14, 3, SUBTEXT_COLOR);
    lcd.fillCircle(234, 14, 3, SUBTEXT_COLOR);


    // Title

    lcd.setTextColor(TEXT_COLOR);
    lcd.setTextSize(3);

    lcd.drawString(
        "Menu",
        18,
        40
    );


    // Menu items

    int startY = 82;
    int rowHeight = 30;

    for (int i = 0; i < menuCount; i++)
    {
        int y = startY + i * rowHeight;


        // Selected background

        if (i == selected)
        {
            lcd.fillRoundRect(
                12,
                y - 4,
                216,
                29,
                7,
                CARD_SELECTED
            );

            lcd.fillRoundRect(
                12,
                y - 4,
                4,
                29,
                2,
                ACCENT_COLOR
            );
        }


        // Simple icons
        lcd.setTextDatum(middle_center);
        lcd.setTextSize(2);

        if (i == 0)
            lcd.drawString("O", 30, y + 9);

        else if (i == 1)
            lcd.drawString(">", 30, y + 9);

        else if (i == 2)
            lcd.drawString("*", 30, y + 9);

        else if (i == 3)
            lcd.drawString("#", 30, y + 9);

        else if (i == 4)
            lcd.drawString("@", 30, y + 9);

        else if (i == 5)
            lcd.drawString("!", 30, y + 9);

        else if (i == 6)
            lcd.drawString("+", 30, y + 9);


        // Text

        if (i == selected)
            lcd.setTextColor(ACCENT_COLOR);
        else
            lcd.setTextColor(TEXT_COLOR);

        lcd.setTextSize(2);

        lcd.drawString(
            menuItems[i],
            105,
            y + 9
        );
    }


    // Bottom controls

    lcd.setTextColor(SUBTEXT_COLOR);
    lcd.setTextSize(1);

    lcd.drawString("LEFT", 20, 298);
    lcd.drawString("RIGHT", 92, 298);
    lcd.drawString("OK", 175, 298);
}


// ======================================================
// TODO
// ======================================================

void drawTodo()
{
    drawFeatureHeader("Todo");

    lcd.setTextDatum(top_left);

    lcd.setTextSize(2);
    lcd.setTextColor(ACCENT_COLOR);

    lcd.drawString("3/7", 18, 55);

    lcd.setTextSize(1);
    lcd.setTextColor(SUBTEXT_COLOR);

    lcd.drawString(
        "Today's tasks",
        65,
        62
    );

    drawTask("Design review", 95, false);
    drawTask("Send proposal", 130, false);
    drawTask("Morning workout", 165, true);
    drawTask("Buy groceries", 200, false);
    drawTask("Team meeting", 235, false);

    drawBottomHint("OK = complete");
}


// ======================================================
// TASK
// ======================================================

void drawTask(
    const char* name,
    int y,
    bool completed
)
{
    lcd.setTextDatum(middle_left);

    lcd.setTextSize(1);

    if (completed)
    {
        lcd.setTextColor(GREEN_COLOR);
        lcd.drawString("●", 20, y);

        lcd.setTextColor(SUBTEXT_COLOR);
        lcd.drawString(name, 38, y);
    }
    else
    {
        lcd.setTextColor(SUBTEXT_COLOR);
        lcd.drawString("○", 20, y);

        lcd.setTextColor(TEXT_COLOR);
        lcd.drawString(name, 38, y);
    }
}


// ======================================================
// FOCUS
// ======================================================

void drawFocus()
{
    drawFeatureHeader("Focus");

    lcd.setTextDatum(middle_center);

    lcd.setTextColor(TEXT_COLOR);
    lcd.setTextSize(5);

    lcd.drawString(
        "25:00",
        120,
        130
    );

    lcd.setTextColor(SUBTEXT_COLOR);
    lcd.setTextSize(1);

    lcd.drawString(
        "Design review",
        120,
        175
    );

    lcd.fillRoundRect(
        35,
        205,
        170,
        8,
        4,
        DARK_BAR
    );

    lcd.fillRoundRect(
        35,
        205,
        65,
        8,
        4,
        ACCENT_COLOR
    );

    lcd.setTextColor(ACCENT_COLOR);
    lcd.setTextSize(2);

    lcd.drawString(
        "START",
        120,
        250
    );

    drawBottomHint("OK = start / pause");
}


// ======================================================
// HABITS
// ======================================================

void drawHabits()
{
    drawFeatureHeader("Habits");

    drawHabit("Exercise", "5 day streak", 65, true);
    drawHabit("Read 30 min", "12 day streak", 110, false);
    drawHabit("Meditate", "7 day streak", 155, true);
    drawHabit("Drink water", "30 day streak", 200, false);
    drawHabit("Journal", "3 day streak", 245, true);

    drawBottomHint("OK = complete");
}


// ======================================================
// HABIT
// ======================================================

void drawHabit(
    const char* name,
    const char* streak,
    int y,
    bool done
)
{
    lcd.setTextDatum(top_left);

    if (done)
        lcd.setTextColor(GREEN_COLOR);
    else
        lcd.setTextColor(SUBTEXT_COLOR);

    lcd.setTextSize(1);

    lcd.drawString(
        done ? "●" : "○",
        18,
        y
    );

    lcd.setTextColor(TEXT_COLOR);

    lcd.drawString(
        name,
        35,
        y
    );

    lcd.setTextColor(SUBTEXT_COLOR);

    lcd.drawString(
        streak,
        35,
        y + 14
    );
}


// ======================================================
// PROGRESS
// ======================================================

void drawProgress()
{
    drawFeatureHeader("Progress");

    drawProgressCard(
        "Tasks",
        "3/7",
        80,
        ACCENT_COLOR,
        43
    );

    drawProgressCard(
        "Focus",
        "1h 25m",
        145,
        ACCENT_COLOR,
        0
    );

    drawProgressCard(
        "Habits",
        "4/6",
        210,
        GREEN_COLOR,
        50
    );

    drawBottomHint("Today's progress");
}


// ======================================================
// PROGRESS CARD
// ======================================================

void drawProgressCard(
    const char* title,
    const char* value,
    int y,
    uint16_t color,
    int progress
)
{
    lcd.fillRoundRect(
        15,
        y,
        210,
        55,
        8,
        CARD_COLOR
    );

    lcd.setTextDatum(top_left);

    lcd.setTextColor(SUBTEXT_COLOR);
    lcd.setTextSize(1);

    lcd.drawString(
        title,
        28,
        y + 9
    );

    lcd.setTextColor(TEXT_COLOR);
    lcd.setTextSize(2);

    lcd.drawString(
        value,
        28,
        y + 28
    );

    if (progress > 0)
    {
        lcd.fillRoundRect(
            110,
            y + 28,
            85,
            6,
            3,
            DARK_BAR
        );

        lcd.fillRoundRect(
            110,
            y + 28,
            progress,
            6,
            3,
            color
        );
    }
}


// ======================================================
// CLOCK
// ======================================================

void drawClock()
{
    lcd.fillScreen(BG_COLOR);

    lcd.setTextDatum(middle_center);

    lcd.setTextColor(TEXT_COLOR);
    lcd.setTextSize(5);

    lcd.drawString(
        "10:42",
        120,
        125
    );

    lcd.setTextColor(SUBTEXT_COLOR);
    lcd.setTextSize(2);

    lcd.drawString(
        "Thursday",
        120,
        180
    );

    lcd.setTextSize(1);

    lcd.drawString(
        "September 18, 2026",
        120,
        207
    );

    lcd.drawString(
        "HOME = return",
        120,
        295
    );
}


// ======================================================
// ALARM
// ======================================================

void drawAlarm()
{
    drawFeatureHeader("Alarms");

    drawAlarmRow(
        "06:30",
        "Wake up",
        70,
        true
    );

    drawAlarmRow(
        "07:00",
        "Exercise",
        125,
        true
    );

    drawAlarmRow(
        "12:00",
        "Lunch break",
        180,
        false
    );

    drawAlarmRow(
        "22:00",
        "Wind down",
        235,
        true
    );

    drawBottomHint("OK = edit");
}


// ======================================================
// ALARM ROW
// ======================================================

void drawAlarmRow(
    const char* alarmTime,
    const char* name,
    int y,
    bool enabled
)
{
    lcd.setTextDatum(top_left);

    lcd.setTextColor(
        enabled ? TEXT_COLOR : SUBTEXT_COLOR
    );

    lcd.setTextSize(3);

    lcd.drawString(
        alarmTime,
        18,
        y
    );

    lcd.setTextSize(1);

    lcd.setTextColor(SUBTEXT_COLOR);

    lcd.drawString(
        name,
        20,
        y + 32
    );

    if (enabled)
        lcd.fillCircle(
            205,
            y + 15,
            6,
            ACCENT_COLOR
        );
    else
        lcd.fillCircle(
            205,
            y + 15,
            6,
            SUBTEXT_COLOR
        );
}


// ======================================================
// SYSTEM
// ======================================================

void drawSystem()
{
    drawFeatureHeader("System");

    drawSystemRow(
        "Wi-Fi",
        "Not connected",
        80,
        SUBTEXT_COLOR
    );

    drawSystemRow(
        "Server",
        "Offline",
        115,
        SUBTEXT_COLOR
    );

    drawSystemRow(
        "Battery",
        "USB power",
        150,
        TEXT_COLOR
    );

    drawSystemRow(
        "Storage",
        "Local",
        185,
        TEXT_COLOR
    );

    drawSystemRow(
        "Firmware",
        "v0.1.0",
        220,
        TEXT_COLOR
    );

    drawSystemRow(
        "Device",
        "XIAO ESP32-S3",
        255,
        TEXT_COLOR
    );

    drawBottomHint("System settings");
}


// ======================================================
// SYSTEM ROW
// ======================================================

void drawSystemRow(
    const char* left,
    const char* right,
    int y,
    uint16_t rightColor
)
{
    lcd.setTextDatum(top_left);

    lcd.setTextColor(SUBTEXT_COLOR);
    lcd.setTextSize(1);

    lcd.drawString(
        left,
        18,
        y
    );

    lcd.setTextColor(rightColor);

    lcd.drawString(
        right,
        120,
        y
    );

    lcd.drawLine(
        18,
        y + 25,
        222,
        y + 25,
        LINE_COLOR
    );
}


// ======================================================
// FEATURE HEADER
// ======================================================

void drawFeatureHeader(const char* title)
{
    lcd.fillScreen(BG_COLOR);

    lcd.setTextDatum(top_left);

    lcd.setTextColor(SUBTEXT_COLOR);
    lcd.setTextSize(1);

    lcd.drawString(
        "10:42",
        18,
        10
    );

    lcd.drawString(
        "Wi-Fi",
        178,
        10
    );

    lcd.fillCircle(
        218,
        14,
        3,
        ACCENT_COLOR
    );

    lcd.setTextColor(TEXT_COLOR);
    lcd.setTextSize(3);

    lcd.drawString(
        title,
        18,
        38
    );

    lcd.drawLine(
        15,
        72,
        225,
        72,
        LINE_COLOR
    );
}


// ======================================================
// BOTTOM HINT
// ======================================================

void drawBottomHint(const char* text)
{
    lcd.setTextDatum(middle_center);

    lcd.setTextColor(SUBTEXT_COLOR);
    lcd.setTextSize(1);

    lcd.drawString(
        text,
        120,
        295
    );
}