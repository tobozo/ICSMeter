#pragma once


namespace ICSMeter
{

  namespace UI
  {

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
        settings_callback_t callback;
      };

      settings_handler_t menuchoices[] =
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

      const char *choiceBrightness  = "BRIGHTNESS";
      const char *choiceBeep        = "BEEP LEVEL";
      const char *choiceScreensaver = "TIMEOUT";

    };

  };

};
