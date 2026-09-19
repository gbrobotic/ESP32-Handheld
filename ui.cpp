#include "ui.h"
#include "display.h"
#include "colors.h"
#include "clock.h"
#include "alarm.h"
#include "wifi.h"
#include "data_storage.h"
#include <cstring>
#include <WiFi.h>
void updateFocusDisplay();
void handleHabitsInput(ButtonEvent event);
void handleProgressInput(ButtonEvent event);
void handleAlarmInput(ButtonEvent event);
void drawAlarmRinging();
void handleClockInput(ButtonEvent event);
void handleTodoInput(ButtonEvent event);
void drawHabitRow(
    const char* name,
    int y,
    bool completed,
    int index
);
// ============================================================
// UI STATE
// ============================================================

Screen currentScreen = SCREEN_HOME;
int selectedMenu = 0;

int selectedTodo = 0;
char todoNames[3][32];
int selectedAlarm = 0;
enum AlarmEditField
{
    ALARM_SELECT,
    ALARM_HOUR,
    ALARM_MINUTE,
    ALARM_ENABLED
};

AlarmEditField alarmEditField = ALARM_SELECT;

bool todoCompleted[3] =
{
    false,
    true,
    false
};

int selectedHabit = 0;

bool habitCompleted[3] =
{
    true,
    false,
    true
};
char habitNames[3][32];
bool focusRunning = false;
unsigned long focusStartTime = 0;
unsigned long focusPausedTime = 0;
int lastFocusSecond = -1;
const unsigned long FOCUS_DURATION = 25UL * 60UL * 1000UL;


const char* menuItems[] =
{
    "TODO",
    "FOCUS",
    "HABITS",
    "PROGRESS",
    "CLOCK",
    "ALARM",
    "SYSTEM"
};

const int MENU_COUNT = 7;

// ============================================================
// DESIGN CONSTANTS
// ============================================================

static const int MARGIN = 16;

static const int HEADER_HEIGHT = 42;
static const int FOOTER_Y = 286;

static const uint16_t BG = BG_COLOR;
static const uint16_t TEXT = TEXT_COLOR;
static const uint16_t SUBTEXT = SUBTEXT_COLOR;
static const uint16_t ACCENT = ACCENT_COLOR;
static const uint16_t CARD = CARD_COLOR;
static const uint16_t SELECTED = CARD_SELECTED;
static const uint16_t GREEN = GREEN_COLOR;
static const uint16_t WARNING = WARNING_COLOR;

// ============================================================
// UI INIT
// ============================================================

void uiInit()
{
    currentScreen = SCREEN_HOME;
    selectedMenu = 0;

    // Load saved Todo states
    // Load saved Todo data
for (int i = 0; i < 3; i++)
{
    todoCompleted[i] = loadTodoState(i);

    loadTodoName(
        i,
        todoNames[i],
        sizeof(todoNames[i])
    );
}

    // Load saved Habit states
    // Load saved Habit data
for (int i = 0; i < 3; i++)
{
    habitCompleted[i] = loadHabitState(i);

    loadHabitName(
        i,
        habitNames[i],
        sizeof(habitNames[i])
    );
}
}

// ============================================================
// INPUT
// ============================================================

void uiHandleInput(ButtonEvent event)
{
    if (currentScreen == SCREEN_HOME)
    {
        if (event == BTN_LEFT_PRESSED)
        {
            selectedMenu--;

            if (selectedMenu < 0)
                selectedMenu = MENU_COUNT - 1;

            currentScreen = SCREEN_MENU;
            drawCurrentScreen();
        }

        else if (event == BTN_RIGHT_PRESSED)
        {
            selectedMenu++;

            if (selectedMenu >= MENU_COUNT)
                selectedMenu = 0;

            currentScreen = SCREEN_MENU;
            drawCurrentScreen();
        }

        else if (event == BTN_OK_PRESSED)
        {
            currentScreen = SCREEN_MENU;
            drawCurrentScreen();
        }
    }

    else if (currentScreen == SCREEN_MENU)
    {
        if (event == BTN_LEFT_PRESSED)
        {
            selectedMenu--;

            if (selectedMenu < 0)
                selectedMenu = MENU_COUNT - 1;

            drawCurrentScreen();
        }

        else if (event == BTN_RIGHT_PRESSED)
        {
            selectedMenu++;

            if (selectedMenu >= MENU_COUNT)
                selectedMenu = 0;

            drawCurrentScreen();
        }

        else if (event == BTN_OK_PRESSED)
        {
            openSelectedFeature();
        }

        else if (event == BTN_HOME_PRESSED)
        {
            currentScreen = SCREEN_HOME;
            drawCurrentScreen();
        }
    }

    else if (currentScreen == SCREEN_TODO)
    {
        handleTodoInput(event);
    }
    else if (currentScreen == SCREEN_FOCUS)
    {
        handleFocusInput(event);
    }
else if (currentScreen == SCREEN_HABITS)
{
    handleHabitsInput(event);
}

else if (currentScreen == SCREEN_PROGRESS)
{
    handleProgressInput(event);

}

else if (currentScreen == SCREEN_CLOCK)
{
    handleClockInput(event);
}


else if (currentScreen == SCREEN_ALARM)
{
    handleAlarmInput(event);
}
else if (currentScreen == SCREEN_ALARM_RINGING)
{
    if (event == BTN_OK_PRESSED)
    {
        stopAlarm();

        currentScreen = SCREEN_HOME;
        drawCurrentScreen();
    }
}
else if (currentScreen == SCREEN_SYSTEM)
{
    if (event == BTN_HOME_PRESSED)
    {
        currentScreen = SCREEN_MENU;
        drawCurrentScreen();
    }
}

}
// ============================================================
// CURRENT SCREEN
// ============================================================

