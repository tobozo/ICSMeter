#include "../Daemon.hpp"

namespace ICSMeter
{
  namespace net
  {

    namespace wifi
    {

      using namespace proxy;
      using namespace daemon;

      constexpr const char* MSG_WIFI_UP   = "Wifi Client Connected";
      constexpr const char* MSG_WIFI_DOWN = "Wifi Client disconnected";
      constexpr const char* MSG_CHECKWIFI = "Check Wifi";

      comm_struct_t agent = { wifi::setup, wifi::loop, wifi::available, WebClient::sendCommand, wifi::message };

      char wifi_ssid[33];
      char wifi_pass[64];

      const uint32_t max_reconnect_attempts = 10;
      uint32_t connect_attempts = 0;
      bool connected = false;
      bool was_connected = false;
      bool has_credentials = ( strcmp( WIFI_SSID, "YOUR WIFI SSID" )!=0 && strcmp( WIFI_PASSWORD, "YOUR WIFI PASSWORD" )!=0 );
      bool setup_done = false;


      void setup()
      {
        daemon::message = MSG_CHECKWIFI;
        if( !setup_done ) {
          if( !has_credentials ) {
            if( loadCredentials( wifi_ssid, wifi_pass ) ) {
              // got some ssid/pass from NVS
              log_d("Loaded WiFi credentials from NVS");
            } else {
              log_w("No WiFi credentials found either in settings.h or in NVS");
            }
          } else {
            // got some ssid/pass from the settings.h, see if they need persistence
            if( saveCredentials( WIFI_SSID, WIFI_PASSWORD ) ) {
              log_d("Successfully saved WiFi credentials");
            } else {
              log_d("WiFi credentials are both in settings.h and NVS");
            }
            loadCredentials( wifi_ssid, wifi_pass );
          }
          WiFi.onEvent( WiFiEvent ); // attach event manager
          setup_done = true;
        }
        begin();
      }


      void loop()
      {
        static uint32_t retrytimer = millis();
        static uint32_t retries = 50;


        if( !wifi::connected && wifi::was_connected ) {
          // lost wifi connection
          // TODO: ESP set deep sleep( wake by touch int )
          return;
        }


        if( !CIV::txConnected && CIV::last_poll + CIV::poll_timeout < millis() ) {
          if( CIV::had_success ) {

            if( retries == 0 ) {
              // TODO: full sleep
              log_e("Max retries reached, giving up");
              return;
            }

            const uint32_t now = millis();
            const uint32_t retryafter = 5000;
            if( retrytimer + retryafter < millis() ) {
              // retry subscriptions
              WebClient::setup();
              retrytimer = millis();
            }
          }
        } else {
          retries = 50;
          retrytimer = millis();
        }
      }


      void begin()
      {
        if( ! has_credentials ) {
          log_i("Logging WiFi without credentials");
          WiFi.begin();
          //log_d("No credentials provided in settings.h, using previously saved tokens, if any...");
        } else {
          log_d("Logging WiFi with credentials: %s / %s", wifi_ssid, wifi_pass );
          WiFi.begin( wifi_ssid, wifi_pass );
        }
      }


      bool loadCredentials( char* dst_wifi_ssid, char* dst_wifi_pass )
      {
        char ssid[33];
        char pass[64];
        size_t s1, s2;

        s1 = prefs::getString( "wifi.ssid", ssid, 32 );
        s2 = prefs::getString( "wifi.pass", pass, 63 );

        bool res = s1+s2 > 0;

        if( res ) { // overwrite defauts if values found in prefs
          // populate if needed
          if( dst_wifi_ssid ) snprintf( dst_wifi_ssid, 32, "%s", ssid );
          if( dst_wifi_pass ) snprintf( dst_wifi_pass, 63, "%s", pass );
          has_credentials = true;
        }
        return res;
      }


      bool saveSSID( const char* new_ssid )
      {
        if( !new_ssid || strlen(new_ssid)== 0 ) {
          log_e("Can't save empty ssid!");
          return false;
        }
        char ssid[33];
        size_t s = prefs::getString( "wifi.ssid", ssid, 32 );
        if( s>0 && strcmp(ssid, new_ssid)==0 ) {
          log_d("Ignoring save as source and destination are similar");
          return false; // unchanged/empty
        }
        prefs::setString( "wifi.ssid", new_ssid );
        log_i("Saved SSID");
        return true;
      }


      bool savePass( const char* new_pass )
      {
        if( !new_pass || strlen(new_pass)== 0 ) {
          log_e("Can't save empty pass!");
          return false;
        }
        char pass[64];
        size_t s = prefs::getString( "wifi.pass", pass, 63 );
        if( s>0 && strcmp(pass, new_pass)== 0 ) {
          log_d("Ignoring save as source and destination are similar");
          return false; // unchanged/empty
        }
        prefs::setString( "wifi.pass", new_pass );
        log_i("Saved Pass");
        return true;
      }


      bool saveCredentials( const char* new_ssid, const char* new_pass )
      {
        bool ret1 = saveSSID( new_ssid );
        bool ret2 = savePass( new_pass );

        return ret1 || ret2;
      }



      bool available()
      {
        return connected;
      }


      // This is for espressif arduino-1.0.6 package
      // Some events have been renamed since 2.x.x, fortunately we know this ESP_ARDUINO_VERSION_VAL macro didn't exist before 2.x.x
      #if !defined ESP_ARDUINO_VERSION_VAL
        #define ARDUINO_EVENT_WIFI_STA_GOT_IP SYSTEM_EVENT_STA_GOT_IP
        #define ARDUINO_EVENT_WIFI_STA_DISCONNECTED SYSTEM_EVENT_STA_DISCONNECTED
      #endif


      void WiFiEvent(WiFiEvent_t event) // wifi event handler
      {
        switch(event) {
          case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.println( MSG_WIFI_UP );
            Serial.println(WiFi.localIP());
            wifi::connected = true;
            wifi::was_connected = true;
            daemon::message = proxy::MSG_CHECKPROXY;
            WebServer::setupAsync(); // start websocket server and wait for proxy connections
            WebClient::setup();      // start http client and try to contact proxy
            connect_attempts = 0;
          break;
          case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println( MSG_WIFI_DOWN );
            wifi::connected = false;
            daemon::message = MSG_CHECKWIFI;
            WiFi.reconnect();
            if( ++connect_attempts >= max_reconnect_attempts ) { // TODO: reset connect_attempts from settings
              WiFi.disconnect( true );
              begin();
              WiFi.scanNetworks();
              connect_attempts = 0;
            }
          break;
          default: break;
        }
      }

    };

  };

};
