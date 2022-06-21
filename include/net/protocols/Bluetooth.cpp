#include "../Daemon.hpp"

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
      comm_struct_t agent = { bluetooth::setup, bluetooth::loop, bluetooth::available, bluetooth::sendCommand, bluetooth::message };


      void BluetoothEvent(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);


      void setup()
      {
        daemon::message = MSG_NEEDPAIRING;
        CAT.register_callback( BluetoothEvent );

        if (!CAT.begin(NAME)) {
          Serial.println( ERR_BT_INIT );
        } else {
          Serial.println( MSG_BT_INIT );
        }
      }


      void loop()
      {
        static uint32_t retrytimer = millis();
        static uint32_t retries = 50;

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
              // retry any command
              CIV::setRequest( CIV::CIV_CHECK );
              sendCommand(CIV::request, 6, CIV::buffer, 6);
              retrytimer = millis();
              retries--;
            }
          }
        } else {
          retries = 50;
          retrytimer = millis();
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
          CIV::last_poll = millis();
          return true;
      }



      // Bluetooth callback
      void BluetoothEvent(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
      {
        if (event == ESP_SPP_SRV_OPEN_EVT) {
          proxy::setFlag( PROXY_ONLINE );
          bluetooth::connected = true;
          Serial.println( MSG_BT_CONNECTED );
          daemon::message = nullptr;
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
          daemon::message = MSG_NEEDPAIRING;
        } else {
          Serial.printf("evt: %d\n", event );
        }

      }

    };
  };

};

