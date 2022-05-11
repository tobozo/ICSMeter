#include "Themes.hpp"


namespace ICSMeter
{

  namespace UI
  {

    namespace Theme
    {

      void set()
      {
        layout = layouts[theme];
      }


      // Reset color
      void resetColor()
      {
        switch (theme) {
          case 0:
            TFT_BACK  = tft.color565(TFT_BACK_CLASSIC.r,  TFT_BACK_CLASSIC.g,  TFT_BACK_CLASSIC.b);
            TFT_FRONT = tft.color565(TFT_FRONT_CLASSIC.r, TFT_FRONT_CLASSIC.g, TFT_FRONT_CLASSIC.b);
          break;

          case 1:
            TFT_BACK  = tft.color565(TFT_BACK_DARK.r,  TFT_BACK_DARK.g,  TFT_BACK_DARK.b);
            TFT_FRONT = tft.color565(TFT_FRONT_DARK.r, TFT_FRONT_DARK.g, TFT_FRONT_DARK.b);
          break;
        }
      }

    };

  };

};