void drawCurrentScreen()
{
    lcd.fillScreen(BG);

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
        case SCREEN_ALARM_RINGING:
            drawAlarmRinging();
            break;
    }
}

// ============================================================
// HOME SCREEN


void reloadHomeData()
{
    for (int i = 0; i < 3; i++)
    {
        todoCompleted[i] = loadTodoState(i);

        loadTodoName(
            i,
            todoNames[i],
            sizeof(todoNames[i])
        );

        habitCompleted[i] = loadHabitState(i);

        loadHabitName(
            i,
            habitNames[i],
            sizeof(habitNames[i])
        );
    }
}


void drawHome()
{
     reloadHomeData();
    lcd.fillScreen(BG);

    // ========================================================
    // TOP BAR
    // ========================================================

    lcd.setFont(&fonts::Font2);
    lcd.setTextDatum(top_left);
    lcd.setTextColor(TEXT, BG);
lcd.drawString(getDayName(), 16, 14);
    

    // Status indicator
    // Later this will become Wi-Fi / battery status
    lcd.fillCircle(
        222,
        18,
        4,
        ACCENT
    );


    // ========================================================
    // DATE
    // ========================================================

    lcd.setFont(&fonts::Font2);
    lcd.setTextDatum(top_left);
    lcd.setTextColor(SUBTEXT, BG);

    lcd.drawString(getDateString(), 16, 38);


    // ========================================================
    // MAIN CLOCK
    // ========================================================

    lcd.setFont(&fonts::Font6);
    lcd.setTextDatum(middle_center);
    lcd.setTextColor(TEXT, BG);

    lcd.drawString(getTimeString(), 120, 105);


    // ========================================================
    // TODAY
    // ========================================================

    lcd.setFont(&fonts::Font4);
    lcd.setTextDatum(top_left);
    lcd.setTextColor(TEXT, BG);

    lcd.drawString(
        "TODAY",
        16,
        170
    );


    // ========================================================
    // TASKS
    // ========================================================

    lcd.setFont(&fonts::Font2);
    lcd.setTextColor(ACCENT, BG);

    int totalTasks = 0;
int completedTasks = 0;

for (int i = 0; i < 3; i++)
{
    if (strlen(todoNames[i]) > 0)
    {
        totalTasks++;

        if (todoCompleted[i])
            completedTasks++;
    }
}

if (totalTasks > 0 && completedTasks == totalTasks)
{
    // ALL created tasks completed
    lcd.fillCircle(25, 215, 7, GREEN);
}
else
{
    // At least one task remains incomplete
    lcd.drawCircle(25, 215, 7, ACCENT);
}

    lcd.setTextDatum(middle_left);
    lcd.setTextColor(TEXT, BG);

    int taskCount = 0;

for (int i = 0; i < 3; i++)
{
    if (strlen(todoNames[i]) > 0)
        taskCount++;
}

char taskText[8];
sprintf(taskText, "%d", taskCount);

lcd.drawString(
    taskText,
    48,
    215
);

    lcd.setTextColor(SUBTEXT, BG);

    lcd.drawString(
        "Tasks",
        72,
        215
    );


    // ========================================================
    // HABITS
    // ========================================================

    lcd.setTextColor(ACCENT, BG);

   int totalHabits = 0;
int completedHabits = 0;

for (int i = 0; i < 3; i++)
{
    if (strlen(habitNames[i]) > 0)
    {
        totalHabits++;

        if (habitCompleted[i])
            completedHabits++;
    }
}

if (totalHabits > 0 && completedHabits == totalHabits)
{
    // ALL created habits completed
    lcd.fillCircle(25, 250, 7, GREEN);
}
else
{
    // At least one habit remains incomplete
    lcd.drawCircle(25, 250, 7, ACCENT);
}

    lcd.setTextDatum(middle_left);
    lcd.setTextColor(TEXT, BG);

    int habitCount = 0;

for (int i = 0; i < 3; i++)
{
    if (strlen(habitNames[i]) > 0)
        habitCount++;
}

char habitText[8];
sprintf(habitText, "%d", habitCount);

lcd.drawString(
    habitText,
    48,
    250
);

    lcd.setTextColor(SUBTEXT, BG);

    lcd.drawString(
        "Habits",
        72,
        250
    );


    // ========================================================
    // MENU
    // ========================================================

    lcd.setTextDatum(middle_right);
    lcd.setTextColor(SUBTEXT, BG);
    lcd.setFont(&fonts::Font2);

    lcd.drawString(
        "MENU  >",
        224,
        304
    );
}
// ============================================================

