# ESP32 Handheld

A pocket-sized productivity device built on an ESP32-S3. It runs a todo list, habit tracker, Pomodoro-style focus timer, clock and alarms on a 240×320 color display, controlled by four buttons. The same data is also available from a web dashboard that the device hosts over Wi-Fi, so changes made on a phone or laptop show up on the device instantly, and the other way round.

## Features

- **Todo**: up to 3 tasks, mark done, add, rename and delete
- **Habits**: up to 3 daily habits with completion tracking
- **Focus timer**: start, pause and reset, with 15 / 25 / 50 minute presets (default 25)
- **Progress**: task and habit completion at a glance
- **Clock**: live time and date, synced over the internet (NTP, IST)
- **Alarms**: 3 alarms with a buzzer and a full-screen ringing screen
- **System**: Wi-Fi status, device IP, storage and device info
- **Web dashboard**: control everything from a browser on the same network
- **Saved data**: todos, habits and alarms survive power-off (stored in flash)

## Hardware

| Part | Notes |
|---|---|
| ESP32-S3 board | Code uses XIAO-style pin names (D0–D10), e.g. Seeed Studio XIAO ESP32-S3 |
| 2.0" ST7789 TFT, 240×320, SPI | |
| 4 × push buttons | Left, OK, Right, Home |
| Active buzzer | For alarms |
| Wires / breadboard or PCB | |

### Wiring

**Display (ST7789)**

| Display pin | ESP32 pin |
|---|---|
| SCL / SCLK | D8 |
| SDA / MOSI | D10 |
| DC | D2 |
| CS | D3 |
| RST | D1 |
| VCC | 3.3V |
| GND | GND |
| BL / LED | 3.3V (backlight is not controlled in code) |

**Buttons**: one leg to the pin, the other leg to GND. Internal pull-ups are used, so no resistors are needed.

| Button | Pin |
|---|---|
| Left | D0 |
| OK | D4 |
| Right | D5 |
| Home | D6 |

**Buzzer**: active buzzer `+` to D7, `−` to GND.

## Software setup

1. Install **Arduino IDE 2** and the **ESP32 board package** (by Espressif).
2. Install the **LovyanGFX** library from Library Manager. Everything else (`WiFi`, `WebServer`, `LittleFS`, `Preferences`) comes with the ESP32 core.
3. Open `ESP32-Handheld.ino` and set your Wi-Fi name and password in `wifi.cpp`:
   ```cpp
   const char* WIFI_SSID = "your-wifi-name";
   const char* WIFI_PASSWORD = "your-password";
   ```
4. Select your board (e.g. *XIAO_ESP32S3*) and port, then **Upload**.
5. Upload the web dashboard (the `data/` folder) to the board's flash with the **LittleFS upload** plugin for Arduino IDE 2 (`arduino-littlefs-upload`): press `Ctrl+Shift+P` and run *Upload LittleFS to Pico/ESP8266/ESP32*. Close the Serial Monitor first.
6. Open the Serial Monitor at **115200** baud. The device prints its IP address once Wi-Fi connects.

> The clock is set to **IST (UTC+5:30)**. To change it, edit `IST_OFFSET_SECONDS` in `clock.cpp`.

## Using the device

| Button | What it does |
|---|---|
| Left / Right | Move through the menu and items |
| OK | Open, select, toggle; start/pause the focus timer; stop a ringing alarm |
| Home | Go back to the menu / home screen |

Screens: Home → Menu → Todo, Focus, Habits, Progress, Clock, Alarm, System.

## Web dashboard

Connect your phone or laptop to the same Wi-Fi and open the IP shown on the System screen (e.g. `http://192.168.1.25`). The dashboard shows tasks, habits, the focus timer, alarms, progress and device status, and refreshes itself every few seconds.

To work on the dashboard from your PC, open `data/index.html` locally and enter the device IP in the **Connection** box on the Device page.

### API

All responses are JSON. Parameters go in the query string or form body.

| Method | Endpoint | Params | Purpose |
|---|---|---|---|
| GET | `/api/status` | | Wi-Fi, IP, time, date |
| GET | `/api/todos` | | List todos |
| GET/POST | `/api/todo` | `index`, `completed` | Mark a todo done / not done |
| GET/POST | `/api/todo/create` | `name` | Add a todo |
| GET/POST | `/api/todo/edit` | `index`, `name` | Rename a todo |
| GET/POST | `/api/todo/delete` | `index` | Delete a todo |
| GET | `/api/habits` | | List habits |
| GET/POST | `/api/habit` | `index`, `completed` | Mark a habit done / not done |
| GET/POST | `/api/habit/create` | `name` | Add a habit |
| GET/POST | `/api/habit/edit` | `index`, `name` | Rename a habit |
| GET/POST | `/api/habit/delete` | `index` | Delete a habit |
| GET | `/api/focus` | | Timer state and time left |
| POST | `/api/focus/start` | | Start the timer |
| POST | `/api/focus/pause` | | Pause the timer |
| POST | `/api/focus/reset` | | Reset the timer |
| POST | `/api/focus/preset` | `minutes` = 15, 25 or 50 | Set timer length |
| GET | `/api/alarms` | | List alarms |
| POST | `/api/alarm` | `index`, `hour`, `minute`, `enabled` | Update an alarm |

Example: `curl -X POST "http://192.168.1.25/api/focus/preset?minutes=50"`

## Project structure

```
ESP32-Handheld.ino   Entry point (setup and main loop)
display.*            ST7789 setup with LovyanGFX
buttons.*            Button pins and debounced input
ui.*                 All screens, navigation and the focus timer
colors.h             Screen size and color palette
clock.*              NTP time sync
alarm.*              Alarm checking and buzzer
data_storage.*       Saves todos, habits and alarms to flash (Preferences)
wifi.*               Wi-Fi connection
server.*             Web server and REST API
data/                Web dashboard (index.html, app.js, style.css)
DESIGN_SPEC.md       Design notes
```

## Current limits

- 3 slots each for todos, habits and alarms
- Wi-Fi credentials and time zone are set in code
- The System screen shows battery as "USB"; battery monitoring is not wired up yet
- If Wi-Fi does not connect within about 15 seconds, the device still works offline, but the clock and web dashboard stay unavailable

## Planned

- Battery level and power saving
- Wi-Fi setup without editing code
- More todo and habit slots
- Habit streaks over multiple days
