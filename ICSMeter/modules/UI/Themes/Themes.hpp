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

      const RGBColor TFT_MODE_BORDER   = {115, 135, 159};
      const RGBColor TFT_MODE_BACK     = {24,  57,  92};

      uint16_t bgcolor   = 0; // bgcolor in use
      uint16_t fgcolor  = 0; // fgcolor in use

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
        const image_t *top;
        const image_t *middle;
        const image_t *bottom;
        const RGBColor bgcolor;
        const RGBColor fgcolor;
      };

    };

  };
};
