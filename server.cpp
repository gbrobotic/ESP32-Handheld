#include "server.h"
#include <Arduino.h>
#include <WebServer.h>
#include "wifi.h"
#include "clock.h"
#include "data_storage.h"
WebServer server(80);

void handleRoot()
{
    String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>ESP32 Handheld</title>

    <style>
        body {
            font-family: Arial, sans-serif;
            background: #101820;
            color: white;
            text-align: center;
            padding-top: 50px;
        }

        h1 {
            font-size: 28px;
        }

        .status {
            color: #45F5A0;
            font-size: 20px;
        }
    </style>
</head>

<body>

    <h1>ESP32 Handheld</h1>

    <p class="status">● Connected</p>

    <p>Web server is working.</p>

</body>
</html>
)rawliteral";

    server.send(200, "text/html", page);
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

void serverInit()
{
    if (!wifiConnected())
    {
        Serial.println("Server not started: Wi-Fi not connected.");
        return;
    }

   server.on("/", handleRoot);

server.on(
    "/api/status",
    HTTP_GET,
    handleStatus
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

    server.send(
        200,
        "application/json",
        "{\"success\":true,\"deleted\":true}"
    );
}