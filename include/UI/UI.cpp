#include "UI.hpp"


namespace ICSMeter
{

  namespace UI
  {

    using namespace Theme;
    using namespace net;
    using namespace Utils;
    using namespace modules;


    void setup()
    {
      tft.setRotation(1);
    }


    // imported from LGFXMeter, remove that later
    void drawImage( LovyanGFX* dst, const image_t *img, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0 )
    {
      float scale_x = 1.0f;
      float scale_y = 0.0f;

      switch( img->type ) {
         case IMAGE_JPG: dst->drawJpg( img->data, img->len, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y ); break;
//         case IMAGE_PNG: dst->drawPng( img->data, img->len, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y ); break;
//         case IMAGE_BMP: dst->drawBmp( img->data, img->len, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y ); break;
//         case IMAGE_QOI: dst->drawQoi( img->data, img->len, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y ); break;
//         case IMAGE_RAW: dst->pushImage( -offX, -offY, maxWidth, maxHeight, img->data, img->len );   break;
      }
    }



    void drawWidgets( bool force_redraw )
    {

      if( force_redraw ) drawTop();
      Battery::draw( force_redraw );
      Transverter::draw( force_redraw );
      if( DataMode::needs_redraw() ) {
        DataMode::draw( force_redraw );
      }

      if( force_redraw ) drawMiddle();
      if( Needle::needs_redraw() ) {
        Needle::draw();
      }

      if( force_redraw )  drawBottom();
      Measure::drawLabels( force_redraw );
      if( Measure::needs_redraw() ) {
        Measure::drawValues( force_redraw );
      }

    }


    void drawTop()
    {
      drawImage( &tft, layout->topImage, 0, 0, layout->topImage->width, layout->topImage->height);
    }


    void drawMiddle()
    {
      drawImage( &tft, layout->middleImage, 0, 20, 320, 140);
    }


    void drawBottom()
    {
      drawImage( &tft, layout->bottomImage, 0, 160, 320, 80);
    }


    void draw()
    {
      drawTop();
      drawMiddle();
      drawBottom();
    }

    bool canDrawUI()
    {
       return (ScreenSaver::mode == false && Settings::mode == false);
    }



  };
};

