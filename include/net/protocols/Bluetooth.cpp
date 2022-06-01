#include "../Daemon.hpp"

#if IC_CONNECT==BT && IC_MODEL==705

namespace ICSMeter
{

  namespace net
  {

    BluetoothSerial CAT; // Bluetooth connector

    namespace bluetooth
    {

      using namespace daemon;
      using namespace UI;

      constexpr const char * ERR_BT_INIT         = "An error occurred initializing Bluetooth";
      constexpr const char * MSG_BT_INIT         = "Bluetooth initialized";
      constexpr const char * MSG_NEEDPAIRING     = "Need Pairing";
      constexpr const char * MSG_BT_CONNECTED    = "BT Client Connected";
      constexpr const char * MSG_BT_DISCONNECTED = "BT Client disconnected";

      bool connected;
      const char* message = MSG_NEEDPAIRING;


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
        return bluetooth::connected;
      }


      // Send CI-V Command by Bluetooth
      bool sendCommand(char *request, size_t request_size, char *response, uint8_t response_size )
      {
        // send request
        if( CAT.write( (uint8_t*)request, request_size ) != request_size ) {
          goto _tx_offline;
        }
        // give some time for the peer to respond
        vTaskDelay(100);
        // got response ?
        if( !CAT.available() ) {
          goto _tx_offline;
        }
        // read packet header
        uint8_t bytes[2];
        if( CAT.readBytes( bytes, 2 ) != 2 ) {
          goto _tx_offline;
        }
        // validate header
        if( bytes[0] != 0xfe || bytes[1] != 0xfe ) {
          goto _tx_offline;
        }
        // read packet data
        if( CAT.readBytesUntil( 0xfd, response, response_size ) == response_size ) {
          goto _tx_online;
        }
        //while( CAT.available() ) CAT.read(); // flush ?

        _tx_offline:
          proxy::setFlag( TX_OFFLINE );
          return false;

        _tx_online:
          proxy::setFlag( TX_ONLINE );
          return proxy::available();
      }



      // Bluetooth callback
      void BluetoothEvent(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
      {
        if (event == ESP_SPP_SRV_OPEN_EVT) {
          proxy::setFlag( PROXY_ONLINE );
          bluetooth::connected = true;
          Serial.println( MSG_BT_CONNECTED );
          bluetooth::message = nullptr;
        }
        else if (event == ESP_SPP_CLOSE_EVT) {
          proxy::setFlag( TX_OFFLINE );
          proxy::setFlag( PROXY_OFFLINE );
          if( bluetooth::connected ) {
            // IC was disconnected, go to sleep
            ScreenSaver::sleep();
          }
          bluetooth::connected = false;
          Serial.println( MSG_BT_DISCONNECTED );
          bluetooth::message = MSG_NEEDPAIRING;
        } else {
          Serial.printf("evt: %d\n", event );
        }

      }

    };
  };

};

#endif
