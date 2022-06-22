#line 1 "/home/runner/work/ICSMeter/ICSMeter/include/net/services/WebClient.cpp"
#include "../../assets/index_html.hpp"
#include "../../assets/images.hpp"
#include "../../UI/Themes/Themes.hpp"
#include "../Daemon.hpp"

namespace ICSMeter
{

  namespace net
  {

    namespace WebClient
    {
      using namespace proxy;
      using namespace daemon;


      #define QUOTE_X(t)#t
      #define QUOTE(t)QUOTE_X(t)
      #define subscribe_url PROXY_URL ":" QUOTE(PROXY_PORT) "/subscribe"
      #define command_url   PROXY_URL ":" QUOTE(PROXY_PORT) "/?civ="

      // called by WiFi event STA_GOT_IP
      void setup()
      {
        log_i("Checking WiFi");
        while( !wifi::available() ) {
          // TODO: timeout this
          vTaskDelay(1);
        }
        log_i("Checking server");
        while( !WebServer::isRunning ) {
          // TODO: timeout this
          vTaskDelay(1);
        }
        log_i("Running subscriptions");
        WebClient::has_subscriptions = subscribeAll();
        if( !WebClient::has_subscriptions ) {
          log_i("Proxy does not support websockets or is down");
          proxy::setFlag( PROXY_OFFLINE );
        } else {
          proxy::setFlag( PROXY_ONLINE );
        }
      }



      bool subscribeAll()
      {
        if( !WebServer::isRunning ) {
          log_e("Ignoring attempt to start subscriptions while webserver is not running");
          return false;
        }

        int succeeded = 0;

        for( int i=0; i<proxy::subscriptions_count; i++ ) {
          succeeded += WebClient::subscribe( proxy::subscriptions[i] ) ? 1 : 0;
        }
        bool ret = succeeded == proxy::subscriptions_count;
        if( !ret ) {
          log_e("Subscriptions count mismatch: got %d, was expecting %d", succeeded, proxy::subscriptions_count );
        } else {
          log_e("Subscriptions complete: got %d threads", proxy::subscriptions_count );
        }
        return ret;
      }


      bool subscribe( civ_subscription_t *sub )
      {
        if( !wifi::available() ) {
          log_e("No WiFi :-(");
          return false; // wifi must be connected
        }
        bool ret = false;

        HTTPClient http;
        WiFiClient civClient;

        //http.setTimeout(1);           // HTTPClient.h => setTimeout(uint16_t timeout) set the timeout (seconds) for the incoming connection
        //http.setConnectTimeout(1000); // HTTPClient.h => setConnectTimeout(int32_t connectTimeout) set the timeout (milliseconds) outgoing connection

        http.begin( civClient, subscribe_url );
        http.addHeader("User-Agent", USER_AGENT );

        String requestStr = "";
        CIV::buildRequest( sub->command, &requestStr );

        char json[256]; // cheap substitute for JSONStaticBuffer :-)
        snprintf( json, 255, subscribe_json_format, SERIAL_DEVICE, String(BAUDE_RATE).c_str(), requestStr.c_str(), String(sub->polling_freq, 2).c_str(), WiFi.localIP().toString().c_str() );

        http.addHeader("Content-Type", "application/json");
        http.addHeader("Connection", "keep-alive");
        int httpResponseCode = http.POST( json );

        if( httpResponseCode == 200 ) {
          proxy::setFlag( PROXY_ONLINE );
          String response = http.getString(); // Get data
          response.trim();
          int sub_id = atoi( response.c_str() );
          if( response != "" && sub_id > -1 ) {
            sub->subscription_id = sub_id;
            sub->subscribed = true;
            log_i("Successfully subscribed to '%s', got subscription_id=%d", requestStr.c_str(), sub_id );
            ret = true;
          } else {
            // Python error, or old ICUSBProxy.py somehow responding to POST ?
            log_e("Failed to read subscription ID from response (resp=%s)", response.c_str() );
          }
        }

        http.end();
        return ret;
      }


      // Legacy function using plain HTTP to query the proxy
      // The new implementation uses websockets so this will be dead code soon
      bool sendCommand( char *request, size_t request_size, char *resp, uint8_t response_size )
      {
        if( !wifi::available() ) {
          log_e("No WiFi :-(");
          return false; // wifi must be connected
        }

        bool ret = false;

        HTTPClient http;
        WiFiClient civClient;

        //http.setTimeout(1);           // HTTPClient.h => setTimeout(uint16_t timeout) set the timeout (seconds) for the incoming connection
        //http.setConnectTimeout(1000); // HTTPClient.h => setConnectTimeout(int32_t connectTimeout) set the timeout (milliseconds) outgoing connection
        //civClient.setTimeout( 5 );    // WiFiClient.h => setTimeout(uint32_t seconds) set the timeout for the client waiting for incoming data

        String command = "";

        CIV::buildRequest( request, &command, "," );

        command += BAUDE_RATE + String(",") + SERIAL_DEVICE;

        http.begin( civClient, String( command_url ) + command );
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
              log_i("Got empty 200 response");
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

          if( httpCode == 404 || httpCode == 500 ) {
            log_i("awww this proxy is not doing well (HTTP Error Code: %d, response: %s", httpCode, response.c_str() );
            proxy::setFlag( PROXY_ONLINE );
            proxy::setFlag( TX_OFFLINE );
          } else {
            // http timeout ?
            log_i("Got unknown status response: %d", httpCode );
            if( errors_count > max_errors ) {
              wifi::message = proxy::MSG_CHECKPROXY;
              proxy::setFlag( PROXY_OFFLINE );
              proxy::setFlag( TX_OFFLINE );
            }
          }
        }
        http.end(); // Free the resources

        return ret;
      }




    };

  };

};
