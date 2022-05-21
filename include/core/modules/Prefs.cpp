#include "../ICSMeter.hpp"
#include "../../UI/Themes/Themes.hpp"
#include "../../UI/Widgets.hpp"

namespace ICSMeter
{

  namespace prefs
  {

    Preferences preferences;

    void load()
    {
      using namespace UI;
      using namespace modules;

      preferences.begin(NAME);

      BackLight::setup();
      FastLed::setup();
      Beeper::setup();
      ScreenSaver::setup();
      Theme::setup();
      Needle::setup();
      Measure::setup();
      Transverter::setup();

      preferences.end();
    }


    unsigned int get( const char* name, unsigned int default_value )
    {
      preferences.begin(NAME);

      unsigned int ret = preferences.getUInt( name, default_value );

      preferences.end();

      return ret;
    }


    void set( const char* name, unsigned int value )
    {
      preferences.begin(NAME);

      preferences.putUInt( name, value );

      preferences.end();
    }

  };

};