void drawMenu()
{
    // ========================================================
    // MENU HEADER
    // ========================================================

    lcd.setFont(&fonts::Font4);
    lcd.setTextDatum(top_left);
    lcd.setTextColor(TEXT, BG);

    lcd.drawString(
        "MENU",
        MARGIN,
        12
    );


    // ========================================================
    // MENU ITEMS
    // ========================================================

    const int startY = 58;
    const int itemHeight = 31;

    for (int i = 0; i < MENU_COUNT; i++)
    {
        int y = startY + i * itemHeight;

        bool selected = (i == selectedMenu);


        // ----------------------------------------------------
        // Selection box
        // ----------------------------------------------------

        if (selected)
        {
            lcd.fillRoundRect(
                MARGIN,
                y,
                208,
                27,
                7,
                SELECTED
            );

            // Small accent indicator
            lcd.fillRoundRect(
                MARGIN + 7,
                y + 7,
                3,
                13,
                2,
                ACCENT
            );
        }


        // ----------------------------------------------------
        // Menu text
        // ----------------------------------------------------

        lcd.setFont(&fonts::Font2);
        lcd.setTextDatum(middle_left);

        lcd.setTextColor(
            selected ? TEXT : SUBTEXT,
            selected ? SELECTED : BG
        );

        lcd.drawString(
            menuItems[i],
            MARGIN + 18,
            y + 13
        );
    }


    // ========================================================
    // FOOTER
    // ========================================================

    lcd.setFont(&fonts::Font2);
    lcd.setTextDatum(middle_center);
    lcd.setTextColor(SUBTEXT, BG);

    lcd.drawString(
        " <  SELECT   >",
        120,
        304
    );
}
// ============================================================
// FEATURE HEADER
// ============================================================

void drawFeatureHeader(const char* title)
{
    lcd.setFont(&fonts::Font4);
    lcd.setTextSize(1);
    lcd.setTextDatum(top_left);
    lcd.setTextColor(TEXT, BG);

    lcd.drawString(
        title,
        MARGIN,
        12
    );
}
// ============================================================
// FOOTER
// ============================================================

void drawBottomHint(const char* text)
{
    lcd.setFont(&fonts::Font2);
    lcd.setTextSize(1);
    lcd.setTextDatum(middle_center);

    lcd.setTextColor(SUBTEXT, BG);

    lcd.drawString(
        text,
        120,
        304
    );
}

// ============================================================
// TODO
// ============================================================
void drawTodo()
{
    drawFeatureHeader("TODO");

 if (strlen(todoNames[0]) == 0)
{
    drawTask(
        "Empty",
        72,
        false
    );
}
else
{
    drawTask(
        todoNames[0],
        72,
        todoCompleted[0]
    );
}

if (strlen(todoNames[1]) == 0)
{
    drawTask(
        "Empty",
        122,
        false
    );
}
else
{
    drawTask(
        todoNames[1],
        122,
        todoCompleted[1]
    );
}

if (strlen(todoNames[2]) == 0)
{
    drawTask(
        "Empty",
        172,
        false
    );
}
else
{
    drawTask(
        todoNames[2],
        172,
        todoCompleted[2]
    );
}

    drawBottomHint("<   SELECT   OK");
}
void drawTask(
    const char* name,
    int y,
    bool completed
)
{
    bool selected = false;

    if (y == 72 && selectedTodo == 0)
        selected = true;

    if (y == 122 && selectedTodo == 1)
        selected = true;

    if (y == 172 && selectedTodo == 2)
        selected = true;


    // Selected row — same style as MENU
    if (selected)
    {
        lcd.fillRoundRect(
            MARGIN,
            y - 16,
            208,
            32,
            8,
            SELECTED
        );

        // Small cyan indicator
        lcd.fillRoundRect(
            MARGIN + 8,
            y - 8,
            4,
            16,
            2,
            ACCENT
        );
    }


    // Task completion circle
    if (completed)
    {
        lcd.fillCircle(
            MARGIN + 28,
            y,
            6,
            GREEN
        );
    }
    else
    {
        lcd.drawCircle(
            MARGIN + 28,
            y,
            6,
            ACCENT
        );
    }


    // Task name
    lcd.setFont(&fonts::Font2);
    lcd.setTextDatum(middle_left);

    lcd.setTextColor(
        completed ? SUBTEXT : TEXT,
        selected ? SELECTED : BG
    );

    lcd.drawString(
        name,
        MARGIN + 42,
        y
    );
}// FOCUS

