#pragma once

#include "../core/ICSMeter.hpp"

namespace ICSMeter
{

  const char *settings[] = {"Measured Values", "Transverter Mode", "Themes", "Brightness", "Beep", "Screensaver", "IP Address", "Shutdown", "Exit"};
  const char *choiceMeasures[] = {"PWR", "S", "SWR"};
  const char *choiceThemes[] = {"CLASSIC", "DARK"};
  const char *choiceBrightness[] = {"BRIGHTNESS"};
  const char *choiceBeep[] = {"BEEP LEVEL"};
  const char *choiceScreensaver[] = {"TIMEOUT"};
  const double choiceTransverter[] = {
      0,
      TRANSVERTER_LO_1,
      TRANSVERTER_LO_2,
      TRANSVERTER_LO_3,
      TRANSVERTER_LO_4,
      TRANSVERTER_LO_5
  };

  int8_t measureOld = 5;

  namespace UI
  {

    // Copyright (c) F4HWN Armel. All rights reserved.
    // Licensed under the MIT license. See LICENSE file in the project root for full license information.

    void viewMenu(uint8_t x, uint8_t y, uint16_t w, uint8_t h);

    void viewOption(int8_t settingsChoice, boolean settingsSelect, uint8_t x, uint8_t y, uint16_t w);

  };
};
