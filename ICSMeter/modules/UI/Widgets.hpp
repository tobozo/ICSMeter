#pragma once

#include "Controls.hpp"
#include "Themes/Themes.hpp"


namespace ICSMeter
{

  namespace UI
  {

    namespace Battery
    {
      uint8_t levelOld = 0;
      bool charginglOld = true;
      int8_t getLevel(bool type);
      bool isCharging();
      void draw();
    };


    namespace Measure
    {
      int8_t value = 1;
      int8_t valueOld = 5;
      // for settings menu
      const char *choices[] = {"PWR", "S", "SWR"};
      void draw(); // Print Measure
      void setValue(String valString, uint8_t x = 160, uint8_t y = 180); // Print value
      void subValue(String valString, uint8_t x = 160, uint8_t y = 206); // Print sub value
    };


    namespace Needle
    {
      float angleOld;
      void draw(float_t angle, uint16_t a = 0, uint16_t b = 200, uint16_t c = 0, uint16_t d = 100); // Print needle
    };


    namespace Transverter
    {
      int8_t value = 0;
      int8_t valueOld = 0;
      const double choices[] = { 0, TRANSVERTER_LO_1, TRANSVERTER_LO_2, TRANSVERTER_LO_3, TRANSVERTER_LO_4, TRANSVERTER_LO_5 };
      void draw();
    };


    namespace ScreenSaver
    {
      uint32_t timer;
      uint32_t countdown;
      uint32_t countdownOld;
      bool mode = false;
      void shutdown();
      void handle(); // Manage screensaver
      constexpr const char *settingsLabel = "TIMEOUT";
    };


    namespace Settings
    {
      bool mode = false;
      bool lock = true;
      int8_t choice = 0;
      bool select = false;

      int32_t  x = 44;
      int32_t  y = 4;
      uint32_t w = 320 - (x * 2);
      uint32_t h = 185;

      void draw();
      void handle(); // called from task
      void drawMenu(uint8_t x, uint8_t y, uint16_t w, uint8_t h);
      void drawOption(int8_t settingsChoice, bool settingsSelect, uint8_t x, uint8_t y, uint16_t w); // option choice decorator

      // callbacks
      void onBrowse();
      void onMeasured();
      void onTransverter();
      void onTheme();
      void onBrightness();
      void onBeep();
      void onScreensaver();
      void onIPAddress();
      void onExit();

      typedef void (*settings_callback_t)();
      struct settings_handler_t
      {
        const char* label;
        const settings_callback_t callback;
      };

      const settings_handler_t menuchoices[] =
      {
        { "Measured Values",  onMeasured    },
        { "Transverter Mode", onTransverter },
        { "Themes",           onTheme       },
        { "Brightness",       onBrightness  },
        { "Beep",             onBeep        },
        { "Screensaver",      onScreensaver },
        { "IP Address",       onIPAddress   },
        { "Shutdown",         ScreenSaver::shutdown },
        { "Exit",             onExit        },
      };

      constexpr const char *choiceBrightness  = "BRIGHTNESS";
      constexpr const char *choiceBeep        = "BEEP LEVEL";
    };

  };

};