void handleTodoInput(ButtonEvent event)
{
    if (event == BTN_LEFT_PRESSED)
    {
        selectedTodo--;

        if (selectedTodo < 0)
            selectedTodo = 2;

        drawCurrentScreen();
    }

    else if (event == BTN_RIGHT_PRESSED)
    {
        selectedTodo++;

        if (selectedTodo > 2)
            selectedTodo = 0;

        drawCurrentScreen();
    }

    else if (event == BTN_OK_PRESSED)
    {
        // Don't complete an empty slot
        if (strlen(todoNames[selectedTodo]) > 0)
        {
            todoCompleted[selectedTodo] =
                !todoCompleted[selectedTodo];

            saveTodoState(
                selectedTodo,
                todoCompleted[selectedTodo]
            );
        }

        drawCurrentScreen();
    }

    else if (event == BTN_HOME_PRESSED)
    {
        currentScreen = SCREEN_MENU;
        drawCurrentScreen();
    }
}

// ============================================================
void drawFocus()
{
    drawFeatureHeader("FOCUS");

    unsigned long elapsed = focusPausedTime;

    if (focusRunning)
    {
        elapsed = millis() - focusStartTime;
    }

    if (elapsed > FOCUS_DURATION)
        elapsed = FOCUS_DURATION;

    unsigned long remaining = FOCUS_DURATION - elapsed;

    int minutes = remaining / 60000;
    int seconds = (remaining % 60000) / 1000;

    char timerText[10];

    sprintf(
        timerText,
        "%02d:%02d",
        minutes,
        seconds
    );

    // TIMER
    lcd.setFont(&fonts::Font6);
    lcd.setTextSize(1);
    lcd.setTextDatum(middle_center);
    lcd.setTextColor(TEXT, BG);

    lcd.drawString(
        timerText,
        120,
        110
    );

    // SESSION NAME
    lcd.setFont(&fonts::Font4);
    lcd.setTextSize(1);
    lcd.setTextColor(SUBTEXT, BG);

    lcd.drawString(
        "DEEP WORK",
        120,
        155
    );

    // PROGRESS
    int progressWidth =
        (168UL * elapsed) / FOCUS_DURATION;

    lcd.fillRoundRect(
        36,
        195,
        168,
        6,
        3,
        DARK_BAR
    );

    if (progressWidth > 0)
    {
        lcd.fillRoundRect(
            36,
            195,
            progressWidth,
            6,
            3,
            ACCENT
        );
    }

    // SESSION INFO
    char sessionText[20];

    sprintf(
        sessionText,
        "%lu:%02lu / 25:00",
        elapsed / 60000,
        (elapsed % 60000) / 1000
    );

lcd.setFont(&fonts::Font4);
lcd.setTextSize(1);
lcd.setTextDatum(middle_center);
    lcd.setTextColor(SUBTEXT, BG);

    lcd.drawString(
        sessionText,
        120,
        225
    );

    // STATUS
    lcd.setFont(&fonts::Font4);
lcd.setTextSize(1);
lcd.setTextDatum(middle_center);

    lcd.setTextColor(
        focusRunning ? ACCENT : SUBTEXT,
        BG
    );

    lcd.drawString(
        focusRunning ? "RUNNING" : "PAUSED",
        120,
        250
    );

    drawBottomHint(
        "OK  START/PAUSE     HOME"
    );
}



void handleFocusInput(ButtonEvent event)
{
        if (event == BTN_OK_PRESSED)
    {
        if (!focusRunning)
        {
            focusStartTime =
                millis() - focusPausedTime;

            focusRunning = true;
            lastFocusSecond = -1;

            drawCurrentScreen();
        }
        else
        {
            focusPausedTime =
                millis() - focusStartTime;

            focusRunning = false;

            drawCurrentScreen();
        }
    }

    else if (event == BTN_HOME_PRESSED)
    {
        focusRunning = false;
        currentScreen = SCREEN_MENU;
        drawCurrentScreen();
    }
}


void drawHabits()
{
    drawFeatureHeader("HABITS");

    // Habit 1
    if (strlen(habitNames[0]) == 0)
    {
        drawHabitRow("Empty", 72, false, 0);
    }
    else
    {
        drawHabitRow(
            habitNames[0],
            72,
            habitCompleted[0],
            0
        );
    }

    // Habit 2
    if (strlen(habitNames[1]) == 0)
    {
        drawHabitRow("Empty", 122, false, 1);
    }
    else
    {
        drawHabitRow(
            habitNames[1],
            122,
            habitCompleted[1],
            1
        );
    }

    // Habit 3
    if (strlen(habitNames[2]) == 0)
    {
        drawHabitRow("Empty", 172, false, 2);
    }
    else
    {
        drawHabitRow(
            habitNames[2],
            172,
            habitCompleted[2],
            2
        );
    }

    drawBottomHint(" <   SELECT   OK");
}

