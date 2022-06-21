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
        daemon::agent = CIV::IC->type == IC_COMM_BLUETOOTH ? &bluetooth::agent : &wifi::agent;
        daemon::agent->setup();
        while( !daemon::agent->available() ) {
          // TODO: timeout this
          vTaskDelay(1);
        }
        log_i("Daemon setup complete");
      }


      bool connected() // called from main loop, non blocking
      {
        return message ? false : true;
      }


      void daemonTask(void *pvParameters)
      {
        using namespace modules;
        daemon::setup();

        for (;;) {

          CIV::loop();
          daemon::agent->loop();

          if( CIV::txConnected ) {
            if(CIV::status.tx != 0) {
              ScreenSaver::resetTimer();
              FastLed::set( CIV::status.tx );
              CIV::status.tx = 0;
            }
          } else {
            if( !daemon::agent->available() ) {
              // tx lost because link lost (WiFi or BLE)
            } else {
              // tx lost because IC off or python proxy down
            }


          }

          vTaskDelay( 10 );
        }
      }

/*
      void check() // called from task, may induce network blocking operations
      {
        if( ! daemon::agent.available() ) { // link check
          message = daemon::agent.message;
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
      }
*/


      void ICScan() // called from main loop
      {
        if( WebClient::has_subscriptions ) return; // CIV updates are coming via async websocket

        // no websocket on the proxy (yet?), query via synchroneous HTTP

        CIV::getMode();
        CIV::getFrequency();
        switch( Measure::mode ) {
          case Measure::MODE_PWR: CIV::getPower();  break;
          case Measure::MODE_SMT: CIV::getSmeter(); break;
          case Measure::MODE_SWR: CIV::getSWR();    break;
          default:
            // this can't happen since Measure::mode is enum, but who knows?
            log_e("Unsupported measure mode: 0x%02x",  Measure::mode );
          break;
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
        bool ret = daemon::agent->sendCommand(request, request_size, response, response_size);
        if( ret ) proxy::last_check = millis();
        return ret;
      }



    };

  };

};

