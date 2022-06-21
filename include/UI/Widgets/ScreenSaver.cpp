

/*


  - Screen saver is triggered by a timer
  - Screen saving consists in:
    - temporarily reducing brightness (see brightness_dim)
    - animating a logo **unless tft sleep mode is enabled**
  - Screen saving can be interrupted by:
    - resetting the timer with ScreenSaver::resetTimer()
    - button press
    - incoming message from IC
    - proxy status change

*/

namespace ICSMeter
{

  namespace UI
  {

    namespace ScreenSaver
    {
      using namespace modules;
      using namespace net;

      uint32_t timer;
      uint32_t minutes_countdown;  // minutes, loaded from preferences / editable via settings panel
      uint32_t milliseconds_countdown;       // milliseconds translated from minutes_countdown
      uint32_t countdownOld;
      const uint32_t min_countdown = 1;
      const uint32_t max_countdown = 60;

      const uint16_t logo_width  = 44;
      const uint16_t logo_height = 22;
      const uint16_t min_x = logo_width;
      const uint16_t max_x = 320-logo_width;
      const uint16_t min_y = logo_height;
      const uint16_t max_y = 240-logo_height;

      uint16_t x = rand() % max_x;
      uint16_t y = rand() % max_y;
      bool xDir = rand() & 1;
      bool yDir = rand() & 1;


      bool tft_sleep_mode_enabled   = false;
      bool tft_screen_saver_enabled = false;


      const float brightness_dim = 0.5; // percentage of current brightness
      uint8_t old_brightness = 0x40;

      constexpr const char *label = "TIMEOUT";


      void setup()
      {
        uint32_t minutes = prefs::get("screensaver", 60);
        ScreenSaver::setTimer( minutes );
        ScreenSaver::resetTimer();
      }


      void loop()
      {
        if( ScreenSaver::shouldSleep() ) { // IC or timer signal
          log_i("shouldSleep()==true, will sleep");
          ScreenSaver::sleep();
          return;
        }
        if( ( ScreenSaver::isAsleep() || ScreenSaver::isEnabled() ) && ScreenSaver::shouldWakeup() ) { // proxy or buttons
          log_i("shouldWakeup()==true, will wakeup");
          ScreenSaver::wakeup();
          return;
        }

        if( ScreenSaver::isDisabled() && ScreenSaver::isAwake() ) { // timer signal
          if( millis() - timer > milliseconds_countdown ) { // enable screensaver animation
            ScreenSaver::enable();
          }
          return;
        }

        if( !ScreenSaver::isDisabled() && timer != 0 ) { // wakeup/disable signal
          ScreenSaver::disable();
          return;
        }

        if( ScreenSaver::isAsleep() || ScreenSaver::isDisabled() ) return;

        ScreenSaver::draw();
        vTaskDelay(75);
      }


      // some closed answer verbs

      bool isEnabled()  { return   tft_screen_saver_enabled; }
      bool isDisabled() { return ! tft_screen_saver_enabled; }
      bool isAsleep()   { return   tft_sleep_mode_enabled;   }
      bool isAwake()    { return ! tft_sleep_mode_enabled;   }

      bool shouldSleep() // sleep signal comes either from IC being turned off (depends on proxy) or by timer
      {
        if( ScreenSaver::isAsleep() ) {
          log_w("Already sleeping");
          return false; // already sleeping
        }
        if( ScreenSaver::isAwake() && proxy::available() && CIV::had_success // tft must be awake to go to sleep
            && ( ( CIV::IC->type == IC_COMM_BLUETOOTH && !bluetooth::available()) || ( CIV::IC->type == IC_COMM_WIFI && !proxy::available() ) ) ) {
          log_i("is awake and should sleep");
          return true;
        }
        uint32_t elapsed = millis() - timer; // elapsed time since last timer reset
        //log_i( "is awake and timer sleep (%d/%d)", elapsed, milliseconds_countdown*2 );
        return elapsed > milliseconds_countdown*2;
      }

