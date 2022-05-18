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
      //DataMode::reset();
      //Transverter::reset();
    }


    void netTask(void *pvParameters)
    {
      for (;;) {

        // do NOT draw from this task

        screenshot::check(); // check for queued screenshot request

        vTaskDelay( 10 );
      }
    }


  };
};

