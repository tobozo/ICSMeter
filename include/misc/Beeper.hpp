
#include "../core/ICSMeter.hpp"

namespace ICSMeter
{

  namespace modules
  {

    namespace Beeper
    {

      int8_t beepVolume    = 0;
      int8_t beepVolumeOld = 0;

      const int8_t max_volume = 100;
      const int8_t min_volume = 0;

      const uint32_t beepDuration = 50;
      uint32_t beepPause    = 150;

      uint32_t lastBeepBtnAC = millis();
      uint32_t lastBeepBtnB  = millis();

      constexpr const char *label = "BEEP LEVEL";


      void setup()
      {
        beepVolume = getPref("beep", 0);
        M5.Speaker.setVolume( beepVolume );
      }


      void save()
      {
        int8_t tmp = getPref("beep", 0);
        if( beepVolume != tmp ) {
          setPref("beep", beepVolume);
          beepVolumeOld = tmp;
        }
      }


      void increase()
      {
        beepVolume++;
        beepVolume = min( max_volume, beepVolume );
        M5.Speaker.setVolume( beepVolume ); // set master volume (0~255)
      }


      void decrease()
      {
        beepVolume--;
        beepVolume = max( min_volume, beepVolume );
        M5.Speaker.setVolume( beepVolume ); // set master volume (0~255)
      }


      void handle()
      {
        if(M5.Speaker.isEnabled() && beepVolume > 0 && ! M5.Speaker.isPlaying() ) {
          uint32_t now = millis();
          if(btnA || btnC) {
            if( lastBeepBtnAC + beepPause > now ) return;
            lastBeepBtnAC = now;
            M5.Speaker.tone(1000, 50); // play beep sound 1000Hz 100msec (background task)
            vTaskDelay(1);
            return;
          }
          if(btnB) {
            if( lastBeepBtnB + beepPause > now ) return;
            lastBeepBtnB = now;
            M5.Speaker.tone(2000, 50); // play beep sound 2000Hz 100msec (background task)
            vTaskDelay(1);
          }
        }
      }

    };

  };

};
