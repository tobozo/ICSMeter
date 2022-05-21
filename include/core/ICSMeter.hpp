#pragma once

#include "../../main/core.h"

namespace ICSMeter
{

  M5GFX &tft(M5.Lcd); // provide a shorthand to "M5.Lcd"

  void setup(); // Main Setup

  void loop(); // Main loop

  void checkButtons();

  void shutdown();

  // Fake mutex support, this helps mitigate SPI collisions
  static bool LcdMux = false;
  void takeLcdMux();
  void giveLcdMux();


  namespace prefs
  {
    void load();
    unsigned int get( const char* name, unsigned int default_value );
    void set( const char* name, unsigned int value );
  };

  namespace FSUpdater
  {
    void binLoader();
    void getBinaryList( fs::FS *sourceFS );
    #if defined GZIP_BINLOADER
      void updateProgressCallback( uint8_t progress );
    #endif
  };


  namespace modules
  {
    namespace buttons
    {
      void loop();
      void cancelBubble();
      bool hasBubble();
      int btnA, btnB, btnC;
      // Flags for button presses via Web site Screen Capture
      bool buttonLeftPressed   = false;
      bool buttonCenterPressed = false;
      bool buttonRightPressed  = false;
    };

    namespace BackLight
    {
      void setup();
      uint8_t getBrightness();
      void setBrightness( uint8_t value );
      void increase();
      void decrease();
      void save();
    };

    namespace FastLed
    {
      void setup();
      void set( uint8_t tx );
    };

    namespace Beeper
    {
      void setup();
      void loop();
      void increase();
      void decrease();
      void save();
    };
  };



};
