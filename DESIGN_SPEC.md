# ESP32 Handheld Design Spec

## Overview
This project is a compact ESP32 handheld device UI inspired by a productivity dashboard. The display uses a 240x320 ST7789 panel and a 4-button input layout.

## File Structure
- `display.h` / `display.cpp`: LCD initialization and display object definition.
- `buttons.h` / `buttons.cpp`: button pins and press-release helpers.
- `ui.h` / `ui.cpp`: all screen drawing and navigation logic.
- `colors.h`: shared color palette for the UI.
- `data_storage.h` / `data_storage.cpp`: state and persistence placeholders.
- `ESP32-Handheld.ino`: Arduino entry point.

## Screens
- Home
- Menu
- Todo
- Focus
- Habits
- Progress
- Clock
- Alarm
- System

## Planned Enhancements
- EEPROM-backed task storage
- Real time clock synchronization
- Wi-Fi and server status integration
- alarm scheduling and notifications
- battery state and power management

## Hardware Notes
- Display: ST7789 SPI
- Buttons: D0, D4, D5, D6
- Use `INPUT_PULLUP` for all buttons
- Keep SPI wiring stable for the current ESP32 configuration
