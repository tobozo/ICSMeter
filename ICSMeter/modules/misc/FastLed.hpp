#pragma once

#define FASTLED_INTERNAL // To disable pragma messages on compile
#define NUM_LEDS 10
#define Neopixel_PIN 32 // 21
#define NUM_LEDS_STRIP 30

#include <FastLED.h>


namespace ICSMeter
{

  namespace modules
  {

    CRGB leds[NUM_LEDS];
    CRGB strip[NUM_LEDS_STRIP];

    // LED
    void setupFastLed()
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


  };
};
