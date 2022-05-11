#pragma once

#include "../../main/core.h"

namespace ICSMeter
{

  Preferences preferences;

  M5GFX &tft(M5.Lcd); // provide a shorthand to "M5.Lcd"

  // Setup
  void setup();

  // Main loop
  void loop();

  void checkButtons();
  int btnA, btnB, btnC, btnL, btnM, btnR;

  void loadPrefs();


};
