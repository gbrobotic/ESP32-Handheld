#pragma once

#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device
{
private:

    lgfx::Panel_ST7789 _panel;
    lgfx::Bus_SPI _bus;

public:

    LGFX();
};

extern LGFX lcd;

void displayInit();