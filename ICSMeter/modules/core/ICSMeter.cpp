#include "ICSMeter.hpp"

#include "../misc/FastLed.hpp"
#include "../FS/Updater.cpp"
#include "../net/Controls.cpp"
#include "../net/Bluetooth.cpp"
#include "../net/WiFi.cpp"
#include "../UI/Themes/Themes.cpp"
#include "../UI/UI.cpp"
#include "../UI/Controls.cpp"
#include "../UI/Widgets.cpp"


namespace ICSMeter
{

  using namespace modules;
  using namespace fs;
  using namespace net;
  using namespace UI;
  using namespace UI::Measure;
  using namespace UI::Battery;
  using namespace UI::ScreenSaver;

  // Main Setup
  void setup()
  {

    // Init M5
    auto cfg = M5.config();
    M5.begin(cfg);

    loadPrefs();

    // TODO: if prefs::use_fastled
    modules::setupFastLed();

    binLoader();

    UI::setup();
    UI::draw();

    setupNetwork();

    xTaskCreatePinnedToCore( buttonTask, "buttonTask", 8192, NULL, 4, NULL, 1);
  }


  // Main Loop
  void loop()
  {
    static uint8_t alternance = 0;
    static uint8_t tx = 0;

    if(checkConnection()) {
      tx = getTX();
      if(tx != 0) ScreenSaver::timer = millis();   // If transmit, refresh tempo

      if (ScreenSaver::mode == false && screenshot == false && Settings::mode == false) {
        Settings::lock = true;

        if(tx == 0) {
          for(uint8_t i = 0; i <= 9; i++){
            leds[i] = CRGB::Black;
          }
          FastLED.setBrightness(16);
          FastLED.show();
        } else {
          for(uint8_t i = 0; i <= 9; i++){
            leds[i] = CRGB::Red;
          }
          FastLED.setBrightness(16);
          FastLED.show();
        }

        Measure::draw();
        Battery::draw();
        Transverter::draw();

        getMode();
        getFrequency();

        switch (Measure::value) {
        case 0:
          getPower();
          break;
        case 1:
          getSmeter();
          break;
        case 2:
          getSWR();
          break;
        }
        Settings::lock = false;
      }
    }
    alternance = (alternance++ < 2) ? alternance : 0;

    // check if Screen Saver needs enabling
    ScreenSaver::handle();

    if(DEBUG) {
      Serial.print(ScreenSaver::mode);
      Serial.print(" ");
      Serial.print(millis() - ScreenSaver::timer);
      Serial.print(" ");
      Serial.println(ScreenSaver::countdown * 60 * 1000);
    }
  }



  const uint32_t buttons_poll_delay = 30; // min delay (milliseconds) between each button poll
  uint32_t last_button_poll = millis();

  // Get button
  void checkButtons()
  {

    uint32_t now = millis();

    if( last_button_poll + buttons_poll_delay > now ) return;
    last_button_poll = now;

    M5.update();

    btnA = M5.BtnA.isPressed();
    btnB = M5.BtnB.isPressed();
    btnC = M5.BtnC.isPressed();

    btnL = M5.BtnA.pressedFor(2000);
    btnM = M5.BtnB.pressedFor(2000);
    btnR = M5.BtnC.pressedFor(2000);

    /*
    Serial.print(btnA);
    Serial.print(btnB);
    Serial.print(btnC);
    Serial.print(btnL);
    Serial.print(btnM);
    Serial.println(btnR);
    */
  }


  void loadPrefs()
  {
    // Preferences
    preferences.begin(NAME);
    Measure::value = preferences.getUInt("measure", 1);
    brightness = preferences.getUInt("brightness", 64);
    Transverter::value = preferences.getUInt("transverter", 0);
    beep = preferences.getUInt("beep", 0);
    ScreenSaver::countdown = preferences.getUInt("screensaver", 60);
    Theme::theme = preferences.getUInt("theme", 0);
    Theme::set();
  }


};
