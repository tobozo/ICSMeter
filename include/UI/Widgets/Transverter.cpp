


namespace ICSMeter
{

  namespace UI
  {

    namespace Transverter
    {
      using namespace CSS;

      int8_t value = 0, valueOld = 0;


      const TextStyle_t labelTextStyle =
      {
        .fgColor   = 0xffffffU,
        .bgColor   = 0x000000U,
        .size      = 1,
        .datum     = MC_DATUM,
        .paddingX  = 0
      };
      const FontStyle_t labelFontStyle     = { &Font0,  &labelTextStyle, TRANSPARENT };


      void setup()
      {
        value = getPref("transverter", 0);
      }


      void save()
      {
        if( value != valueOld ) {
          setPref("transverter", Transverter::value);
          valueOld = value;
        }
      }


      void set( int8_t _value )
      {
        value = _value;
      }


      int8_t get()
      {
        return value;
      }


      void draw( bool force_redraw )
      {
        if (ScreenSaver::mode || Settings::mode ) return;
        if ( force_redraw || value > 0) {
          char label[16];
          snprintf(label, 15, "LO%d", value);
          CSS::drawStyledBox( &tft, label, 62, 4, 26, 13, &Theme::BadgeBoxStyle );
        }
      }

    };

  };

};
