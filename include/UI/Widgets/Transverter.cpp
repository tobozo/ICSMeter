#include "../Widgets.hpp"

namespace ICSMeter
{

  namespace UI
  {

    namespace Transverter
    {

      int8_t value = 0, valueOld = 0;

      const CSS::TextStyle_t labelTextStyle =
      {
        .fgColor   = 0xffffffU,
        .bgColor   = 0x000000U,
        .size      = 1,
        .datum     = MC_DATUM,
        .paddingX  = 0
      };
      const CSS::FontStyle_t labelFontStyle     = { &Font0,  &labelTextStyle, CSS::TRANSPARENT };


      void setup()
      {
        value = prefs::get("transverter", 0);
      }


      void save()
      {
        int8_t tmp = prefs::get("transverter", 0);
        if( value != tmp ) {
          prefs::set("transverter", Transverter::value);
          valueOld = tmp;
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
        if ( value > 0 && ( force_redraw || valueOld != value ) ) {
          valueOld = value;
          char label[16];
          snprintf(label, 15, "LO%d", value);
          CSS::drawStyledBox( &tft, label, 62, 4, 26, 13, &Theme::BadgeBoxStyle );
        }
      }

    };

  };

};
