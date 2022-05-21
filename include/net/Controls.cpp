#include "Controls.hpp"
#include "../Utils/Maths.hpp"
#include "../UI/UI.hpp"
#include "../UI/Widgets.hpp"



namespace ICSMeter
{

  namespace net
  {

    namespace daemon
    {

      using namespace Utils;
      using namespace UI;

      constexpr const char* MSG_NEEDPAIRING = "Need Pairing";
      constexpr const char* MSG_CHECKWIFI   = "Check Wifi";


      void setup()
      {
        if( !wifi::available() ) {
          wifi::setup(); // non blocking wifi setup
        }

        if( hasBluetooth() ) {
          bluetooth::setup(); // start BT if needed
        }
      }


      void netTask(void *pvParameters)
      {
        daemon::setup();

        for (;;) {

          screenshot::check(); // check for queued screenshot request
          daemon::check();

          vTaskDelay( 10 );
        }
      }


      void check() // called from task, may induce network blocking operations
      {
        if ( daemon::needsPairing() ) {
          message = MSG_NEEDPAIRING;
        } else if ( daemon::needsWiFiChecked() ) {
          message = MSG_CHECKWIFI;
        } else if ( daemon::needsProxyChecked() ) {
          message = proxy::checkStatus();
        } else {
          message = nullptr;
        }
      }


      bool connected() // called from main loop, non blocking
      {
        if (canSleep() ) {
          // Sleep signal received (e.g. no TX connexion or screensaver enabled)
          tft.sleep();
          ScreenSaver::sleep();
        } else if (canWakeup()) {
          // Wake signal received
          clearData();
          tft.wakeup();
          UI::draw();
          ScreenSaver::reset();
        }

        if ( message && UI::canDrawUI() ) {
          static bool active_state = false;
          bool state = (millis()/773)%2==0;
          if( active_state != state ) {
            CSS::drawStyledString( &tft, state ? message : "", 160, 180, Theme::H3FontStyle );
            active_state = state;
          }
          return false;
        }
        return true;
      }


      void clearData() // lost in refactoring
      {
        Needle::reset();
        Measure::reset();
        Battery::reset();
      }


      void ICScan() // called from main loop
      {
        setMode();
        setFrequency();
        switch (Measure::value) {
          case 0: setPower();  break;
          case 1: setSmeter(); break;
          case 2: setSWR();    break;
          default: /* duh? */  break;
        }
      }


      bool dispatchCommand(char *request, size_t n, char *buffer, uint8_t limit) // CI-V Command dispatcher
      {
        if ( hasBluetooth() ) return bluetooth::sendCommand(request, n, buffer, limit);
        else if( !proxy::available() ) return false;
        return wifi::sendCommand(request, n, buffer, limit);
      }


      void setSmeter()
      {
        CIV::GaugeMeasure_t GaugeMeasure = CIV::getSmeter();
        if( GaugeMeasure.label != "" ) {
          Needle::set( GaugeMeasure.angle );
          Measure::setPrimaryValue( GaugeMeasure.label );
        }
      }


      void setSWR()
      {
        CIV::GaugeMeasure_t GaugeMeasure = CIV::getSWR();
        if( GaugeMeasure.label != "" ) {
          Needle::set( GaugeMeasure.angle );
          Measure::setPrimaryValue( GaugeMeasure.label );
        }
      }


      void setPower()
      {
        CIV::GaugeMeasure_t GaugeMeasure = CIV::getPower();
        if( GaugeMeasure.label != "" ) {
          Needle::set( GaugeMeasure.angle );
          Measure::setPrimaryValue( GaugeMeasure.label );
        }
      }


      void setDataMode()
      {
        DataMode::mode = CIV::getDataMode();
      }


      void setFrequency()
      {
        Measure::setSecondaryValue( CIV::getFrequency() );
      }


      void setMode()
      {
        CIV::FilterMode_t FilterMode = CIV::getMode();
        DataMode::setFilter( FilterMode.filter );
        DataMode::setMode( FilterMode.mode );
      }


      // some boolean verbs for user settings specifics

      bool hasBluetooth()
      {
        return IC_MODEL == 705 && IC_CONNECT == BT;
      }

      bool needsPairing()
      {
        return (hasBluetooth() && !bluetooth::available());
      }

      bool needsWiFiChecked()
      {
        return (IC_CONNECT == USB && !wifi::available());
      }

      bool needsProxyChecked()
      {
        return (IC_CONNECT == USB && (proxy::available() || !proxy::connected()));
      }

      bool canSleep()
      {
        return ( ScreenSaver::wakeup && proxy::available() ) // tft must be awake to go to sleep
            && ( ( IC_CONNECT == BT && !bluetooth::available()) || ( IC_CONNECT == USB && !proxy::connected() ) );
      }

      bool canWakeup()
      {
        return ( !ScreenSaver::wakeup && proxy::available() ) // tft must be asleep to wake
            && ( ( IC_CONNECT == BT && bluetooth::available() ) || ( IC_CONNECT == USB && !proxy::connected() ) );
      }


    };

  };

};

