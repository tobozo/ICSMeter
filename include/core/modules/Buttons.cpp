#include "../ICSMeter.hpp"

namespace ICSMeter
{

  namespace modules
  {

    namespace buttons
    {

      const uint32_t poll_delay = 25; // min delay (milliseconds) between each button poll
      uint32_t last_poll = millis();

      void loop()
      {
        uint32_t now = millis();

        if( buttons::last_poll + buttons::poll_delay > now ) return;
        buttons::last_poll = now;

        M5.update();

        // alias WebUI and I/O button states
        btnA = M5.BtnA.isPressed()  || buttonLeftPressed;
        btnB = M5.BtnB.wasPressed() || buttonCenterPressed; // no repeat on this one
        btnC = M5.BtnC.isPressed()  || buttonRightPressed;

        if( buttonLeftPressed || buttonCenterPressed || buttonRightPressed ) {
          log_d("Auto cancel bubble for WebUI");
          // since it's been propagated already
          buttonLeftPressed = buttonCenterPressed = buttonRightPressed = false;
        }
      }

      bool hasBubble()
      {
        return btnA || btnB || btnC;
      }


      void cancelBubble()
      {
        btnA = btnB = btnC = false;
      }

    };


  };

};
