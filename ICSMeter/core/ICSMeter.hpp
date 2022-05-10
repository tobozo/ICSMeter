#pragma once

#include "../main/core.h"

// static assets
#include "../assets/font.hpp"
#include "../assets/images.hpp"
#include "../assets/index_html.hpp"

// additional modules
#include "../modules/FastLed.hpp"




namespace ICSMeter
{

  Preferences preferences;

  int8_t measure = 1;
  int8_t beep = 0;
  int8_t transverter = 0;
  int8_t screensaver = 0;
  int8_t theme = 0;
  uint8_t brightness = 64;

  boolean screenshot = false;
  boolean settingsMode = false;
  boolean settingLock = true;

  // Setup
  void setup();

  // Main loop
  void loop();


};
