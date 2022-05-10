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

    // LED
    CRGB leds[NUM_LEDS];
    CRGB strip[NUM_LEDS_STRIP];

  };
};