void drawHabitRow(
    const char* name,
    int y,
    bool completed,
    int index
)
{
    bool selected = (index == selectedHabit);

    // Selection background
    if (selected)
    {
        lcd.fillRoundRect(
            MARGIN,
            y - 16,
            208,
            32,
            8,
            SELECTED
        );

        // Accent indicator
        lcd.fillRoundRect(
            MARGIN + 8,
            y - 8,
            4,
            16,
            2,
            ACCENT
        );
    }

    // Completion circle
    if (completed)
    {
        lcd.fillCircle(
            MARGIN + 28,
            y,
            6,
            GREEN
        );
    }
    else
    {
        lcd.drawCircle(
            MARGIN + 28,
            y,
            6,
            ACCENT
        );
    }

    // Habit name
    lcd.setFont(&fonts::Font2);
    lcd.setTextDatum(middle_left);

    lcd.setTextColor(
        completed ? SUBTEXT : TEXT,
        selected ? SELECTED : BG
    );

    lcd.drawString(
        name,
        MARGIN + 42,
        y
    );
}


// ============================================================
// HABIT
// ============================================================
void drawHabit(
    const char* name,
    const char* streak,
    int y,
    bool done
)
{
    int habitIndex = 0;

    if (y == 130)
        habitIndex = 1;
    else if (y == 180)
        habitIndex = 2;

    bool selected = (habitIndex == selectedHabit);

    // Selection background
    if (selected)
    {
        lcd.fillRoundRect(
            MARGIN,
            y - 12,
            208,
            42,
            8,
            SELECTED
        );

        lcd.fillRoundRect(
            MARGIN + 8,
            y - 5,
            4,
            16,
            2,
            ACCENT
        );
    }

    // Habit name
    lcd.setFont(&fonts::Font2);
    lcd.setTextSize(1);
    lcd.setTextDatum(top_left);

    lcd.setTextColor(
        TEXT,
        selected ? SELECTED : BG
    );

    lcd.drawString(
        name,
        MARGIN + 18,
        y - 8
    );

    // Streak
    lcd.setTextColor(
        SUBTEXT,
        selected ? SELECTED : BG
    );

    lcd.drawString(
        streak,
        MARGIN + 18,
        y + 10
    );

    // Completion indicator
    lcd.setTextDatum(middle_center);

    if (done)
    {
        lcd.fillCircle(
            210,
            y + 5,
            7,
            GREEN
        );
    }
    else
    {
        lcd.drawCircle(
            210,
            y + 5,
            7,
            SUBTEXT
        );
    }
}

void handleHabitsInput(ButtonEvent event)
{
    if (event == BTN_LEFT_PRESSED)
    {
        selectedHabit--;

        if (selectedHabit < 0)
            selectedHabit = 2;

        drawCurrentScreen();
    }

    else if (event == BTN_RIGHT_PRESSED)
    {
        selectedHabit++;

        if (selectedHabit > 2)
            selectedHabit = 0;

        drawCurrentScreen();
    }

else if (event == BTN_OK_PRESSED)
{
    habitCompleted[selectedHabit] =
        !habitCompleted[selectedHabit];

    saveHabitState(
        selectedHabit,
        habitCompleted[selectedHabit]
    );

    drawCurrentScreen();
}

    else if (event == BTN_HOME_PRESSED)
    {
        currentScreen = SCREEN_MENU;
        drawCurrentScreen();
    }
}


