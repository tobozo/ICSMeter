#line 1 "/home/runner/work/ICSMeter/ICSMeter/include/core/ICSMeter.cpp"
#include "ICSMeter.hpp"

#include "modules/BackLight.cpp"
#include "modules/Beeper.cpp"
#include "modules/Buttons.cpp"
#include "modules/FastLed.cpp"
#include "modules/Prefs.cpp"
#include "modules/Updater.cpp"

#include "../net/protocols/Bluetooth.cpp"
#include "../net/protocols/CI-V.cpp"
#include "../net/protocols/WiFi.cpp"
#include "../net/services/Proxy.cpp"
#include "../net/services/Screenshots.cpp"
#include "../net/services/WebClient.cpp"
#include "../net/services/WebServer.cpp"

#include "../net/Daemon.cpp"

#include "../UI/Helpers/CSS.cpp"
#include "../UI/Themes/Themes.cpp"
#include "../UI/Widgets/Battery.cpp"
#include "../UI/Widgets/DataMode.cpp"
#include "../UI/Widgets/Measure.cpp"
#include "../UI/Widgets/Needle.cpp"
#include "../UI/Widgets/Network.cpp"
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

    xTaskCreatePinnedToCore( daemon::daemonTask, "daemonTask", 8192, NULL, 1, NULL, 0);
  }


  void loop() // Main Loop
  {
    if( WebServer::OTA_enabled ) {
      ArduinoOTA.handle();
    }

    Beeper::loop();    // handle beep

    takeLcdMux(); // this will wait for current screen capture to finish
                  // and prevent further screen capture to be performed while drawing
                  // until giveLcdMux() is called

    buttons::loop();   // handle buttons
    if( buttons::hasBubble() ) {
      ScreenSaver::resetTimer();
    }

    UI::loop();

    giveLcdMux();

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
