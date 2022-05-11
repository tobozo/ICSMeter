#pragma once

#include "../../../settings.h"


namespace ICSMeter
{

  namespace UI
  {

    namespace Transverter
    {
      int8_t value = 0;
      int8_t valueOld = 0;

      const double choices[] = {
        0,
        TRANSVERTER_LO_1,
        TRANSVERTER_LO_2,
        TRANSVERTER_LO_3,
        TRANSVERTER_LO_4,
        TRANSVERTER_LO_5
      };

      void draw();
    };

  };
};
