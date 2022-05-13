#include "Themes.hpp"


namespace ICSMeter
{

  namespace UI
  {

    namespace Theme
    {

      const image_t middle10Classic   = { 16, smeterMiddleClassic10 , sizeof(smeterMiddleClassic10  ), IMAGE_JPG, 320, 140 };
      const image_t middle100Classic  = { 16, smeterMiddleClassic100, sizeof(smeterMiddleClassic100 ), IMAGE_JPG, 320, 140 };
      const image_t topClassic        = { 16, smeterTopClassic      , sizeof(smeterTopClassic       ), IMAGE_JPG, 320, 20  };
      const image_t bottomClassic     = { 16, smeterBottomClassic   , sizeof(smeterBottomClassic    ), IMAGE_JPG, 320, 80  };
      const image_t *middleClassicPtr = (IC_MODEL == 705) ? &middle10Classic : &middle100Classic;

      const image_t middle10Dark      = { 16, smeterMiddleDark10 ,    sizeof(smeterMiddleDark10     ), IMAGE_JPG, 320, 140 };
      const image_t middle100Dark     = { 16, smeterMiddleDark100,    sizeof(smeterMiddleDark100    ), IMAGE_JPG, 320, 140 };
      const image_t topDark           = { 16, smeterTopDark         , sizeof(smeterTopDark          ), IMAGE_JPG, 320, 20  };
      const image_t bottomDark        = { 16, smeterBottomDark      , sizeof(smeterBottomDark       ), IMAGE_JPG, 320, 80  };
      const image_t *middleDarkPtr    = (IC_MODEL == 705) ? &middle10Dark : &middle100Dark;

      const RGBColor TFT_BACK_CLASSIC  = {255, 248, 236};
      const RGBColor TFT_FRONT_CLASSIC = {0,   0,   0};

      const RGBColor TFT_BACK_DARK     = {0,   0,   0};
      const RGBColor TFT_FRONT_DARK    = {255, 255, 255};

      const layout_t classic = { &topClassic, middleClassicPtr, &bottomClassic, TFT_BACK_CLASSIC, TFT_FRONT_CLASSIC };
      const layout_t dark    = { &topDark,    middleDarkPtr,    &bottomDark,    TFT_BACK_DARK,    TFT_FRONT_DARK    };

      const int8_t THEMES_COUNT = 2;
      const char     *choices[THEMES_COUNT] = {"CLASSIC", "DARK"}; // labels for settings menu
      const layout_t *layouts[THEMES_COUNT] = { &classic, &dark }; // pointers to layouts
      const layout_t *layout = layouts[theme]; // current layout in use

      const uint16_t TFT_MENU_BORDER   = tft.color565(115, 135, 159);
      const uint16_t TFT_MENU_BACK     = tft.color565(24,  57,  92);
      const uint16_t TFT_MENU_SELECT   = tft.color565(255, 255, 255);

      // Needle
      const RGBColor TFT_NEDDLE_1      = {241, 120, 100};
      const RGBColor TFT_NEDDLE_2      = {241, 140, 120};

      void set()
      {
        layout = layouts[theme];
      }

      void resetColor()
      {
        bgcolor  = tft.color565(layout->bgcolor.r, layout->bgcolor.g,  layout->bgcolor.b);
        fgcolor = tft.color565(layout->fgcolor.r, layout->fgcolor.g, layout->fgcolor.b);
      }

    };

  };

};

