#pragma once


namespace ICSMeter
{

  namespace UI
  {

    namespace Needle
    {
      float angleOld;

      void draw(float_t angle, uint16_t a = 0, uint16_t b = 200, uint16_t c = 0, uint16_t d = 100); // Print needle
    };

  };

};