// ============================================================
// PROGRESS
// ============================================================
void drawProgress()
{
    drawFeatureHeader("PROGRESS");

    // -------------------------
    // TODAY - TASKS
    // -------------------------

    int totalTasks = 0;
    int completedTasks = 0;

    for (int i = 0; i < 3; i++)
    {
        if (strlen(todoNames[i]) > 0)
        {
            totalTasks++;

            if (todoCompleted[i])
                completedTasks++;
        }
    }

    int todayProgress = 0;

    if (totalTasks > 0)
        todayProgress = (completedTasks * 100) / totalTasks;

    char todayText[16];

    sprintf(
        todayText,
        "%d/%d  %d%%",
        completedTasks,
        totalTasks,
        todayProgress
    );

    drawProgressCard(
        "TODAY",
        todayText,
        65,
        ACCENT,
        todayProgress
    );


    // -------------------------
    // HABITS
    // -------------------------

    int totalHabits = 0;
    int completedHabits = 0;

    for (int i = 0; i < 3; i++)
    {
        if (strlen(habitNames[i]) > 0)
        {
            totalHabits++;

            if (habitCompleted[i])
                completedHabits++;
        }
    }

    int habitProgress = 0;

    if (totalHabits > 0)
        habitProgress =
            (completedHabits * 100) / totalHabits;

    char habitText[16];

    sprintf(
        habitText,
        "%d/%d  %d%%",
        completedHabits,
        totalHabits,
        habitProgress
    );

    drawProgressCard(
        "HABITS",
        habitText,
        205,
        WARNING,
        habitProgress
    );


    drawBottomHint("HOME  BACK");
}// ============================================================
// PROGRESS CARD
// ============================================================
void drawProgressCard(
    const char* title,
    const char* value,
    int y,
    uint16_t color,
    int progress
)
{
    // -------------------------
    // TITLE
    // -------------------------

    lcd.setFont(&fonts::Font4);
    lcd.setTextSize(1);
    lcd.setTextDatum(top_left);
    lcd.setTextColor(SUBTEXT, BG);

    lcd.drawString(
        title,
        MARGIN,
        y
    );


    // -------------------------
    // PERCENTAGE
    // -------------------------

    lcd.setFont(&fonts::Font4);
    lcd.setTextSize(1);
    lcd.setTextDatum(top_right);
    lcd.setTextColor(TEXT, BG);

    lcd.drawString(
        value,
        224,
        y - 2
    );


    // -------------------------
    // PROGRESS BAR
    // -------------------------

    lcd.fillRoundRect(
        MARGIN,
        y + 30,
        208,
        5,
        2,
        DARK_BAR
    );

    lcd.fillRoundRect(
        MARGIN,
        y + 30,
        (208 * progress) / 100,
        5,
        2,
        color
    );
}
// ============================================================
// CLOCK
// ============================================================
void drawClock()
{
    lcd.fillScreen(BG);

    drawFeatureHeader("CLOCK");

    // Large time
    lcd.setFont(&fonts::Font6);
    lcd.setTextDatum(middle_center);
    lcd.setTextColor(TEXT, BG);
    lcd.drawString(getTimeString(), 120, 120);

    // Day
    lcd.setFont(&fonts::Font4);
    lcd.setTextDatum(middle_center);
    lcd.setTextColor(ACCENT, BG);
    lcd.drawString(getDayName(), 120, 175);

    // Date
    lcd.setFont(&fonts::Font2);
    lcd.setTextDatum(middle_center);
    lcd.setTextColor(SUBTEXT, BG);
    lcd.drawString(getDateString(), 120, 205);

    drawBottomHint("HOME  BACK");
}
// ============================================================
void updateClockDisplay()
{
    static char lastTime[10] = "";

    const char* currentTime = getTimeString();

    // Only redraw when the time actually changes
    if (strcmp(lastTime, currentTime) == 0)
        return;

    strcpy(lastTime, currentTime);

    // Clear only the old time area
    lcd.fillRect(55, 75, 130, 80, BG);

    lcd.setFont(&fonts::Font6);
    lcd.setTextDatum(middle_center);
    lcd.setTextColor(TEXT, BG);
    lcd.drawString(currentTime, 120, 120);
}

//============================================================
// ALARM
// ============================================================
void drawAlarm()
{
    drawFeatureHeader("ALARM");

    for (int i = 0; i < 3; i++)
    {
        int y = 75 + (i * 55);

        char alarmText[10];

        sprintf(
            alarmText,
            "%02d:%02d",
            getAlarmHour(i),
            getAlarmMinute(i)
        );

        bool selected = (i == selectedAlarm);

        // Selection background
        if (selected)
        {
            lcd.fillRoundRect(
                MARGIN,
                y - 18,
                208,
                40,
                8,
                SELECTED
            );

            lcd.fillRoundRect(
                MARGIN + 8,
                y - 8,
                4,
                16,
                2,
                ACCENT
            );
        }

        // Alarm time
        lcd.setFont(&fonts::Font4);
        lcd.setTextSize(1);
        lcd.setTextDatum(middle_left);

        lcd.setTextColor(
            selected ? TEXT : SUBTEXT,
            selected ? SELECTED : BG
        );

        lcd.drawString(
            alarmText,
            MARGIN + 20,
            y
        );

        // ON / OFF
        lcd.setFont(&fonts::Font2);
        lcd.setTextDatum(middle_right);

        lcd.setTextColor(
            isAlarmEnabled(i) ? ACCENT : SUBTEXT,
            selected ? SELECTED : BG
        );

        lcd.drawString(
            isAlarmEnabled(i) ? "ON" : "OFF",
            210,
            y
        );
    }

    drawBottomHint(" <  SELECT   OK");
}

void drawAlarmRinging()
{
    lcd.fillScreen(BG);

    lcd.setFont(&fonts::Font4);
    lcd.setTextDatum(middle_center);
    lcd.setTextColor(TEXT, BG);
    lcd.drawString("ALARM", 120, 65);

    lcd.setFont(&fonts::Font6);
    lcd.setTextColor(ACCENT, BG);

    // Find which alarm is ringing
    int ringingIndex = -1;

    for (int i = 0; i < 3; i++)
    {
        if (isAlarmTriggered(i))
        {
            ringingIndex = i;
            break;
        }
    }

    char alarmTime[10];

    if (ringingIndex >= 0)
    {
        sprintf(
            alarmTime,
            "%02d:%02d",
            getAlarmHour(ringingIndex),
            getAlarmMinute(ringingIndex)
        );
    }
    else
    {
        sprintf(alarmTime, "--:--");
    }

    lcd.drawString(alarmTime, 120, 130);

    lcd.setFont(&fonts::Font4);
    lcd.setTextColor(WARNING, BG);
    lcd.drawString("RINGING", 120, 190);

    lcd.setFont(&fonts::Font2);
    lcd.setTextColor(SUBTEXT, BG);
    lcd.drawString("OK  STOP", 120, 270);
}


// ============================================================
// ALARM ROW
// ============================================================


