#include "display.h"

LGFX::LGFX()
{
    {
        auto cfg = _bus.config();

        cfg.spi_host = SPI2_HOST;
        cfg.spi_mode = 0;

        cfg.freq_write = 40000000;
        cfg.freq_read = 16000000;

        cfg.pin_sclk = D8;
        cfg.pin_mosi = D10;
        cfg.pin_miso = -1;
        cfg.pin_dc   = D2;

        _bus.config(cfg);

        _panel.setBus(&_bus);
    }

    {
        auto cfg = _panel.config();

        cfg.pin_cs  = D3;
        cfg.pin_rst = D1;

        cfg.panel_width  = 240;
        cfg.panel_height = 320;

        cfg.offset_x = 0;
        cfg.offset_y = 0;

        cfg.readable = false;
        cfg.invert = true;
        cfg.rgb_order = false;
        cfg.bus_shared = false;

        _panel.config(cfg);
    }

    setPanel(&_panel);
}


// ======================================================
// GLOBAL DISPLAY
// ======================================================

LGFX lcd;


// ======================================================
// DISPLAY INIT
// ======================================================

void displayInit()
{
    lcd.init();

    lcd.setRotation(0);

    lcd.fillScreen(0x0841);
}