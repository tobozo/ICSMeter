#pragma once

#include "../main/core.h"

#include "Controls.hpp"

namespace ICSMeter
{

  namespace UI
  {

    uint8_t sOld = 255;
    uint8_t SWROld = 255;
    uint8_t powerOld = 255;
    uint8_t batteryLevelOld = 0;
    float angleOld = 0;
    int8_t measureOld = 5;
    boolean batteryCharginglOld = true;

    String frequencyOld = "";
    String filterOld = "";
    String modeOld = "";
    String valStringOld = "";
    String subValStringOld = "";

    void clearData();

    void viewGUI();

    namespace Theme
    {
      uint16_t TFT_BACK   = 0;
      uint16_t TFT_FRONT  = 0;

      const RGBColor TFT_BACK_CLASSIC  = {255, 248, 236};
      const RGBColor TFT_FRONT_CLASSIC = {0, 0, 0};

      const RGBColor TFT_BACK_DARK     = {0, 0, 0};
      const RGBColor TFT_FRONT_DARK    = {255, 255, 255};

      const RGBColor TFT_MODE_BORDER   = {115, 135, 159};
      const RGBColor TFT_MODE_BACK     = {24, 57, 92};

      const uint16_t TFT_MENU_BORDER   = M5.Lcd.color565(115, 135, 159);
      const uint16_t TFT_MENU_BACK     = M5.Lcd.color565(24, 57, 92);
      const uint16_t TFT_MENU_SELECT   = M5.Lcd.color565(255, 255, 255);

      // Needle
      const RGBColor TFT_NEDDLE_1      = {241, 120, 100};
      const RGBColor TFT_NEDDLE_2      = {241, 140, 120};

      // Reset color
      void resetColor();
      // View GUI
    };


    namespace Battery
    {
      // Print battery
      void viewBattery();
    };

    namespace Needle
    {
      // Print needle
      void needle(float_t angle, uint16_t a = 0, uint16_t b = 200, uint16_t c = 0, uint16_t d = 100);
    };

    namespace Measure
    {
      // Print Measure
      void viewMeasure();
      // Print value
      void value(String valString, uint8_t x = 160, uint8_t y = 180);
      // Print sub value
      void subValue(String valString, uint8_t x = 160, uint8_t y = 206);
    };

    namespace ScreenSaver
    {
      // Manage screensaver
      void wakeAndSleep();
    };


  };
};
