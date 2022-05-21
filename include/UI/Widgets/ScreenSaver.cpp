
namespace ICSMeter
{

  namespace UI
  {

    namespace ScreenSaver
    {
      using namespace modules;

      uint32_t timer;
      uint32_t countdown;
      uint32_t countdownOld;
      const uint32_t min_countdown = 1;
      const uint32_t max_countdown = 60;

      uint16_t x = rand() % 232;
      uint16_t y = rand() % 196;
      bool xDir = rand() & 1;
      bool yDir = rand() & 1;

      const uint16_t logo_width  = 44;
      const uint16_t logo_height = 22;
      const uint16_t min_x = 44;
      const uint16_t max_x = 232;
      const uint16_t min_y = 22;
      const uint16_t max_y = 196;

      const float brightness_dim = 0.5; // percentage of current brightness
      uint8_t old_brightness = 0x40;

      constexpr const char *label = "TIMEOUT";


      void setup()
      {
        countdown = prefs::get("screensaver", 60);
      }


      void save()
      {
        uint32_t tmp = prefs::get("screensaver", 60);
        if( tmp != countdown ) {
          countdownOld = tmp;
          prefs::set("screensaver", countdown);
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


      void sleep()
      {
        Settings::dialog_enabled = false;
        old_brightness = BackLight::getBrightness();
        BackLight::setBrightness( old_brightness * brightness_dim );
        enabled = true;
        wakeup = false;
        timer = 0;
        tft.fillScreen(TFT_BLACK);
      }

      void wake()
      {
        tft.fillScreen(TFT_BLACK);
        //clearData();
        BackLight::setBrightness( old_brightness );
        UI::drawWidgets( true );
        enabled = false;
        wakeup  = true;
        //reset();
        //Settings::dialog_enabled = false;
        vTaskDelay(100);
      }


      void handle()
      {
        if( !enabled ) {
          if( millis() - timer > countdown * 60 * 1000 ) { // time to save screen, prepare display and clear !
            ScreenSaver::sleep();
          }
          return;
        }

        if( wakeup ) { // wakeup
          ScreenSaver::wake();
          return;
        }

        ScreenSaver::draw();
        vTaskDelay(75);
      }


      void draw()
      {
        x += (xDir) ? 1 : -1;
        y += (yDir) ? 1 : -1;

        xDir = (x < min_x) ? true  : (x > max_x) ? false : xDir;
        x    = (x < min_x) ? min_x : (x > max_x) ? max_x : x; // constrain

        yDir = (y < min_y) ? true  : (y > max_y) ? false : yDir;
        y    = (y < min_y) ? min_y : (y > max_y) ? max_y : y; // constrain

        tft.drawJpg(logo, sizeof(logo), x, y, logo_width, logo_height);
        tft.drawRect( x-1, y-1, logo_width+2, logo_height+2, TFT_BLACK );
      }



    };

  };

};

