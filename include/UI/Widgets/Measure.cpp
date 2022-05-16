


namespace ICSMeter
{

  namespace UI
  {

    namespace Measure
    {

      using namespace Theme;
      using namespace CSS;

      int8_t oldValue = 5;
      String olPrimaryValue, oldSecondaryValue;
      String primaryValue, secondaryValue;
      constexpr const char *choices[] = {"PWR", "S", "SWR"};

      const TextStyle_t labelTextStyle =
      {
        .fgColor   = 0xffffffU,
        .bgColor   = 0x000000U,
        .size      = 1,
        .datum     = MC_DATUM,
        .paddingX  = 0
      };

      const TextStyle_t primaryTextStyle =
      {
        .fgColor   = 0xffffffU,
        .bgColor   = 0x000000U,
        .size      = 190,
        .datum     = MC_DATUM,
        .paddingX  = 160
      };

      const TextStyle_t secondaryTextStyle =
      {
        .fgColor   = 0xffffffU,
        .bgColor   = 0x000000U,
        .size      = 1,
        .datum     = MC_DATUM,
        .paddingX  = 190
      };


      const FontStyle_t labelFontStyle     = { &YELLOWCRE8pt7b,  &labelTextStyle    , OPAQUE };
      const FontStyle_t primaryFontStyle   = { &stencilie16pt7b, &primaryTextStyle  , OPAQUE };
      const FontStyle_t secondaryFontStyle = { &YELLOWCRE8pt7b,  &secondaryTextStyle, OPAQUE };


      void setup()
      {
        value = getPref("measure", 1);
      }


      void save()
      {
        if( value != oldValue ) {
          setPref("measure", value);
          oldValue = value;
        }
      }


      void reset() { oldValue = 5; }

      void setPrimaryValue( String value )   { primaryValue = value; }
      void setSecondaryValue( String value ) { secondaryValue = value; }

      bool primary_needs_redraw()   { return (primaryValue!=olPrimaryValue); }
      bool secondary_needs_redraw() { return (secondaryValue!=oldSecondaryValue); }
      bool needs_redraw()           { return (primary_needs_redraw() || secondary_needs_redraw()); }


      void drawValues( bool force_redraw )
      {
        if( force_redraw || primary_needs_redraw() ) {
          drawPrimaryValue( primaryValue );
          olPrimaryValue = primaryValue;
        }
        if( force_redraw || secondary_needs_redraw() ) {
          drawSecondaryValue( secondaryValue );
          oldSecondaryValue = secondaryValue;
        }
      }


      void drawLabels( bool force_redraw )
      {
        uint16_t i = 65;
        uint8_t j;

        if( force_redraw || value != oldValue ) {

          oldValue = value;

          setFontStyle( &tft, &labelFontStyle );

          for (j = 0; j <= 2; j++) {
            if (value == j) {
              tft.setTextColor(Theme::layout->fgcolor);
              net::reset = true;
            } else {
              tft.setTextColor(TFT_DARKGREY);
            }

            CSS::drawStyledString( &tft, choices[j], i, 230, nullptr );
            i += 95;
          }
        }
      }


      void drawPrimaryValue(String valString, uint8_t x, uint8_t y)
      {
        if (valString != olPrimaryValue)
        {
          olPrimaryValue = valString;

          setFontStyle( &tft, &primaryFontStyle );

          valString.replace(".", ",");

          CSS::drawStyledString( &tft, valString.c_str(), x, y, nullptr );
        }
      }


      void drawSecondaryValue(String valString, uint8_t x, uint8_t y)
      {
        if (valString != oldSecondaryValue)
        {
          oldSecondaryValue = valString;

          setFontStyle( &tft, &secondaryFontStyle );

          tft.setTextColor(Theme::layout->fgcolor, Theme::layout->bgcolor);

          CSS::drawStyledString( &tft, valString.c_str(), x, y, nullptr );
        }
      }


    };

  };

};

