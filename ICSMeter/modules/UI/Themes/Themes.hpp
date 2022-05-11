#pragma once

// static assets
#include "../../assets/font.hpp"
#include "../../assets/images.hpp"
#include "../../assets/index_html.hpp"

namespace ICSMeter
{

  namespace UI
  {


    namespace Theme
    {

      int8_t theme = 0;
      int8_t themeOld = 0;

      void set();
      void resetColor();

      // imported from LGFXMeter, remove that later
      enum image_type_t
      {
        IMAGE_PNG,
        IMAGE_JPG,
        IMAGE_BMP,
        IMAGE_QOI,
        IMAGE_RAW
      };

      // imported from LGFXMeter, remove that later
      struct image_t
      {
        uint8_t       bit_depth; // Note: this is NOT the image bit depth but the minimal *canvas* recommended bit depth (e.g. 24bit image can be rendered as 16bit)
        const uint8_t *data;
        uint32_t      len;
        image_type_t  type;
        int32_t       width;
        int32_t       height;
      };

      struct layout_t
      {
        image_t *top;
        image_t *middle;
        image_t *bottom;
      };

      image_t middle10Classic   = { 16, smeterMiddleClassic10 , sizeof(smeterMiddleClassic10  ), IMAGE_JPG, 320, 140 };
      image_t middle100Classic  = { 16, smeterMiddleClassic100, sizeof(smeterMiddleClassic100 ), IMAGE_JPG, 320, 140 };
      image_t topClassic        = { 16, smeterTopClassic      , sizeof(smeterTopClassic       ), IMAGE_JPG, 320, 20  };
      image_t bottomClassic     = { 16, smeterBottomClassic   , sizeof(smeterBottomClassic    ), IMAGE_JPG, 320, 80  };
      image_t *middleClassicPtr = (IC_MODEL == 705) ? &middle10Classic : &middle100Classic;

      image_t middle10Dark      = { 16, smeterMiddleDark10 ,    sizeof(smeterMiddleDark10     ), IMAGE_JPG, 320, 140 };
      image_t middle100Dark     = { 16, smeterMiddleDark100,    sizeof(smeterMiddleDark100    ), IMAGE_JPG, 320, 140 };
      image_t topDark           = { 16, smeterTopDark         , sizeof(smeterTopDark          ), IMAGE_JPG, 320, 20  };
      image_t bottomDark        = { 16, smeterBottomDark      , sizeof(smeterBottomDark       ), IMAGE_JPG, 320, 80  };
      image_t *middleDarkPtr    = (IC_MODEL == 705) ? &middle10Dark : &middle100Dark;


      layout_t classic = {
        &topClassic,
        middleClassicPtr,
        &bottomClassic
      };

      layout_t dark = {
        &topDark,
        middleDarkPtr,
        &bottomDark
      };


      const int8_t THEMES_COUNT = 2;
      const char *choices[THEMES_COUNT] = {"CLASSIC", "DARK"}; // settings menu
      layout_t *layouts[THEMES_COUNT]   = { &classic, &dark };

      layout_t *layout = layouts[theme]; // current layout in use


      uint16_t TFT_BACK   = 0;
      uint16_t TFT_FRONT  = 0;

      const RGBColor TFT_BACK_CLASSIC  = {255, 248, 236};
      const RGBColor TFT_FRONT_CLASSIC = {0,   0,   0};

      const RGBColor TFT_BACK_DARK     = {0,   0,   0};
      const RGBColor TFT_FRONT_DARK    = {255, 255, 255};

      const RGBColor TFT_MODE_BORDER   = {115, 135, 159};
      const RGBColor TFT_MODE_BACK     = {24,  57,  92};

      const uint16_t TFT_MENU_BORDER   = tft.color565(115, 135, 159);
      const uint16_t TFT_MENU_BACK     = tft.color565(24,  57,  92);
      const uint16_t TFT_MENU_SELECT   = tft.color565(255, 255, 255);

      // Needle
      const RGBColor TFT_NEDDLE_1      = {241, 120, 100};
      const RGBColor TFT_NEDDLE_2      = {241, 140, 120};


    };

  };
};
