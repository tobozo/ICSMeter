
namespace ICSMeter
{

  namespace UI
  {

    namespace Measure
    {

      using namespace net;

      MeasureModes_t oldMode = MODE_SWR;
      String oldUnitValue;
      String unitValue;
      constexpr const char *choices[] = {"PWR", "S", "SWR"};


      constexpr clipRect_t LabelClip =
      {
        .x = 65,
        .y = 230,
        .w = 95,
        .h = 0
      };


      const CSS::TextStyle_t unitModeTextStyle =
      {
        .fgColor   = 0xffffffU,
        .bgColor   = 0x000000U,
        .size      = 1,
        .datum     = MC_DATUM,
        .paddingX  = 0
      };



      const CSS::TextStyle_t unitLabelTextStyle =
      {
        .fgColor   = 0xffffffU,
        .bgColor   = 0x000000U,
        .size      = 1,
        .datum     = MC_DATUM,
        .paddingX  = 160
      };


      const CSS::FontStyle_t unitModeFontStyle  = { &YELLOWCRE8pt7b,  &unitModeTextStyle,  CSS::OPAQUE };
      const CSS::FontStyle_t unitLabelFontStyle = { &YELLOWCRE8pt7b,  &unitLabelTextStyle, CSS::OPAQUE };



      void setup()
      {
        mode = (MeasureModes_t)prefs::get("measure", 1);
        CIV::setMeterSubscription( mode );
      }


      void save()
      {
        MeasureModes_t tmp = (MeasureModes_t)prefs::get("measure", 1);
        if( mode != tmp ) {
          prefs::set("measure", mode);
          oldMode = tmp;
        }
      }


      void onModeChange()
      {
        if( WebClient::has_subscriptions ) {
          WebClient::renewSubscriptions();
        }
      }


      void setUnitValue( String value )
      {
        unitValue = value;
      }


      void updateValues()
      {
        unitValue = CIV::getFrequencyLabel();
      }


      bool needs_redraw()
      {
        updateValues();
        return ( unitValue!= oldUnitValue );
      }


      void drawValues( bool force_redraw )
      {
        if( !needs_redraw() && !force_redraw ) return;
        if( force_redraw || needs_redraw() ) {
          drawUnitValue( unitValue );
          oldUnitValue = unitValue;
        }
      }


      void drawLabels( bool force_redraw )
      {
        uint16_t x = LabelClip.x;//65, y = 230;

        if( force_redraw || mode != oldMode ) {

          oldMode = mode;

          setFontStyle( &tft, &unitModeFontStyle );

          for (uint8_t i = 0; i < 3; i++) {
            if (mode == i) {
              tft.setTextColor(Theme::layout->fgcolor);
            } else {
              tft.setTextColor(TFT_DARKGREY);
            }

            CSS::drawStyledString( &tft, choices[i], x, LabelClip.y, nullptr );
            x += LabelClip.w;
          }
        }
      }


      void drawUnitValue(String _freq, uint8_t x, uint8_t y)
      {
        setFontStyle( &tft, &unitLabelFontStyle );
        tft.setTextColor(Theme::layout->fgcolor, Theme::layout->bgcolor);
        if( _freq != "" )
          CSS::drawStyledString( &tft, _freq.c_str(), x, y, nullptr );
      }


    };

  };

};

