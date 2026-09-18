#include "display.h"
#include "buttons.h"
#include "ui.h"
#include "data_storage.h"


// ======================================================
// SETUP
// ======================================================

void setup()
{
    Serial.begin(115200);

    // Initialize hardware
    displayInit();
    buttonsInit();

    // Initialize storage
    storageInit();

    // Initialize UI
    uiInit();
}


// ======================================================
// LOOP
// ======================================================

void loop()
{
    ButtonEvent event = buttonsUpdate();

    if (event != BTN_NONE)
    {
        uiHandleInput(event);
    }
}