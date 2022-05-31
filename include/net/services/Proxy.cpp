#include "../Daemon.hpp"

namespace ICSMeter
{
  namespace net
  {

    namespace proxy
    {

      using namespace daemon;

      constexpr const char* MSG_CHECKTX     = "Check TX";
      constexpr const char* MSG_CHECKPROXY  = "Check Proxy";
      constexpr const char* MSG_TX_UP       = "TX connected";
      constexpr const char* MSG_TX_DOWN     = "TX disconnected";
      constexpr const char* MSG_MAX_ERRORS  = "Bad network";


      uint32_t check_frequency = 5000; // delay between end of last request and next request
      uint32_t last_check = millis();
      bool online = false;
      bool txConnected = false;
      bool had_success = false;

      void setup()
      {
        message = checkStatus();
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


      const char* checkStatus()
      {
        if( !wifi::available() ) return wifi::MSG_WIFI_DOWN;
        if( errors_count > max_errors ) return MSG_MAX_ERRORS; // or screensaver, shutdown ?
        uint32_t now = millis();
        if( last_check + check_frequency > now ) return message;

        log_d("Checking proxy status");

        const char *resp = nullptr;
        //const char request[] = {0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x03, 0xFD};
        CIV::setRequest( { 0x03, 0xFD, 0x00} );

        // go online
        if( !wifi::sendCommand(CIV::request, 6, CIV::buffer, 6) ) {
          if( !proxy::available() ) {
            resp = MSG_CHECKPROXY;
          } else {
            resp = MSG_CHECKTX;
          }
        } else {
          Serial.println( MSG_TX_UP );
        }

        last_check = millis();
        return resp;
      }


    };

  };

};

