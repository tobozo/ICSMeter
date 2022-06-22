#line 1 "/home/runner/work/ICSMeter/ICSMeter/include/UI/Widgets/Transverter.cpp"
#include "../Widgets.hpp"

namespace ICSMeter
{

  namespace UI
  {

    namespace Transverter
    {

      int8_t value = 0, valueOld = 0;

      constexpr clipRect_t ModeClip =
      {
        .x = 62,
        .y = 4,
        .w = 26,
        .h = 13
      };


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
          CSS::drawStyledBox( &tft, label, ModeClip.x, ModeClip.y, ModeClip.w, ModeClip.h, &Theme::BadgeBoxStyle );
        }
      }

    };

  };

};
