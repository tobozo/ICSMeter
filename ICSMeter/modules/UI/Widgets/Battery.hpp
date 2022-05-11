#pragma once

namespace ICSMeter
{

  namespace UI
  {

    namespace Battery
    {
      uint8_t levelOld = 0;
      bool charginglOld = true;

      int8_t getLevel(bool type);
      bool isCharging();
      // Print battery
      void draw();
    };

  };

};