      bool shouldWakeup() // wakeup signal comes from proxy, buttons, or timer
      {
        if( ScreenSaver::isAwake() && ScreenSaver::isDisabled() ) return false; // already awake and not saving screen
        if( ScreenSaver::isAwake() && ScreenSaver::isEnabled() ) return buttons::hasBubble(); // already awake and saving screen
        if( ScreenSaver::isAsleep() && buttons::hasBubble() ) return true; // sleeping and waiting for button push
        if( ScreenSaver::isAsleep() || ScreenSaver::isEnabled() ) return timer != 0; // timer has been reset
        if( ScreenSaver::isAsleep() && proxy::available() ) { // sleeping and waiting for proxy coming online
          return ( ( CIV::IC->type == IC_COMM_BLUETOOTH && bluetooth::available() ) || ( CIV::IC->type == IC_COMM_WIFI && !proxy::available() ) ); // BT became avail or proxy disconnected
        }
        return false; // nothing-happenness
      }


      void enable()
      {
        log_d("Enabling screen saver");
        old_brightness = BackLight::getBrightness();
        BackLight::setBrightness( old_brightness * brightness_dim );
        tft_screen_saver_enabled = true;
        timer = 0;
        tft.fillScreen(TFT_BLACK);
      }

      void disable()
      {
        log_d("Disabling screen saver");
        tft_screen_saver_enabled = false;
        buttons::cancelBubble();
        ScreenSaver::resetTimer();
        if( ScreenSaver::isAsleep() ) {
          ScreenSaver::wakeup();
        }
        BackLight::setBrightness( old_brightness );
        UI::draw( true );
      }


      void sleep()
      {
        log_d("Entering TFT sleep mode");
        tft_sleep_mode_enabled = true;
        tft.sleep();
      }


      void wakeup()
      {
        log_d("Leaving TFT sleep mode");
        bool force_disable = false;
        buttons::cancelBubble();
        ScreenSaver::resetTimer();
        if( ScreenSaver::isAsleep() ) {
          tft_sleep_mode_enabled = false;
          tft.wakeup();
          force_disable = true;
        }
        if( force_disable || ScreenSaver::isEnabled() ) {
          ScreenSaver::disable();
        }
      }


      void draw()
      {
        if( ScreenSaver::isAsleep() || ScreenSaver::isDisabled() ) return;

        x += (xDir) ? 1 : -1;
        y += (yDir) ? 1 : -1;

        xDir = (x < min_x) ? true  : (x > max_x) ? false : xDir;
        x    = (x < min_x) ? min_x : (x > max_x) ? max_x : x; // constrain

        yDir = (y < min_y) ? true  : (y > max_y) ? false : yDir;
        y    = (y < min_y) ? min_y : (y > max_y) ? max_y : y; // constrain

        tft.drawJpg(logo, sizeof(logo), x, y, logo_width, logo_height);
        tft.drawRect( x-1, y-1, logo_width+2, logo_height+2, TFT_BLACK );
      }


      // getters / setters

      void setTimer( uint32_t minutes )
      {
        minutes_countdown = minutes;
        milliseconds_countdown = minutes * 60 * 1000;
      }


      uint32_t getDelay()
      {
        return max( uint32_t(1), minutes_countdown );
      }


      uint32_t getSleepDelay()
      {
        return ScreenSaver::getDelay() * 2;
      }


      void resetTimer()
      {
        timer = millis();
      }



      // settings panel helpers

      void save()
      {
        uint32_t tmp = prefs::get("screensaver", max_countdown );
        if( tmp != minutes_countdown ) {
          countdownOld = tmp;
          prefs::set("screensaver", minutes_countdown);
        }
      }


      void increase()
      {
        uint32_t minutes = ScreenSaver::getDelay()+1;
        minutes++;
        minutes = min( max_countdown, minutes );
        ScreenSaver::setTimer( minutes );
      }


      void decrease()
      {
        uint32_t minutes = ScreenSaver::getDelay()-1;
        minutes--;
        minutes = max( min_countdown, minutes );
        ScreenSaver::setTimer( minutes );
      }


    };

  };

};

