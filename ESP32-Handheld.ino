#include "display.h"
#include "buttons.h"
#include "ui.h"
#include "data_storage.h"
#include "clock.h"
#include "wifi.h"
#include "alarm.h"
#include "server.h"

// ======================================================
// SETUP
// ======================================================
void setup()
{
    Serial.begin(115200);

    displayInit();
    buttonsInit();
    storageInit();

wifiInit();
clockInit();

serverInit();

alarmInit();

    uiInit();
}
// ======================================================
// LOOP
// ======================================================
void loop()
{
    clockUpdate();
    alarmUpdate();

    ButtonEvent event = buttonsUpdate();

    if (event != BTN_NONE)
    {
        uiHandleInput(event);
    }

    uiUpdate();

    serverUpdate();

    delay(100);
}