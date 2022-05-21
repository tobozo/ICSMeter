#include "ICSMeter.hpp"

#include "modules/BackLight.cpp"
#include "modules/Beeper.cpp"
#include "modules/Buttons.cpp"
#include "modules/FastLed.cpp"
#include "modules/Prefs.cpp"
#include "modules/Updater.cpp"

#if IC_CONNECT==BT && IC_MODEL==705
  #include "../net/protocols/Bluetooth.cpp"
#endif
#include "../net/protocols/WiFi.cpp"
#include "../net/protocols/CI-V.cpp"
#include "../net/services/Screenshots.cpp"
#include "../net/services/Proxy.cpp"
#include "../net/Controls.cpp"

#include "../UI/Helpers/CSS.cpp"
#include "../UI/Themes/Themes.cpp"
#include "../UI/Widgets/Battery.cpp"
#include "../UI/Widgets/DataMode.cpp"
#include "../UI/Widgets/Measure.cpp"
#include "../UI/Widgets/Needle.cpp"
#include "../UI/Widgets/ScreenSaver.cpp"
#include "../UI/Widgets/Settings.cpp"
#include "../UI/Widgets/Transverter.cpp"
#include "../UI/UI.cpp"


namespace ICSMeter
{

  using namespace modules;
  using namespace UI;
  using namespace net;


  void setup() // Main Setup
  {
    auto cfg = M5.config();
    M5.begin(cfg); // Init M5

    LcdMux = false;

    prefs::load();

    FSUpdater::binLoader();

    UI::setup();
    UI::drawWidgets( true );

    xTaskCreatePinnedToCore( daemon::netTask, "netTask", 8192, NULL, 16, NULL, 0);

  }


  void loop() // Main Loop
  {
    buttons::loop();   // handle buttons
    if( buttons::hasBubble() ) {
      ScreenSaver::resetTimer();
    }

    Beeper::loop();    // handle beep

    takeLcdMux(); // this will wait for current screen capture to finish
                  // and prevent further screen capture to be performed while drawing
                  // until giveLcdMux() is called

    ScreenSaver::loop(); // check if Screen Saver needs enabling
    if( ScreenSaver::isEnabled() ) goto _giveMux;

    Settings::loop();
    if( Settings::dialog_enabled ) goto _giveMux;

    #if defined DEMO_MODE
      /********** Needle DEMO MODE BEGIN ******/
      DataMode::setFilter( "FIL1" );
      DataMode::setMode( "USB" );
      Measure::setPrimaryValue( "S 9.06 db");
      Measure::setSecondaryValue( "14.235.000" );

      static float random_angle = 0.0;
      Needle::ICSGauge->easeNeedle( 300 ); // 300ms easing
      if( (millis()/1259)%2== 0 ) {
        random_angle = ( (rand()%9000) / 100.0 ); // [0...90] with 2 digits decimal precision
        Needle::ICSGauge->setNeedle( random_angle );
        UI::drawWidgets();
      }
      /********** Needle DEMO MODE END ******/
       goto _giveMux;
    #endif

    daemon::loop();


    _giveMux:
    giveLcdMux();

  }


  void checkButtons()
  {

  }


  void shutdown()
  {
    M5.Power.powerOff();
  }


  void takeLcdMux()
  {
    do vTaskDelay(1);
    while( LcdMux );
    LcdMux = true;
  }


  void giveLcdMux()
  {
    LcdMux = false;
    vTaskDelay(1);
  }



};
