#line 1 "/home/runner/work/ICSMeter/ICSMeter/include/core/modules/BackLight.cpp"
#include "../ICSMeter.hpp"

namespace ICSMeter
{

  namespace modules
  {

    namespace BackLight
    {

      constexpr const char *label = "BRIGHTNESS";

      uint8_t brightness    = 0x40; // [0...100]
      uint8_t brightnessOld = 0x40; // [0...100]

      const uint8_t brightness_default = 0x40;
      const uint8_t brightness_min     = 0x01; // [0...100]
      const uint8_t brightness_max     = 0x64; // [0...100]


      void setup()
      {
        brightness = prefs::get("brightness", brightness_default );
        setBrightness( brightness );
      }


      void save()
      {
        uint8_t tmp = prefs::get("brightness", brightness_default );
        if( tmp != brightness ) {
          prefs::set("brightness", brightness);
          brightnessOld = tmp;
        }
      }


      uint8_t getBrightness()
      {
        return brightness;
      }

      void setBrightness( uint8_t value )
      {
        tft.setBrightness( map( value, brightness_min, brightness_max, 0x00, 0xff) );
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