// ============================================================
// SYSTEM
// ============================================================
// ============================================================
// SYSTEM
// ============================================================
void drawSystem()
{
    drawFeatureHeader("SYSTEM");

    // Wi-Fi status
    lcd.setFont(&fonts::Font2);
    lcd.setTextDatum(middle_left);
    lcd.setTextColor(TEXT, BG);

    lcd.drawString(
        "Wi-Fi",
        MARGIN,
        75
    );

    // Wi-Fi status indicator
    uint16_t wifiColor = wifiConnected() ? GREEN : WARNING;

    lcd.fillCircle(208, 75, 3, wifiColor);
    lcd.drawCircle(208, 75, 7, wifiColor);
    lcd.drawCircle(208, 75, 11, wifiColor);

    // Battery
    drawSystemRow(
        "Battery",
        "USB",
        120,
        GREEN
    );

    // Storage
    drawSystemRow(
        "Storage",
        "OK",
        165,
        GREEN
    );

    // Server IP
    char ipText[20];

    if (wifiConnected())
    {
        String ip = WiFi.localIP().toString();
        ip.toCharArray(ipText, sizeof(ipText));

        drawSystemRow(
            "Server",
            ipText,
            210,
            ACCENT
        );
    }
    else
    {
        drawSystemRow(
            "Server",
            "OFFLINE",
            210,
            WARNING
        );
    }

    // Device
    drawSystemRow(
        "Device",
        "ESP32-S3",
        245,
        SUBTEXT
    );

    drawBottomHint("HOME  BACK");
}// ============================================================
// SYSTEM ROW
// ============================================================

void drawSystemRow(
    const char* left,
    const char* right,
    int y,
    uint16_t rightColor
)
{
    lcd.setTextDatum(middle_left);

    lcd.setTextColor(
        TEXT,
        BG
    );

    lcd.setTextSize(1);

    lcd.drawString(
        left,
        MARGIN,
        y
    );

    lcd.setTextDatum(middle_right);

    lcd.setTextColor(
        rightColor,
        BG
    );

    lcd.drawString(
        right,
        224,
        y
    );
}

// ============================================================
// OPEN FEATURE
// ============================================================

