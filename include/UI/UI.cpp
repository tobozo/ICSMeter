#include "UI.hpp"


namespace ICSMeter
{

  namespace UI
  {

    using namespace Theme;


    void setup()
    {
      tft.setRotation(1);
    }


    bool canDraw()
    {
       return (ScreenSaver::isEnabled() == false && Settings::dialog_enabled == false);
    }


    void drawWidgets( bool force_redraw )
    {
      if( force_redraw ) {
        log_d("Forced redraw");
      }
      if( force_redraw ) drawTop();

      Battery::draw( force_redraw );
      Transverter::draw( force_redraw );

      if( force_redraw ) drawMiddle();

      if( force_redraw || Needle::needs_redraw() ) {
        Needle::draw( force_redraw );
      }

      if( force_redraw )  drawBottom();

      Measure::drawLabels( force_redraw );
      if( force_redraw || DataMode::needs_redraw() ) {
        DataMode::draw( force_redraw );
      }
      if( force_redraw || Measure::needs_redraw() ) {
        Measure::drawValues( force_redraw );
      }

      if( force_redraw && Settings::dialog_enabled ) {
        Settings::draw();
      }
    }


    void drawTop()
    {
      utils::drawImage( &tft, layout->topImage, 0, 0, layout->topImage->width, layout->topImage->height);
    }


    void drawMiddle()
    {
      utils::drawImage( &tft, layout->middleImage, 0, 20, layout->middleImage->width, layout->middleImage->height);
    }


    void drawBottom()
    {
      utils::drawImage( &tft, layout->bottomImage, 0, 160, layout->bottomImage->width, layout->bottomImage->height);
    }


    void draw()
    {
      drawTop();
      drawMiddle();
      drawBottom();
    }


  };
};
