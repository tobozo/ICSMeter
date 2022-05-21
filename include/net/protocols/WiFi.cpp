#include "../Controls.hpp"

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

      const uint32_t max_reconnect_attempts = 10;
      uint32_t connect_attempts = 0;
      bool connected = false;


      void setup()
      {
        WiFi.onEvent(WiFiEvent);
        if( strcmp( WIFI_SSID, "YOUR WIFI SSID" )==0 || strcmp( WIFI_SSID, "YOUR WIFI PASSWORD" )==0 ) {
          WiFi.begin();
          log_d("No credentials provided in settings.h, using previously saved tokens, if any...");
        } else {
          WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        }
      }


      bool available()
      {
        return connected;
      }


      // Send CI-V Command by Wifi
      bool sendCommand(char *request, size_t n, char *buffer, uint8_t limit)
      {
        if( !available() ) return false; // wifi must be connected
        using namespace wifi;

        log_d("Sending proxy request");
        bool ret = false;

        HTTPClient http;

        http.setTimeout(1);          // HTTPClient.h => setTimeout(uint16_t timeout) set the timeout (seconds) for the incoming connection
        http.setConnectTimeout(300); // HTTPClient.h => setConnectTimeout(int32_t connectTimeout) set the timeout (milliseconds) outgoing connection
        civClient.setTimeout( 1 );   // WiFiClient.h => setTimeout(uint32_t seconds) set the timeout for the client waiting for incoming data

        String command = "";
        char hexStr[4];

        for (uint8_t i = 0; i < n; i++) {
          sprintf(hexStr, "%02x,", request[i]);
          command += String(hexStr);
        }

        command += BAUDE_RATE + String(",") + SERIAL_DEVICE;

        http.begin(civClient, PROXY_URL + String(":") + PROXY_PORT + String("/") + String("?civ=") + command);
        http.addHeader("User-Agent", USER_AGENT );
        http.addHeader("Connection", "keep-alive");

        uint16_t httpCode = http.GET();

        if (httpCode == 200) {
          proxy::setFlag( PROXY_ONLINE );

          String response = http.getString(); // Get data
          response.trim();
          response = response.substring(4);

          if (response == "") {
            proxy::setFlag( TX_OFFLINE );
            errors_count++;
          } else {
            proxy::setFlag( TX_ONLINE );
            ret = true;
            errors_count = 0;

            for (uint8_t i = 0; i < limit; i++) {
              buffer[i] = strtol( response.substring(i * 2, (i * 2) + 2).c_str(), NULL, 16);
            }

            #if DEBUG==1
              Serial.println("-----\n" + response + " " + String( response.length() ) ) ;
              for (uint8_t i = 0; i < limit; i++) {
                Serial.print( String( int(buffer[i]) ) + " " );
              }
              Serial.println(" \n-----\n");
            #endif
          }
        } else {
          log_d("awww this proxy is not doing well :-(");
          errors_count++;
          if (errors_count > max_errors) {
            proxy::setFlag( TX_OFFLINE );
            proxy::setFlag( PROXY_OFFLINE );
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
            connected = true;
            screenshot::setup();
            connect_attempts = 0;
          break;
          case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println( MSG_WIFI_DOWN );
            connected = false;
            if( ++connect_attempts <= max_reconnect_attempts ) { // TODO: reset connect_attempts from settings
              setup();
            }
          break;
          default: break;
        }
      }

    };

  };

};