void openSelectedFeature()
{
    switch (selectedMenu)
    {
       case 0:

    // Reload latest Todo data from storage
    for (int i = 0; i < 3; i++)
    {
        todoCompleted[i] = loadTodoState(i);

        loadTodoName(
            i,
            todoNames[i],
            sizeof(todoNames[i])
        );
    }

    currentScreen = SCREEN_TODO;
    break;

        case 1:
            currentScreen = SCREEN_FOCUS;
            break;

        case 2:

    // Reload latest Habit data from storage
    for (int i = 0; i < 3; i++)
    {
        habitCompleted[i] = loadHabitState(i);

        loadHabitName(
            i,
            habitNames[i],
            sizeof(habitNames[i])
        );
    }

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
}


// ============================================================
// TODO INPUT
// ============================================================
void handleClockInput(ButtonEvent event)
{
    if (event == BTN_HOME_PRESSED)
    {
        currentScreen = SCREEN_MENU;
        drawCurrentScreen();
    }
}

void uiUpdate()
{

    if (isAlarmRinging() &&
        currentScreen != SCREEN_ALARM_RINGING)
    {
        currentScreen = SCREEN_ALARM_RINGING;
        drawCurrentScreen();
    }
    static int lastMinute = -1;

    if (currentScreen == SCREEN_FOCUS && focusRunning)
    {
        updateFocusDisplay();
    }

    if (currentScreen == SCREEN_HOME)
    {
        time_t now = time(nullptr);
        struct tm* timeinfo = localtime(&now);

        if (timeinfo != nullptr && timeinfo->tm_min != lastMinute)
        {
            lastMinute = timeinfo->tm_min;
            drawCurrentScreen();
        }
    }
static unsigned long lastClockUpdate = 0;

if (currentScreen == SCREEN_CLOCK)
{
    if (millis() - lastClockUpdate >= 1000)
    {
        lastClockUpdate = millis();

        clockUpdate();
        updateClockDisplay();
    }
}
}
void updateFocusDisplay()
{
    unsigned long elapsed = focusPausedTime;

    if (focusRunning)
    {
        elapsed = millis() - focusStartTime;
    }

    if (elapsed > FOCUS_DURATION)
        elapsed = FOCUS_DURATION;

    unsigned long remaining = FOCUS_DURATION - elapsed;

    int minutes = remaining / 60000;
    int seconds = (remaining % 60000) / 1000;

    // -------------------------
    // TIMER
    // -------------------------

    char timerText[10];

    sprintf(
        timerText,
        "%02d:%02d",
        minutes,
        seconds
    );

    // Clear ONLY timer area
   lcd.fillRect(
    10,
    55,
    220,
    80,
    BG
);

    lcd.setFont(&fonts::Font6);
    lcd.setTextSize(1);
    lcd.setTextDatum(middle_center);
    lcd.setTextColor(TEXT, BG);

    lcd.drawString(
        timerText,
        120,
        110
    );

    // -------------------------
    // PROGRESS BAR
    // -------------------------

    int progressWidth =
        (168UL * elapsed) / FOCUS_DURATION;

    lcd.fillRoundRect(
        36,
        195,
        168,
        6,
        3,
        DARK_BAR
    );

    if (progressWidth > 0)
    {
        lcd.fillRoundRect(
            36,
            195,
            progressWidth,
            6,
            3,
            ACCENT
        );
    }

    // -------------------------
    // SESSION TIME
    // -------------------------

    char sessionText[20];

    sprintf(
        sessionText,
        "%lu:%02lu / 25:00",
        elapsed / 60000,
        (elapsed % 60000) / 1000
    );

    lcd.fillRect(
        55,
        210,
        130,
        25,
        BG
    );

    lcd.setFont(&fonts::Font4);
    lcd.setTextDatum(middle_center);
    lcd.setTextColor(SUBTEXT, BG);

    lcd.drawString(
        sessionText,
        120,
        225
    );
}


void handleProgressInput(ButtonEvent event)
{
    if (event == BTN_HOME_PRESSED)
    {
        currentScreen = SCREEN_MENU;
        drawCurrentScreen();
    }
}

// ============================================================
// ALARM INPUT
// ============================================================


// ============================================================
// ALARM INPUT
// ============================================================

void handleAlarmInput(ButtonEvent event)
{
    // --------------------------------------------------------
    // SELECT ALARM
    // --------------------------------------------------------

    if (alarmEditField == ALARM_SELECT)
    {
        if (event == BTN_LEFT_PRESSED)
        {
            selectedAlarm--;

            if (selectedAlarm < 0)
                selectedAlarm = 2;

            drawCurrentScreen();
        }

        else if (event == BTN_RIGHT_PRESSED)
        {
            selectedAlarm++;

            if (selectedAlarm > 2)
                selectedAlarm = 0;

            drawCurrentScreen();
        }

        else if (event == BTN_OK_PRESSED)
        {
            alarmEditField = ALARM_HOUR;
            drawCurrentScreen();
        }

        else if (event == BTN_HOME_PRESSED)
        {
            currentScreen = SCREEN_MENU;
            drawCurrentScreen();
        }
    }

    // --------------------------------------------------------
    // EDIT HOUR
    // --------------------------------------------------------

    else if (alarmEditField == ALARM_HOUR)
    {
        if (event == BTN_LEFT_PRESSED)
        {
            int hour = getAlarmHour(selectedAlarm);

            hour--;

            if (hour < 0)
                hour = 23;

            setAlarmHour(selectedAlarm, hour);

            drawCurrentScreen();
        }

        else if (event == BTN_RIGHT_PRESSED)
        {
            int hour = getAlarmHour(selectedAlarm);

            hour++;

            if (hour > 23)
                hour = 0;

            setAlarmHour(selectedAlarm, hour);

            drawCurrentScreen();
        }

        else if (event == BTN_OK_PRESSED)
        {
            alarmEditField = ALARM_MINUTE;
            drawCurrentScreen();
        }

        else if (event == BTN_HOME_PRESSED)
        {
            alarmEditField = ALARM_SELECT;
            drawCurrentScreen();
        }
    }

    // --------------------------------------------------------
    // EDIT MINUTE
    // --------------------------------------------------------

    else if (alarmEditField == ALARM_MINUTE)
    {
        if (event == BTN_LEFT_PRESSED)
        {
            int minute = getAlarmMinute(selectedAlarm);

            minute--;

            if (minute < 0)
                minute = 59;

            setAlarmMinute(selectedAlarm, minute);

            drawCurrentScreen();
        }

        else if (event == BTN_RIGHT_PRESSED)
        {
            int minute = getAlarmMinute(selectedAlarm);

            minute++;

            if (minute > 59)
                minute = 0;

            setAlarmMinute(selectedAlarm, minute);

            drawCurrentScreen();
        }

        else if (event == BTN_OK_PRESSED)
        {
            alarmEditField = ALARM_ENABLED;
            drawCurrentScreen();
        }

        else if (event == BTN_HOME_PRESSED)
        {
            alarmEditField = ALARM_SELECT;
            drawCurrentScreen();
        }
    }

    // --------------------------------------------------------
    // ENABLE / DISABLE
    // --------------------------------------------------------

    else if (alarmEditField == ALARM_ENABLED)
    {
        if (event == BTN_LEFT_PRESSED ||
            event == BTN_RIGHT_PRESSED)
        {
            bool enabled = isAlarmEnabled(selectedAlarm);

            setAlarmEnabled(
                selectedAlarm,
                !enabled
            );

            drawCurrentScreen();
        }

        else if (event == BTN_OK_PRESSED)
        {
            alarmEditField = ALARM_SELECT;
            drawCurrentScreen();
        }

        else if (event == BTN_HOME_PRESSED)
        {
            alarmEditField = ALARM_SELECT;
            drawCurrentScreen();
        }
    }
}