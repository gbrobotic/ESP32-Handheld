#include "server.h"
#include <Arduino.h>
#include <WebServer.h>
#include <LittleFS.h>
#include "wifi.h"
#include "clock.h"
#include "data_storage.h"
#include "ui.h"
#include "alarm.h"
WebServer server(80);

void handleRoot()
{
    if (LittleFS.exists("/index.html"))
    {
        File file = LittleFS.open("/index.html", "r");

        server.streamFile(file, "text/html");

        file.close();
    }
    else
    {
        server.send(
            404,
            "text/plain",
            "index.html not found"
        );
    }
}

 

void handleStatus()
{
    String json = "{";

    json += "\"device\":\"ESP32-S3\",";
    json += "\"wifi\":";
    json += wifiConnected() ? "true," : "false,";

    json += "\"ip\":\"";
    json += WiFi.localIP().toString();
    json += "\",";

    json += "\"time\":\"";
    json += getTimeString();
    json += "\",";

    json += "\"date\":\"";
    json += getDateString();
    json += "\"";

    json += "}";

    server.send(
        200,
        "application/json",
        json
    );
}

// ============================================================
// FOCUS API
// ============================================================

void handleFocus()
{
    unsigned long remaining = getFocusRemainingSeconds();
    unsigned long duration = getFocusDurationSeconds();

    unsigned long minutes = remaining / 60;
    unsigned long seconds = remaining % 60;

    String json = "{";
    json += "\"running\":";
    json += isFocusRunningFromWeb() ? "true" : "false";
    json += ",";
    json += "\"remaining\":";
    json += String(remaining);
    json += ",";
    json += "\"minutes\":";
    json += String(minutes);
    json += ",";
   json += "\"seconds\":";
json += String(seconds);
json += ",";
json += "\"duration\":";
json += String(duration);
json += "}";

    server.send(200, "application/json", json);
}

void handleFocusStart()
{
    focusStartFromWeb();

    handleFocus();
}

void handleFocusPause()
{
    focusPauseFromWeb();

    handleFocus();
}

void handleFocusReset()
{
    focusResetFromWeb();

    handleFocus();
}


void handleFocusPreset()
{
    if (!server.hasArg("minutes"))
    {
        server.send(400, "application/json",
                    "{\"error\":\"minutes missing\"}");
        return;
    }

    unsigned long minutes = server.arg("minutes").toInt();

    if (minutes != 15 && minutes != 25 && minutes != 50)
    {
        server.send(400, "application/json",
                    "{\"error\":\"invalid preset\"}");
        return;
    }

    focusSetPreset(minutes);
    handleFocus();
}

void handleAlarms()
{
    String json = "{";
    json += "\"alarms\":[";

    for (int i = 0; i < 3; i++)
    {
        json += "{";

        json += "\"index\":";
        json += String(i);
        json += ",";

        json += "\"hour\":";
        json += String(getAlarmHour(i));
        json += ",";

        json += "\"minute\":";
        json += String(getAlarmMinute(i));
        json += ",";

        json += "\"enabled\":";
        json += isAlarmEnabled(i) ? "true" : "false";

        json += "}";

        if (i < 2)
            json += ",";
    }

    json += "]}";

    server.send(
        200,
        "application/json",
        json
    );
}

void handleAlarmUpdate()
{
    if (!server.hasArg("index"))
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Missing index\"}"
        );

        return;
    }

    int index = server.arg("index").toInt();

    if (index < 0 || index >= 3)
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Invalid alarm index\"}"
        );

        return;
    }

    if (server.hasArg("hour"))
    {
        int hour = server.arg("hour").toInt();
        setAlarmHour(index, hour);
    }

    if (server.hasArg("minute"))
    {
        int minute = server.arg("minute").toInt();
        setAlarmMinute(index, minute);
    }

    if (server.hasArg("enabled"))
    {
        String value = server.arg("enabled");

        bool enabled =
            (value == "true" || value == "1");

        setAlarmEnabled(index, enabled);
    }

    // Update the physical LCD immediately
    drawCurrentScreen();

    // Send updated alarm data back to the browser
    handleAlarms();
}
void handleTodos()
{
    String json = "{";
    json += "\"todos\":[";

    for (int i = 0; i < 3; i++)
    {
        char todoName[32];

        loadTodoName(
            i,
            todoName,
            sizeof(todoName)
        );

        bool completed =
            loadTodoState(i);

        json += "{";

        json += "\"name\":\"";
        json += todoName;
        json += "\",";

        json += "\"completed\":";
        json += completed ? "true" : "false";

        json += "}";

        if (i < 2)
            json += ",";
    }

    json += "]}";

    server.send(
        200,
        "application/json",
        json
    );
}
// ============================================================
// UPDATE TODO API
// ============================================================

