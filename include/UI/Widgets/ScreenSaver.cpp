


namespace ICSMeter
{

  namespace UI
  {

    namespace ScreenSaver
    {

      uint32_t timer;
      uint32_t countdown;
      uint32_t countdownOld;
      const uint32_t min_countdown = 1;
      const uint32_t max_countdown = 60;

      uint16_t x = rand() % 232;
      uint16_t y = rand() % 196;
      bool xDir = rand() & 1;
      bool yDir = rand() & 1;


      constexpr const char *label = "TIMEOUT";

      void setup()
      {
        countdown = getPref("screensaver", 60);
      }


      void save()
      {
        uint32_t tmp = getPref("screensaver", 60);
        if( tmp != countdown ) {
          countdownOld = tmp;
          setPref("screensaver", countdown);
        }
      }


      void shutdown()
      {
        M5.Power.powerOff();
      }


      void reset()
      {
         timer   = millis();
         enabled = false;
      }


      void increase()
      {
        countdown++;
        countdown = min( max_countdown, countdown );
      }


      void decrease()
      {
        countdown--;
        countdown = max( min_countdown, countdown );
      }


      void handle()
      {
        if(! enabled ) return;

        if( millis() - timer > countdown * 60 * 1000 ) { // time to save screen, prepare display and clear !
          Settings::dialog_enabled = false;
          enabled = true;
          timer = 0;
          tft.fillScreen(TFT_BLACK);
          return;
        }

        if( timer != 0 ) { // wakeup
          tft.fillScreen(TFT_BLACK);
          //clearData();
          UI::drawWidgets( true );
          enabled = false;
          Settings::dialog_enabled = false;
          vTaskDelay(100);
          return;
        }

        tft.fillRect(x, y, 44, 22, TFT_BLACK);

        if (xDir)  x += 1;
        else  x -= 1;

        if (yDir)  y += 1;
        else  y -= 1;

        if (x < 44) {
          xDir = true;
          x = 44;
        } else if (x > 232) {
          xDir = false;
          x = 232;
        }

        if (y < 22) {
          yDir = true;
          y = 22;
        } else if (y > 196) {
          yDir = false;
          y = 196;
        }

        tft.drawJpg(logo, sizeof(logo), x, y, 44, 22);

        vTaskDelay(75);

        // if (IC_MODEL == 705 && IC_CONNECT == BT && bluetooth::connected == false) {
        //   vTaskDelay(75);
        // } else if (IC_CONNECT == USB && wifi::connected == false) {
        //   vTaskDelay(75);
        // }


      }


    };

  };

};

