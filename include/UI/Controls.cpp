// Copyright (c) F4HWN Armel. All rights reserved.
#include "Controls.hpp"

#include "../core/ICSMeter.hpp"
#include "../net/Controls.hpp"
#include "../misc/Beeper.hpp"
#include "../misc/BackLight.hpp"
#include "UI.hpp"


namespace ICSMeter
{

  namespace UI
  {

    using namespace net;
    using namespace Utils;
    using namespace modules;


    void clearData()
    {
      Needle::reset();
      Measure::reset();
      Battery::reset();
    }


    void buttonTask(void *pvParameters)
    {
      for (;;) {

        screenshot::check(); // check for queued screenshot request
        checkButtons();    // check buttons status

        if(btnA || btnB || btnC) {
          ScreenSaver::reset();
        }

        Beeper::handle();
        Settings::handle();

        vTaskDelay(pdMS_TO_TICKS(10));
      }
    }


  };
};