void handleTodoUpdate()
{
    // Check required parameters
    if (!server.hasArg("index") ||
        !server.hasArg("completed"))
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Missing index or completed\"}"
        );

        return;
    }

    int index = server.arg("index").toInt();

    String completedValue =
        server.arg("completed");

    // Validate Todo index
    if (index < 0 || index >= 3)
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Invalid todo index\"}"
        );

        return;
    }

    bool completed =
        (completedValue == "true" ||
         completedValue == "1");

    // Save to ESP32 storage
    saveTodoState(
        index,
        completed
    );
drawCurrentScreen();
    Serial.print("Todo ");
    Serial.print(index);
    Serial.print(" updated: ");
    Serial.println(completed ? "COMPLETED" : "NOT COMPLETED");

    // Response
    String json = "{";

    json += "\"success\":true,";
    json += "\"index\":";
    json += index;
    json += ",";

    json += "\"completed\":";
    json += completed ? "true" : "false";

    json += "}";

    server.send(
        200,
        "application/json",
        json
    );
}


// ============================================================
// EDIT TODO NAME
// ============================================================

void handleTodoEdit()
{
    if (!server.hasArg("index") ||
        !server.hasArg("name"))
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Missing index or name\"}"
        );

        return;
    }

    int index = server.arg("index").toInt();
    String name = server.arg("name");

    if (index < 0 || index >= 3)
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Invalid todo index\"}"
        );

        return;
    }

    if (name.length() == 0)
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Todo name cannot be empty\"}"
        );

        return;
    }

    // Limit name length
    if (name.length() > 31)
    {
        name = name.substring(0, 31);
    }

    saveTodoName(
        index,
        name.c_str()
    );
drawCurrentScreen();
    Serial.print("Todo ");
    Serial.print(index);
    Serial.print(" renamed to: ");
    Serial.println(name);

    String json = "{";

    json += "\"success\":true,";
    json += "\"index\":";
    json += index;
    json += ",";

    json += "\"name\":\"";
    json += name;
    json += "\"";

    json += "}";

    server.send(
        200,
        "application/json",
        json
    );
}


// ============================================================
// CREATE TODO
// ============================================================

void handleTodoCreate()
{
    if (!server.hasArg("name"))
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Missing name\"}"
        );

        return;
    }

    String name = server.arg("name");

    if (name.length() == 0)
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Todo name cannot be empty\"}"
        );

        return;
    }

    if (name.length() > 31)
    {
        name = name.substring(0, 31);
    }

    // Find first empty slot
    int freeIndex = -1;

    for (int i = 0; i < 3; i++)
    {
        if (!todoExists(i))
        {
            freeIndex = i;
            break;
        }
    }

    if (freeIndex == -1)
    {
        server.send(
            409,
            "application/json",
            "{\"error\":\"No empty Todo slot\"}"
        );

        return;
    }

    // Save Todo name
    saveTodoName(
        freeIndex,
        name.c_str()
    );

    // New Todo starts incomplete
    saveTodoState(
        freeIndex,
        false
    );
drawCurrentScreen();
    Serial.print("Created Todo ");
    Serial.print(freeIndex);
    Serial.print(": ");
    Serial.println(name);

    String json = "{";

    json += "\"success\":true,";
    json += "\"index\":";
    json += freeIndex;
    json += ",";

    json += "\"name\":\"";
    json += name;
    json += "\",";

    json += "\"completed\":false";

    json += "}";

    server.send(
        200,
        "application/json",
        json
    );
}


// ============================================================
// DELETE TODO
// ============================================================

