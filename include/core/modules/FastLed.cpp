#include "../ICSMeter.hpp"

namespace ICSMeter
{

  namespace modules
  {

    namespace FastLed
    {

      CRGB leds[NUM_LEDS];
      CRGB strip[NUM_LEDS_STRIP];

      // LED
      void setup()
      {
        // Init Led
        if(M5.getBoard() == m5::board_t::board_M5Stack) {
          FastLED.addLeds<NEOPIXEL, 15>(leds, NUM_LEDS);  // GRB ordering is assumed
        } else if(M5.getBoard() == m5::board_t::board_M5StackCore2) {
          FastLED.addLeds<NEOPIXEL, 25>(leds, NUM_LEDS);  // GRB ordering is assumed
        }
        // If led strip...
        /*
        FastLED.addLeds<WS2811,Neopixel_PIN,GRB>(strip, NUM_LEDS_STRIP).setCorrection(TypicalLEDStrip);
        FastLED.setBrightness(32);
        */
      }


      void set( uint8_t tx )
      {
        if(tx == 0) {
          for(uint8_t i = 0; i <= 9; i++){
            leds[i] = CRGB::Black;
          }
        } else {
          for(uint8_t i = 0; i <= 9; i++){
            leds[i] = CRGB::Red;
          }
        }
        FastLED.setBrightness(16);
        FastLED.show();
      }

    };

  };

};
