#include "data_storage.h"
#include <Preferences.h>

Preferences preferences;

void storageInit()
{
    preferences.begin("handheld", false);
}

// ============================================================
// TODO
// ============================================================

void saveTodoState(int index, bool completed)
{
    if (index < 0 || index >= 3)
        return;

    char key[16];
    sprintf(key, "todo%d", index);

    preferences.putBool(key, completed);
}

bool loadTodoState(int index)
{
    if (index < 0 || index >= 3)
        return false;

    char key[16];
    sprintf(key, "todo%d", index);

    return preferences.getBool(key, false);
}


// ============================================================
// TODO NAMES
// ============================================================

// ============================================================
// TODO NAMES
// ============================================================

void saveTodoName(int index, const char* name)
{
    if (index < 0 || index >= 3)
        return;

    char key[16];

    sprintf(
        key,
        "todo%d_name",
        index
    );

    preferences.putString(
        key,
        name
    );
}

void loadTodoName(
    int index,
    char* name,
    size_t maxLength
)
{
    if (index < 0 || index >= 3)
        return;

    if (name == nullptr || maxLength == 0)
        return;

    char key[16];

    sprintf(
        key,
        "todo%d_name",
        index
    );

    String savedName =
        preferences.getString(
            key,
            ""
        );

    if (savedName.length() == 0)
    {
        name[0] = '\0';
        return;
    }

    savedName.toCharArray(
        name,
        maxLength
    );
}

// ============================================================
// TODO EXISTS
// ============================================================

bool todoExists(int index)
{
    if (index < 0 || index >= 3)
        return false;

    char key[16];

    sprintf(
        key,
        "todo%d_name",
        index
    );

    return preferences.isKey(key);
}



// ============================================================
// DELETE TODO
// ============================================================

void deleteTodo(int index)
{
    if (index < 0 || index >= 3)
        return;

    char nameKey[16];
    char stateKey[16];

    sprintf(
        nameKey,
        "todo%d_name",
        index
    );

    sprintf(
        stateKey,
        "todo%d",
        index
    );

    preferences.remove(nameKey);
    preferences.remove(stateKey);
}
// ============================================================
// HABITS
// ============================================================

void saveHabitState(int index, bool completed)
{
    if (index < 0 || index >= 3)
        return;

    char key[16];
    sprintf(key, "habit%d", index);

    preferences.putBool(key, completed);
}

bool loadHabitState(int index)
{
    if (index < 0 || index >= 3)
        return false;

    char key[16];
    sprintf(key, "habit%d", index);

    return preferences.getBool(key, false);
}
// ============================================================
// HABIT NAMES
// ============================================================

void saveHabitName(int index, const char* name)
{
    if (index < 0 || index >= 3)
        return;

    char key[16];

    sprintf(
        key,
        "habit%d_name",
        index
    );

    preferences.putString(
        key,
        name
    );
}

void loadHabitName(
    int index,
    char* name,
    size_t maxLength
)
{
    if (index < 0 || index >= 3)
        return;

    if (name == nullptr || maxLength == 0)
        return;

    char key[16];

    sprintf(
        key,
        "habit%d_name",
        index
    );

    String savedName =
        preferences.getString(
            key,
            ""
        );

    if (savedName.length() == 0)
    {
        name[0] = '\0';
        return;
    }

    savedName.toCharArray(
        name,
        maxLength
    );
}

// ============================================================
// HABIT EXISTS
// ============================================================

bool habitExists(int index)
{
    if (index < 0 || index >= 3)
        return false;

    char key[16];

    sprintf(
        key,
        "habit%d_name",
        index
    );

    return preferences.isKey(key);
}

// ============================================================
// DELETE HABIT
// ============================================================

void deleteHabit(int index)
{
    if (index < 0 || index >= 3)
        return;

    char nameKey[16];
    char stateKey[16];

    sprintf(
        nameKey,
        "habit%d_name",
        index
    );

    sprintf(
        stateKey,
        "habit%d",
        index
    );

    preferences.remove(nameKey);
    preferences.remove(stateKey);
}
// ============================================================
// ALARM
// ============================================================

void saveAlarmData(int index, int hour, int minute, bool enabled)
{
    if (index < 0 || index >= 3)
        return;

    char hourKey[16];
    char minuteKey[16];
    char enabledKey[16];

    sprintf(hourKey, "alarm%d_h", index);
    sprintf(minuteKey, "alarm%d_m", index);
    sprintf(enabledKey, "alarm%d_e", index);

    preferences.putInt(hourKey, hour);
    preferences.putInt(minuteKey, minute);
    preferences.putBool(enabledKey, enabled);
}

void loadAlarmData(
    int index,
    int &hour,
    int &minute,
    bool &enabled
)
{
    if (index < 0 || index >= 3)
        return;

    char hourKey[16];
    char minuteKey[16];
    char enabledKey[16];

    sprintf(hourKey, "alarm%d_h", index);
    sprintf(minuteKey, "alarm%d_m", index);
    sprintf(enabledKey, "alarm%d_e", index);

    hour = preferences.getInt(hourKey, hour);
    minute = preferences.getInt(minuteKey, minute);
    enabled = preferences.getBool(enabledKey, enabled);
}