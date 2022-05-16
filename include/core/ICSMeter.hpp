#pragma once

#include "../../main/core.h"

namespace ICSMeter
{

  Preferences preferences;

  M5GFX &tft(M5.Lcd); // provide a shorthand to "M5.Lcd"

  void setup(); // Main Setup

  void loop(); // Main loop

  void checkButtons();
  int btnA, btnB, btnC, btnL, btnM, btnR;

  void loadPrefs();
  unsigned int getPref( const char* name, unsigned int default_value );
  void setPref( const char* name, unsigned int value );


};
