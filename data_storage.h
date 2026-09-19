#pragma once
#include <stddef.h>
void storageInit();

// TODO
void saveTodoState(int index, bool completed);
bool loadTodoState(int index);

void saveTodoName(int index, const char* name);
void loadTodoName(int index, char* name, size_t maxLength);

bool todoExists(int index);

void deleteTodo(int index);
// ============================================================
// HABITS
// ============================================================

void saveHabitState(int index, bool completed);
bool loadHabitState(int index);

void saveHabitName(int index, const char* name);
void loadHabitName(int index, char* name, size_t maxLength);

bool habitExists(int index);

void deleteHabit(int index);
// Alarm
void saveAlarmData(int index, int hour, int minute, bool enabled);
void loadAlarmData(int index, int &hour, int &minute, bool &enabled);