void handleTodoDelete()
{
    if (!server.hasArg("index"))
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Missing index\"}"
        );

        return;
    }

    int index = server.arg("index").toInt();

    if (index < 0 || index >= 3)
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Invalid todo index\"}"
        );

        return;
    }

    if (!todoExists(index))
    {
        server.send(
            404,
            "application/json",
            "{\"error\":\"Todo does not exist\"}"
        );

        return;
    }

    deleteTodo(index);
drawCurrentScreen();
    Serial.print("Deleted Todo ");
    Serial.println(index);

    String json = "{";

    json += "\"success\":true,";
    json += "\"index\":";
    json += index;
    json += ",";

    json += "\"deleted\":true";

    json += "}";

    server.send(
        200,
        "application/json",
        json
    );
}

// ============================================================
// HABIT FUNCTION DECLARATIONS
// ============================================================

void handleHabits();
void handleHabitUpdate();
void handleHabitEdit();
void handleHabitCreate();
void handleHabitDelete();
void handleFocus();
void handleFocusStart();
void handleFocusPause();
void handleFocusReset();
void handleFocusPreset();
void handleAlarms();
void handleAlarmUpdate();

void serverInit()
{


    if (!wifiConnected())
    {
        Serial.println("Server not started: Wi-Fi not connected.");
        return;
    }

    // Mount the filesystem that contains the web dashboard.
    if (!LittleFS.begin(true))
    {
        Serial.println("LittleFS mount failed.");
    }
    else
    {
        Serial.println("LittleFS mounted.");
    }

    // Allow a browser served from localhost or another device to call the API.
    server.enableCORS(true);

    // Serve index.html, app.js and style.css directly from the ESP32.
    server.on("/", handleRoot);

server.on(
    "/api/status",
    HTTP_GET,
    handleStatus
);
// Focus
server.on("/api/focus", HTTP_GET, handleFocus);

server.on(
    "/api/focus/start",
    HTTP_POST,
    handleFocusStart
);

server.on(
    "/api/focus/pause",
    HTTP_POST,
    handleFocusPause
);

server.on(
    "/api/focus/reset",
    HTTP_POST,
    handleFocusReset
);

server.on(
    "/api/focus/preset",
    HTTP_POST,
    handleFocusPreset
);

server.on(
    "/api/alarms",
    HTTP_GET,
    handleAlarms
);

server.on(
    "/api/alarm",
    HTTP_POST,
    handleAlarmUpdate
);

server.on(
    "/api/todos",
    HTTP_GET,
    handleTodos
);

server.on(
    "/api/todo",
    HTTP_GET,
    handleTodoUpdate
);

server.on(
    "/api/todo",
    HTTP_POST,
    handleTodoUpdate
);

server.on(
    "/api/todo/edit",
    HTTP_GET,
    handleTodoEdit
);

server.on(
    "/api/todo/edit",
    HTTP_POST,
    handleTodoEdit
);

server.on(
    "/api/todo/create",
    HTTP_GET,
    handleTodoCreate
);

server.on(
    "/api/todo/create",
    HTTP_POST,
    handleTodoCreate
);

server.on(
    "/api/todo/delete",
    HTTP_GET,
    handleTodoDelete
);

server.on(
    "/api/todo/delete",
    HTTP_POST,
    handleTodoDelete
);

server.on(
    "/api/habits",
    HTTP_GET,
    handleHabits
);

server.on(
    "/api/habit",
    HTTP_GET,
    handleHabitUpdate
);

server.on(
    "/api/habit",
    HTTP_POST,
    handleHabitUpdate
);

server.on(
    "/api/habit/edit",
    HTTP_GET,
    handleHabitEdit
);

server.on(
    "/api/habit/edit",
    HTTP_POST,
    handleHabitEdit
);

server.on(
    "/api/habit/create",
    HTTP_GET,
    handleHabitCreate
);

server.on(
    "/api/habit/create",
    HTTP_POST,
    handleHabitCreate
);

server.on(
    "/api/habit/delete",
    HTTP_GET,
    handleHabitDelete
);

server.on(
    "/api/habit/delete",
    HTTP_POST,
    handleHabitDelete
);

// CORS preflight support for future browser requests.
server.on(
    "/api/status",
    HTTP_OPTIONS,
    []()
    {
        server.send(204);
    }
);
server.serveStatic("/", LittleFS, "/");
server.begin();

    Serial.println("Web server started.");
}

