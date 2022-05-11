#pragma once

namespace ICSMeter
{

  namespace UI
  {

    namespace Measure
    {
      int8_t value = 1;
      int8_t valueOld = 5;

      // for settings menu
      const char *choices[] = {"PWR", "S", "SWR"};

      void draw(); // Print Measure
      void setValue(String valString, uint8_t x = 160, uint8_t y = 180); // Print value
      void subValue(String valString, uint8_t x = 160, uint8_t y = 206); // Print sub value
    };

  };

};
