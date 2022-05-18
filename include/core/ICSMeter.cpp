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

    LcdMux = false;

    loadPrefs();

    FSUpdater::binLoader();

    UI::setup();

    UI::drawWidgets( true );

    net::setup();

    #if defined DEMO_MODE
      DataMode::setFilter( "FIL1" );
      DataMode::setMode( "USB" );
      Measure::setPrimaryValue( "S 9.06 db");
      Measure::setSecondaryValue( "14.235.000" );
    #endif

    xTaskCreatePinnedToCore( UI::netTask, "netTask", 8192, NULL, 4, NULL, 1);

  }


  void loop() // Main Loop
  {
    checkButtons(); // check buttons status

    takeLcdMux(); // this will wait for current screen capture to finish
                  // and prevent further screen capture to be performed while drawing
                  // until giveLcdMux() is called

    ScreenSaver::handle(); // check if Screen Saver needs enabling
    if( ScreenSaver::enabled ) {
      giveLcdMux();
      return;
    }

    Settings::handle();
    if( Settings::dialog_enabled ) {
      giveLcdMux();
      return;
    }

    #if defined DEMO_MODE
      /********** Needle DEMO MODE BEGIN ******/
      static float random_angle = 0.0;
      Needle::ICSGauge->easeNeedle( 300 ); // 300ms easing
      if( (millis()/1259)%2== 0 ) {
        random_angle = ( (rand()%9000) / 100.0 );
        Needle::ICSGauge->setNeedle( random_angle );
        UI::drawWidgets();
      }
      /********** Needle DEMO MODE END ******/
      giveLcdMux();
      return;
    #endif


    if(net::connected()) {
      uint8_t tx = getTX(); // check connection health, retrieve last status
      if(tx != 0) ScreenSaver::reset(); // If transmit, refresh tempo
      ICScan();
      FastLed::set( tx );
      UI::drawWidgets();
    }

    giveLcdMux();

  }


  void checkButtons()
  {
    const uint32_t buttons_poll_delay = 0; // min delay (milliseconds) between each button poll
    static uint32_t last_button_poll = millis();
    uint32_t now = millis();

    if( last_button_poll + buttons_poll_delay > now ) return;
    last_button_poll = now;

    M5.update();

    btnA = M5.BtnA.isPressed();
    btnB = M5.BtnB.wasPressed(); // no repeat on this one
    btnC = M5.BtnC.isPressed();

    if( btnA || btnB || btnC ) ScreenSaver::reset();
    Beeper::handle();    // handle beep

  }


  void loadPrefs()
  {
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
