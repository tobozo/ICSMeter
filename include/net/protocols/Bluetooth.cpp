#include "../Controls.hpp"

#if IC_CONNECT==BT && IC_MODEL==705

namespace ICSMeter
{

  namespace net
  {

    BluetoothSerial CAT; // Bluetooth connector

    namespace bluetooth
    {

      using namespace daemon;

      constexpr const char * ERR_BT_INIT = "An error occurred initializing Bluetooth";
      constexpr const char * MSG_BT_INIT = "Bluetooth initialized";

      constexpr const char* MSG_BT_CONNECTED    = "BT Client Connected";
      constexpr const char* MSG_BT_DISCONNECTED = "BT Client disconnected";

      bool connected;


      void BluetoothEvent(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

      void setup()
      {
        CAT.register_callback( BluetoothEvent );

        if (!CAT.begin(NAME)) {
          Serial.println( ERR_BT_INIT );
        } else {
          Serial.println( MSG_BT_INIT );
        }
      }


      bool available()
      {
        return connected;
      }


      // Send CI-V Command by Bluetooth
      bool sendCommand(char *request, size_t n, char *buffer, uint8_t limit)
      {
        //if( proxy::errors_count > proxy::max_errors ) return;
        uint8_t bytes[3];
        uint8_t bytes_count = 0;

        // /!\ while()/while()/()while ladder is bad for binary size !
        // using for() loop is less prone to scalar expansion

        while( bytes_count != limit ) {
          for( uint8_t i=0; i<n; i++ ) {
            CAT.write(request[i]);
          }

          vTaskDelay(100);

          while( CAT.available() ) {
            bytes[0] = CAT.read();
            bytes[1] = CAT.read();

            if( bytes[0] == 0xfe && bytes[1] == 0xfe ) {
              bytes_count = 0;
              bytes[2] = CAT.read();

              while( bytes[2] != 0xfd ) {
                buffer[bytes_count] = bytes[2];
                bytes[2] = CAT.read();
                bytes_count++;
                if( bytes_count > limit ) {
                  #if DEBUG==1
                    Serial.print(" Overflow");
                  #endif
                  return false;
                  break;
                }
              }
            }
          }
          proxy::setFlag( PROXY_ONLINE ); // probably not necessary
        }
        return online;
        // Serial.println(" Ok");
      }



      // Bluetooth callback
      void BluetoothEvent(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
      {
        if (event == ESP_SPP_SRV_OPEN_EVT) {
          connected = true;
          Serial.println( MSG_BT_CONNECTED );
        }
        if (event == ESP_SPP_CLOSE_EVT) {
          tft.sleep();
          ScreenSaver::wakeup = false;
          connected = false;
          Serial.println( MSG_BT_DISCONNECTED );
        }
      }


    };
  };

};

#endif
