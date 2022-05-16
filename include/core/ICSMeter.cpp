#include "ICSMeter.hpp"

#include "../UI/Helpers/CSS.cpp"
#include "../misc/FastLed.hpp"
#include "../FS/Updater.cpp"
#include "../net/Controls.cpp"
#if IC_CONNECT==BT && IC_MODEL==705
  #include "../net/Bluetooth.cpp"
#endif
#include "../net/WiFi.cpp"
#include "../UI/Themes/Themes.cpp"
#include "../UI/UI.cpp"
#include "../UI/Controls.cpp"
#include "../UI/Widgets.hpp"
#include "../UI/Widgets/Battery.cpp"
#include "../UI/Widgets/DataMode.cpp"
#include "../UI/Widgets/Measure.cpp"
#include "../UI/Widgets/Needle.cpp"
#include "../UI/Widgets/ScreenSaver.cpp"
#include "../UI/Widgets/Settings.cpp"
#include "../UI/Widgets/Transverter.cpp"


namespace ICSMeter
{

  using namespace modules;
  using namespace UI;


  void setup() // Main Setup
  {
    auto cfg = M5.config();
    M5.begin(cfg); // Init M5

    loadPrefs();

    FSUpdater::binLoader();

    UI::setup();
    UI::draw();
    UI::drawWidgets( true );

    net::setup();

    xTaskCreatePinnedToCore( UI::buttonTask, "buttonTask", 8192, NULL, 4, NULL, 1);
  }


  void loop() // Main Loop
  {
    ScreenSaver::handle(); // check if Screen Saver needs enabling

    if(net::connected()) {
      uint8_t tx = getTX(); // check connection health
      if(tx != 0) ScreenSaver::reset(); // If transmit, refresh tempo

      if ( UI::canDrawUI() && screenshot::capture == false  ) {
        Settings::lock = true;

        ICScan();
        FastLed::set( tx );
        UI::drawWidgets();

        Settings::lock = false;
      }
    }

    #if DEBUG==1
      Serial.print(ScreenSaver::mode);
      Serial.print(" ");
      Serial.print(millis() - ScreenSaver::timer);
      Serial.print(" ");
      Serial.println(ScreenSaver::countdown * 60 * 1000);
    #endif
  }


  void checkButtons()
  {
    const uint32_t buttons_poll_delay = 30; // min delay (milliseconds) between each button poll
    static uint32_t last_button_poll = millis();
    uint32_t now = millis();

    if( last_button_poll + buttons_poll_delay > now ) return;
    last_button_poll = now;

    M5.update();

    btnA = M5.BtnA.isPressed();
    btnB = M5.BtnB.isPressed();
    btnC = M5.BtnC.isPressed();
  }


  void loadPrefs()
  {
    preferences.begin(NAME);

    BackLight::setup();
    FastLed::setup();
    Beeper::setup();
    ScreenSaver::setup();
    Theme::setup();
    Measure::setup();
    Transverter::setup();

    preferences.end();
  }


  unsigned int getPref( const char* name, unsigned int default_value )
  {
    preferences.begin(NAME);

    unsigned int ret = preferences.getUInt( name, default_value );

    preferences.end();

    return ret;
  }


  void setPref( const char* name, unsigned int value )
  {
    preferences.begin(NAME);

    preferences.putUInt( name, value );

    preferences.end();
  }





};