void serverUpdate()

{
    if (wifiConnected())
    {
        server.handleClient();
    }
}

// ============================================================
// HABITS API
// ============================================================

void handleHabits()
{
    String json = "{";
    json += "\"habits\":[";

    for (int i = 0; i < 3; i++)
    {
        char habitName[32];

        loadHabitName(
            i,
            habitName,
            sizeof(habitName)
        );

        bool completed =
            loadHabitState(i);

        json += "{";

        json += "\"name\":\"";
        json += habitName;
        json += "\",";

        json += "\"completed\":";
        json += completed ? "true" : "false";

        json += "}";

        if (i < 2)
            json += ",";
    }

    json += "]}";

    server.send(
        200,
        "application/json",
        json
    );
}

void handleHabitUpdate()
{
    if (!server.hasArg("index") ||
        !server.hasArg("completed"))
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Missing index or completed\"}"
        );

        return;
    }

    int index = server.arg("index").toInt();

    if (index < 0 || index >= 3)
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Invalid habit index\"}"
        );

        return;
    }

    String value = server.arg("completed");

    bool completed =
        (value == "true" || value == "1");

    saveHabitState(
        index,
        completed
    );
drawCurrentScreen();
    String json = "{";

    json += "\"success\":true,";
    json += "\"index\":";
    json += index;
    json += ",";

    json += "\"completed\":";
    json += completed ? "true" : "false";

    json += "}";

    server.send(
        200,
        "application/json",
        json
    );
}
void handleHabitEdit()
{
    if (!server.hasArg("index") ||
        !server.hasArg("name"))
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Missing index or name\"}"
        );

        return;
    }

    int index = server.arg("index").toInt();
    String name = server.arg("name");

    if (index < 0 || index >= 3)
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Invalid habit index\"}"
        );

        return;
    }

    if (name.length() == 0)
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Habit name cannot be empty\"}"
        );

        return;
    }

    if (name.length() > 31)
        name = name.substring(0, 31);

    saveHabitName(
        index,
        name.c_str()
    );
drawCurrentScreen();
    server.send(
        200,
        "application/json",
        "{\"success\":true}"
    );
}

void handleHabitCreate()
{
    if (!server.hasArg("name"))
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Missing name\"}"
        );

        return;
    }

    String name = server.arg("name");

    if (name.length() == 0)
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Habit name cannot be empty\"}"
        );

        return;
    }

    if (name.length() > 31)
        name = name.substring(0, 31);

    int freeIndex = -1;

    for (int i = 0; i < 3; i++)
    {
        if (!habitExists(i))
        {
            freeIndex = i;
            break;
        }
    }

    if (freeIndex == -1)
    {
        server.send(
            409,
            "application/json",
            "{\"error\":\"No empty Habit slot\"}"
        );

        return;
    }

    saveHabitName(
        freeIndex,
        name.c_str()
    );

    saveHabitState(
        freeIndex,
        false
    );
drawCurrentScreen();
    String json = "{";

    json += "\"success\":true,";
    json += "\"index\":";
    json += freeIndex;
    json += ",";
    json += "\"name\":\"";
    json += name;
    json += "\",";
    json += "\"completed\":false";

    json += "}";

    server.send(
        200,
        "application/json",
        json
    );
}

void handleHabitDelete()
{
    if (!server.hasArg("index"))
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Missing index\"}"
        );

        return;
    }

    int index = server.arg("index").toInt();

    if (index < 0 || index >= 3)
    {
        server.send(
            400,
            "application/json",
            "{\"error\":\"Invalid habit index\"}"
        );

        return;
    }

    if (!habitExists(index))
    {
        server.send(
            404,
            "application/json",
            "{\"error\":\"Habit does not exist\"}"
        );

        return;
    }

    deleteHabit(index);
drawCurrentScreen();
    server.send(
        200,
        "application/json",
        "{\"success\":true,\"deleted\":true}"
    );
}