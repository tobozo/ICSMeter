#include "../ICSMeter.hpp"
#include "../../UI/Themes/Themes.hpp"
#include "../../UI/Widgets.hpp"
#include "../../net/Daemon.hpp"

namespace ICSMeter
{

  namespace prefs
  {

    Preferences preferences;

    void load()
    {
      using namespace UI;
      using namespace modules;
      using namespace net;

      preferences.begin(NAME);

      // ok donc 4 choix d'IC:
      // IC7300 (WiFi)
      // IC9700 (WiFi)
      // IC705 (WiFi)
      // IC705 (Bluetooth)
      // et un choix +/- pour la CI-V address


      CIV::setup();

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


    size_t setString(const char* key, const char* value)
    {
      preferences.begin(NAME);
      size_t ret = preferences.putString( key,  value);
      preferences.end();
      return ret;
    }


    size_t getString(const char* key, char* value, size_t maxLen)
    {
      preferences.begin(NAME);

      size_t ret = preferences.getString( key, value, maxLen );

      preferences.end();

      return ret;
    }



  };

};
