#line 1 "/home/runner/work/ICSMeter/ICSMeter/include/UI/UI.cpp"
#include "UI.hpp"


namespace ICSMeter
{

  namespace UI
  {

    using namespace Theme;


    void setup()
    {
      UI::draw( true );
    }


    void loop()
    {
      ScreenSaver::loop(); // check if Screen Saver needs enabling
      if( ScreenSaver::isEnabled() ) return;
      if( Settings::dialog_enabled == false ) UI::draw();
      Settings::loop();
    }


    void draw( bool force_redraw )
    {
      if( force_redraw ) {
        utils::drawImage( &tft, layout->topImage, 0, 0, layout->topImage->width, layout->topImage->height);
        utils::drawImage( &tft, layout->middleImage, 0, 20, layout->middleImage->width, layout->middleImage->height);
        utils::drawImage( &tft, layout->bottomImage, 0, 160, layout->bottomImage->width, layout->bottomImage->height);
        log_d("Forced redraw");
      }
      Battery::draw( force_redraw );
      Transverter::draw( force_redraw );
      Network::draw( force_redraw );
      Needle::draw( force_redraw );
      Measure::drawLabels( force_redraw );
      DataMode::draw( force_redraw );
      Measure::drawValues( force_redraw );
      Settings::draw( force_redraw );
    }


  };
};

