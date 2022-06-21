#include "../ICSMeter.hpp"

namespace ICSMeter
{

  namespace modules
  {

    namespace buttons
    {

      const uint32_t poll_delay = 25; // min delay (milliseconds) between each button poll
      static uint32_t last_poll = millis();

      void loop()
      {
        uint32_t now = millis();

        if( last_poll + poll_delay > now ) return;
        last_poll = now;

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
        // bool has_bubble = btnA || btnB || btnC;
        // if( has_bubble ) {
        //   log_d("Button has bubble: A:%d, B:%d, C:%d", btnA, btnB, btnC );
        // }
        return btnA || btnB || btnC;
      }


      void cancelBubble()
      {
        btnA = btnB = btnC = false;
      }

    };


  };

};
