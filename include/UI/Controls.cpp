// Copyright (c) F4HWN Armel. All rights reserved.
#include "Controls.hpp"

#include "../core/ICSMeter.hpp"
#include "../net/Controls.hpp"
#include "UI.hpp"


namespace ICSMeter
{

  namespace UI
  {

    using namespace net;
    using namespace Utils;


    void clearData()
    {
      Needle::angleOld      = 0;
      Measure::valueOld     = 5;
      Battery::levelOld     = 0;
      Battery::charginglOld = true;

      filterOld       = "";
      modeOld         = "";
      valStringOld    = "";
      subValStringOld = "";
    }


    int8_t getBatteryLevel(bool type)
    {
      return Battery::getLevel(type);
    }


    bool isCharging()
    {
      return Battery::isCharging();
    }


    void setBrightness(uint8_t value)
    {
      tft.setBrightness(value);
    }


    void shutdown()
    {
      ScreenSaver::shutdown();
    }


    void handleBeep()
    {
      if (M5.Speaker.isEnabled() && beep > 0) {
        // set master volume (0~255)
        M5.Speaker.setVolume(beep);
        if(btnA || btnC) {
          // play beep sound 1000Hz 100msec (background task)
          M5.Speaker.tone(1000, 50);
        } else if(btnB) {
          // play beep sound 2000Hz 100msec (background task)
          M5.Speaker.tone(2000, 50);
        }
        // wait done
        while (M5.Speaker.isPlaying()) { vTaskDelay(1); }
      }
    }


    void buttonTask(void *pvParameters)
    {

      for (;;) {
        // check for queued screenshot request
        checkScreenshot();
        // check buttons status
        checkButtons();

        if(btnA || btnB || btnC) {

          handleBeep();

          ScreenSaver::timer = millis();
          brightnessOld = preferences.getUInt("brightness");
          Transverter::valueOld = preferences.getUInt("transverter");
          ScreenSaver::countdownOld = preferences.getUInt("screensaver");
          Theme::themeOld = preferences.getUInt("theme");
        }

        Settings::handle();

        vTaskDelay(pdMS_TO_TICKS(10));
      }
    }


  };
};

