#include "Daemon.hpp"
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

      uint32_t tx_poll_frequency = 1000; // milliseconds between each ICScan()
      uint32_t tx_last_poll      = millis();
      static uint8_t tx = 0;


      void setup()
      {
        proxy::agent.setup();
      }


      void loop()
      {
        using namespace modules;

        if( daemon::connected() ) {

          if(tx != 0) {
            log_i("Drawing widgets");
            ScreenSaver::resetTimer(); // If transmit, refresh tempo
            FastLed::set( tx );
            UI::drawWidgets();
            tx = 0;
          }

        } else {

          if( UI::canDraw() ) {
            static bool active_state = false;
            bool state = (millis()/773)%2==0;
            if( active_state != state ) {
              CSS::drawStyledString( &tft, state ? message : "", 160, 180, Theme::H3FontStyle );
              active_state = state;
            }
          }

        }
      }


      bool connected() // called from main loop, non blocking
      {
        return message ? false : true;
      }


      void netTask(void *pvParameters)
      {
        daemon::setup();

        for (;;) {

          //screenshot::check(); // check for queued screenshot request

          daemon::check(); // check for wifi/ble/proxy health

          vTaskDelay( 10 );
        }
      }


      void check() // called from task, may induce network blocking operations
      {
        if( ! proxy::agent.available() ) { // link check
          message = proxy::agent.message;
          return;
        }

        proxy::checkStatus();

        if( ScreenSaver::isAsleep() ) {
          return;
        }

        if( ! proxy::available() ) {
          //message = proxy::MSG_CHECKTX;
          return;
        }

        if( ! proxy::connected() ) {
          message = proxy::MSG_CHECKPROXY;
          return;
        }

        if( tx_last_poll + tx_poll_frequency < millis() ) {
          tx = CIV::getTX();
          if(tx != 0) {
            log_v("Polling");
            daemon::ICScan(); // query all statuses
            proxy::last_check = millis();
          }
          tx_last_poll = millis();
        }


//         if ( daemon::needsPairing() ) {
//           message = MSG_NEEDPAIRING;
//         } else if ( daemon::needsWiFiChecked() ) {
//           message = MSG_CHECKWIFI;
//         } else if ( daemon::needsProxyChecked() ) {
//           if( ScreenSaver::isAwake() ) {
//             message = proxy::checkStatus();
//           } else {
//             message = proxy::MSG_CHECKTX; // won't be displayed but error state needs to be maintained
//           }
//         } else {
//           message = nullptr;
//           if( tx_last_poll + tx_poll_frequency < millis() ) {
//             tx = CIV::getTX();
//             if(tx != 0) {
//               proxy::last_check = millis();
//               daemon::ICScan(); // query all statuses
//             }
//             tx_last_poll = millis();
//           }
//         }
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


      bool dispatchCommand( char *request, size_t request_size, char *response, uint8_t response_size ) // CI-V Command dispatcher
      {
        if( !proxy::connected() ) {
          log_w("Dispatch request while proxy is diconnected???");
          return false;
        }
        if( !proxy::available() ) {
          log_w("Dispatch request while proxy is offline???");
          return false;
        }
        bool ret = proxy::agent.sendCommand(request, request_size, response, response_size);
        if( ret ) proxy::last_check = millis();
        return ret;
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


      // some boolean verbs for user config specifics

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


    };

  };

};

