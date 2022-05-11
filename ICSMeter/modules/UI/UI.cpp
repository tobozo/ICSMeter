#include "UI.hpp"

#include "Controls.hpp"
#include "Widgets.hpp"


namespace ICSMeter
{

  namespace UI
  {


    using namespace Theme;
    using namespace net;
    using namespace Utils;


    void setup()
    {
      setBrightness(map(brightness, 1, 100, 1, 254));
      tft.setRotation(1);
    }


    // imported from LGFXMeter, remove that later
    void drawImage( LovyanGFX* dst, const image_t *img, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0 )
    {
      float scale_x = 1.0f;
      float scale_y = 0.0f;

      switch( img->type ) {
        case IMAGE_PNG: dst->drawPng( img->data, img->len, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y ); break;
        case IMAGE_JPG: dst->drawJpg( img->data, img->len, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y ); break;
        case IMAGE_BMP: dst->drawBmp( img->data, img->len, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y ); break;
        case IMAGE_QOI: dst->drawQoi( img->data, img->len, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y ); break;
        case IMAGE_RAW: dst->pushImage( -offX, -offY, maxWidth, maxHeight, img->data, img->len );   break;
      }
    }


    void drawTop()
    {
      drawImage( &tft, layout->top, 0, 0, 320, 20);
    }

    void drawMiddle()
    {
      drawImage( &tft, layout->middle, 0, 20, 320, 140);
    }

    void drawBottom()
    {
      drawImage( &tft, layout->bottom, 0, 160, 320, 80);
    }



    void draw()
    {
      Theme::resetColor();
      tft.fillScreen(TFT_BACK);
      drawTop();
      drawMiddle();
      drawBottom();
    }



  };
};

