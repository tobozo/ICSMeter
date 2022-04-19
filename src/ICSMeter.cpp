// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "settings.h"
#include "ICSMeter.h"
#include "font.h"
#include "tools.h"
#include "image.h"
#include "webIndex.h"
#include "functions.h"
#include "command.h"
#include "tasks.h"

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
  }
  else if(M5.getBoard() == m5::board_t::board_M5StackCore2) {
    FastLED.addLeds<NEOPIXEL, 25>(leds, NUM_LEDS);  // GRB ordering is assumed
  }

  // Init Power
  //power();

  // Preferences
  preferences.begin(NAME);
  option = preferences.getUInt("option", 2);
  brightness = preferences.getUInt("brightness", 64);

  // Bin Loader
  //binLoader();

  // Wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED && loop <= 10)
  {
    vTaskDelay(250);
    loop += 1;
  }

  // Start server (for Web site Screen Capture)
  httpServer.begin();

  setBrightness(brightness);
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(TFT_BACK);

  viewGUI();

  if(IC_MODEL == 705 && IC_CONNECT == BT)
  {
    CAT.register_callback(callbackBT);
    
    if (!CAT.begin(NAME))
    {
      Serial.println("An error occurred initializing Bluetooth");
    }
    else
    {
      Serial.println("Bluetooth initialized");
    }
  }
  else
  {
    if (WiFi.status() == WL_CONNECTED) wifiConnected = true;
  }

  // Multitasking task for retreive button
  xTaskCreatePinnedToCore(
      button,   // Function to implement the task
      "button", // Name of the task
      8192,     // Stack size in words
      NULL,     // Task input parameter
      4,        // Priority of the task
      NULL,     // Task handle
      1);       // Core where the task should run
}

// Main loop
void loop()
{
  static uint8_t alternance = 0;
  static uint8_t tx = 0;

  if(checkConnection()) {
    tx = getTX();
    if(tx != 0) screensaver = millis();   // If transmit, refresh tempo

    if (screensaverMode == false && screenshot == false) {

      if(tx == 0) {
        for(uint8_t i = 0; i <= 9; i++){
          leds[i] = CRGB::Black;
        }
        FastLED.setBrightness(16);
        FastLED.show();
      }
      else {
       for(uint8_t i = 0; i <= 9; i++){
          leds[i] = CRGB::Red;
        }
        FastLED.setBrightness(16);
        FastLED.show();
      }
     
      viewMenu();
      viewBattery();
      viewBaseline(alternance);

      getMode();
      getFrequency();

      switch (option)
      {
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
    }
  }

  alternance = (alternance++ < 30) ? alternance : 0;

  // Manage Screen Saver
  wakeAndSleep();
}