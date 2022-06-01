#include "../Daemon.hpp"

namespace ICSMeter
{
  namespace net
  {

    namespace wifi
    {

      using namespace proxy;
      using namespace daemon;

      WiFiClient civClient;

      constexpr const char* MSG_WIFI_UP   = "Wifi Client Connected";
      constexpr const char* MSG_WIFI_DOWN = "Wifi Client disconnected";
      constexpr const char* MSG_CHECKWIFI = "Check Wifi";

      char wifi_ssid[33];
      char wifi_pass[64];

      const uint32_t max_reconnect_attempts = 10;
      uint32_t connect_attempts = 0;
      bool connected = false;
      bool has_credentials = ( strcmp( WIFI_SSID, "YOUR WIFI SSID" )!=0 && strcmp( WIFI_PASSWORD, "YOUR WIFI PASSWORD" )!=0 );
      bool setup_done = false;
      const char* message = MSG_CHECKWIFI;


      void setup()
      {
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


      // Send CI-V Command by Wifi
      bool sendCommand( char *request, size_t request_size, char *resp, uint8_t response_size )
      {
        if( !available() ) {
          log_e("No WiFi :-(");
          return false; // wifi must be connected
        }
        using namespace wifi;

        bool ret = false;

        HTTPClient http;

        http.setTimeout(1);           // HTTPClient.h => setTimeout(uint16_t timeout) set the timeout (seconds) for the incoming connection
        http.setConnectTimeout(1000); // HTTPClient.h => setConnectTimeout(int32_t connectTimeout) set the timeout (milliseconds) outgoing connection
        civClient.setTimeout( 5 );    // WiFiClient.h => setTimeout(uint32_t seconds) set the timeout for the client waiting for incoming data

        String command = "";
        char hexStr[4];

        for (uint8_t i = 0; i < request_size; i++) {
          sprintf(hexStr, "%02x,", request[i]);
          command += String(hexStr);
        }

        command += BAUDE_RATE + String(",") + SERIAL_DEVICE;

        http.begin(civClient, PROXY_URL + String(":") + PROXY_PORT + String("/") + String("?civ=") + command);
        http.addHeader("User-Agent", USER_AGENT );
        http.addHeader("Connection", "keep-alive");

        int httpCode = http.GET();

        log_i("Sent proxy request: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", request[0], request[1], request[2], request[3], request[4], request[5], request[6] );

        if (httpCode == 200) {
          proxy::setFlag( PROXY_ONLINE );

          String response = http.getString(); // Get data
          response.trim();
          response = response.substring(4);

          if (response == "") {
            errors_count++;
              log_i("Got empty response");
            if( errors_count > max_errors ) {
              proxy::setFlag( TX_OFFLINE );
            }
          } else {
            proxy::setFlag( TX_ONLINE );
            wifi::message = nullptr;
            ret = true;
            errors_count = 0;

            for (uint8_t i = 0; i < response_size; i++) {
              resp[i] = strtol( response.substring(i * 2, (i * 2) + 2).c_str(), NULL, 16);
            }

            #if DEBUG==1
              Serial.println("-----\n" + response + " " + String( response.length() ) ) ;
              for (uint8_t i = 0; i < response_size; i++) {
                Serial.print( String( int(resp[i]) ) + " " );
              }
              Serial.println(" \n-----\n");
            #endif
            log_i("Got response: %s", response.c_str() );

          }
        } else {

          errors_count++;

          String response = http.getString(); // Get data

          if( httpCode == 404 ) {
            log_i("awww this proxy is not doing well (HTTP Error Code: %d, response: %s", httpCode, response.c_str() );
            proxy::setFlag( PROXY_ONLINE );
            proxy::setFlag( TX_OFFLINE );
          } else {
            // http timeout ?
            log_i("Got unknown status response: %d", httpCode );
            if( errors_count > max_errors ) {
              wifi::message = proxy::MSG_CHECKPROXY;
              proxy::setFlag( TX_OFFLINE );
            }
          }
        }
        http.end(); // Free the resources

        return ret;
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
            screenshot::setupAsync();
            connect_attempts = 0;
            wifi::message = proxy::MSG_CHECKPROXY;
          break;
          case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println( MSG_WIFI_DOWN );
            wifi::connected = false;
            wifi::message = MSG_CHECKWIFI;
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
