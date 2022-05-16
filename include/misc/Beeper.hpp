
#include "../core/ICSMeter.hpp"

namespace ICSMeter
{

  namespace modules
  {

    namespace Beeper
    {

      int8_t beepVolume    = 0;
      int8_t beepVolumeOld = 0;

      int8_t max_volume = 100;
      int8_t min_volume = 0;

      constexpr const char *label        = "BEEP LEVEL";


      void setup()
      {
        beepVolume = getPref("beep", 0);
        M5.Speaker.setVolume( beepVolume );
      }


      void save()
      {
        if( beepVolume != beepVolumeOld ) {
          setPref("beep", beepVolume);
          beepVolumeOld = beepVolume;
        }
      }


      void increase()
      {
        beepVolume++;
        beepVolume = min( max_volume, beepVolume );
      }


      void decrease()
      {
        beepVolume--;
        beepVolume = max( min_volume, beepVolume );
      }


      void handle()
      {
        if(M5.Speaker.isEnabled() && beepVolume > 0 && ! M5.Speaker.isPlaying() ) {
          M5.Speaker.setVolume( beepVolume ); // set master volume (0~255)
          if(btnA || btnC) {
            M5.Speaker.tone(1000, 50); // play beep sound 1000Hz 100msec (background task)
          } else if(btnB) {
            M5.Speaker.tone(2000, 50); // play beep sound 2000Hz 100msec (background task)
          }
        }
      }

    };

  };

};
