#include "../Daemon.hpp"

namespace ICSMeter
{
  namespace net
  {

    namespace proxy
    {

      using namespace daemon;

      uint32_t check_frequency = 5000; // delay between end of last request and next request
      uint32_t last_check = millis();
      bool online = false;
      bool txConnected = false;
      bool had_success = false;

      void setup()
      {
        checkStatus();
      }


      bool available()
      {
        return online;
      }


      bool connected()
      {
        return txConnected;
      }


      void setFlag( proxy_flags_t flag )
      {
        switch( flag ) {
          case PROXY_ONLINE:  online = true;       break;
          case PROXY_OFFLINE: online = false;      break;
          case TX_ONLINE:     txConnected = true;  had_success = true; break;
          case TX_OFFLINE:    txConnected = false; break;
        }
      }


      void checkStatus()
      {
        log_v("Checking link (WiFi or Bluetooth) status");
        if( !agent.available() ) {
          message = agent.message;
          return;
        }

        uint32_t now = millis();
        if( last_check + check_frequency > now ) return;

        log_v("Checking tx status");

        CIV::setRequest( CIV::CIV_CHECK );

        // go online
        if( !agent.sendCommand(CIV::request, 6, CIV::buffer, 6) ) {
          if( !proxy::available() ) {
            message = MSG_CHECKPROXY;
          } else {
            message = agent.message ? agent.message : MSG_CHECKTX;
          }
        } else {
          message = nullptr;
          log_d( MSG_TX_UP );
        }
        last_check = millis();
      }


    };

  };

};

