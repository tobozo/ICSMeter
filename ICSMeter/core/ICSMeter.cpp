#include "ICSMeter.hpp"

#include "../FS/Updater.cpp"
#include "../net/Controls.cpp"
#include "../net/Bluetooth.cpp"
#include "../net/WiFi.cpp"
#include "../UI/Menu.cpp"
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

  // Setup
  void setup()
  {
    uint8_t loop = 0;

    // Init M5
    auto cfg = M5.config();
    M5.begin(cfg);

    // Init Led
    if(M5.getBoard() == m5::board_t::board_M5Stack) {
      FastLED.addLeds<NEOPIXEL, 15>(leds, NUM_LEDS);  // GRB ordering is assumed
    } else if(M5.getBoard() == m5::board_t::board_M5StackCore2) {
      FastLED.addLeds<NEOPIXEL, 25>(leds, NUM_LEDS);  // GRB ordering is assumed
    }
    // If led strip...
    /*
    FastLED.addLeds<WS2811,Neopixel_PIN,GRB>(strip, NUM_LEDS_STRIP).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(32);
    */
    // Preferences
    preferences.begin(NAME);
    measure = preferences.getUInt("measure", 1);
    brightness = preferences.getUInt("brightness", 64);
    transverter = preferences.getUInt("transverter", 0);
    beep = preferences.getUInt("beep", 0);
    screensaver = preferences.getUInt("screensaver", 60);
    theme = preferences.getUInt("theme", 0);
    // Bin Loader
    binLoader();
    // Wifi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED && loop <= 10) {
      vTaskDelay(250);
      loop += 1;
    }
    // Start server (for Web site Screen Capture)
    httpServer.begin();

    setBrightness(map(brightness, 1, 100, 1, 254));
    M5.Lcd.setRotation(1);
    viewGUI();

    if(IC_MODEL == 705 && IC_CONNECT == BT) {
      CAT.register_callback(bluetooth::callbackBT);

      if (!CAT.begin(NAME)) {
        Serial.println("An error occurred initializing Bluetooth");
      } else {
        Serial.println("Bluetooth initialized");
      }
    } else {
      if (WiFi.status() == WL_CONNECTED) wifiConnected = true;
    }

    // Multitasking task for retreive button
    xTaskCreatePinnedToCore( buttonTask, "buttonTask", 8192, NULL, 4, NULL, 1);
  }

  // Main loop
  void loop()
  {
    static uint8_t alternance = 0;
    static uint8_t tx = 0;

    if(checkConnection()) {
      tx = getTX();
      if(tx != 0) screensaverTimer = millis();   // If transmit, refresh tempo

      if (screensaverMode == false && screenshot == false && settingsMode == false) {
        settingLock = true;

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

        viewMeasure();
        viewBattery();

        getMode();
        getFrequency();

        switch (measure) {
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
        settingLock = false;
      }
    }
    alternance = (alternance++ < 2) ? alternance : 0;

    // Manage Screen Saver
    wakeAndSleep();

    if(DEBUG) {
      Serial.print(screensaverMode);
      Serial.print(" ");
      Serial.print(millis() - screensaverTimer);
      Serial.print(" ");
      Serial.println(screensaver * 60 * 1000);
    }
  }


};
