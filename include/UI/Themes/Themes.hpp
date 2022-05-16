#pragma once

// static assets
#include "../../assets/font.hpp"
#include "../../assets/images.hpp"
#include "../../assets/index_html.hpp"
#include "../Helpers/CSS.hpp"

namespace ICSMeter
{

  namespace UI
  {

    namespace Theme
    {

      using namespace CSS;

      constexpr const int8_t THEMES_COUNT = 2;

      const uint32_t TFT_MODE_BORDER   = 0x73879fU;
      const uint32_t TFT_MODE_BACK     = 0x18395cU;

      const uint32_t TFT_BACK_CLASSIC  = 0xfff8ecU;
      const uint32_t TFT_FRONT_CLASSIC = 0x000000U;

      const uint32_t TFT_BACK_DARK     = 0x000000U;
      const uint32_t TFT_FRONT_DARK    = 0xffffffU;

      const uint32_t TFT_MENU_BORDER   = 0x73879fU;
      const uint32_t TFT_MENU_BACK     = 0x18395cU;
      const uint32_t TFT_MENU_SELECT   = 0xffffffU;

      void set();

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
        const image_t *topImage;
        const image_t *middleImage;
        const image_t *bottomImage;
        const uint32_t fgcolor;
        const uint32_t bgcolor;
      };

      const layout_t    *layout;      // themed layout in use
      const FontStyle_t *H3FontStyle; // themed shared font style for H3
      extern const TextBoxStyle_t BadgeBoxStyle; // unthemed shared box style

      extern const FontStyle_t *H3FontStyleTheme[THEMES_COUNT];
      extern const layout_t    *layouts[THEMES_COUNT];

    };

  };
};
