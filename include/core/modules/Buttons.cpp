#include "../ICSMeter.hpp"

namespace ICSMeter
{

  namespace modules
  {

    namespace buttons
    {

      void check()
      {
        const uint32_t buttons_poll_delay = 0; // min delay (milliseconds) between each button poll
        static uint32_t last_button_poll = millis();
        uint32_t now = millis();

        if( last_button_poll + buttons_poll_delay > now ) return;
        last_button_poll = now;

        M5.update();

        // also forward button presses from WebUI
        btnA = M5.BtnA.isPressed() || buttonLeftPressed;
        btnB = M5.BtnB.wasPressed() || buttonCenterPressed; // no repeat on this one
        btnC = M5.BtnC.isPressed() || buttonRightPressed;
      }

    };


  };

};
