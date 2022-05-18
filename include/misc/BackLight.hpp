
#include "../core/ICSMeter.hpp"

namespace ICSMeter
{

  namespace modules
  {

    namespace BackLight
    {

      constexpr const char *label = "BRIGHTNESS";

      uint8_t brightness    = 64;
      uint8_t brightnessOld = 64;

      const uint8_t brightness_min = 0x01;
      const uint8_t brightness_max = 0x64;

      void setBrightness(uint8_t value);

      void setup()
      {
        brightness = getPref("brightness", 64);
        setBrightness( brightness );
      }


      void save()
      {
        uint8_t tmp = getPref("brightness", 64);
        if( tmp != brightness ) {
          setPref("brightness", brightness);
          brightnessOld = tmp;
        }
      }


      void setBrightness( uint8_t value )
      {
        tft.setBrightness( map(value, brightness_min, brightness_max, 0, 255) );
      }


      void increase()
      {
        BackLight::brightness++;
        BackLight::brightness = min( BackLight::brightness, BackLight::brightness_max );
        setBrightness( brightness );
      }


      void decrease()
      {
        BackLight::brightness--;
        BackLight::brightness = max( BackLight::brightness, BackLight::brightness_min );
        setBrightness( brightness );
      }


    };

  };

};
