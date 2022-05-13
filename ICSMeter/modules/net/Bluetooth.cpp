#include "Controls.hpp"

namespace ICSMeter
{

  namespace net
  {

    namespace bluetooth
    {

      // Send CI-V Command by Bluetooth
      void sendCommandBt(char *request, size_t n, char *buffer, uint8_t limit)
      {
        uint8_t byte1, byte2, byte3;
        uint8_t counter = 0;

        while (counter != limit)
        {
          for (uint8_t i = 0; i < n; i++)
          {
            CAT.write(request[i]);
          }

          vTaskDelay(100);

          while (CAT.available())
          {
            byte1 = CAT.read();
            byte2 = CAT.read();

            if (byte1 == 0xFE && byte2 == 0xFE)
            {
              counter = 0;
              byte3 = CAT.read();
              while (byte3 != 0xFD)
              {
                buffer[counter] = byte3;
                byte3 = CAT.read();
                counter++;
                if (counter > limit)
                {
                  #if DEBUG==1
                    Serial.print(" Overflow");
                  #endif
                  break;
                }
              }
            }
          }
          startup = false;
        }
        // Serial.println(" Ok");
      }


      constexpr const char* MSG_BT_CONNECTED    = "BT Client Connected";
      constexpr const char* MSG_BT_DISCONNECTED = "BT Client disconnected";


      // Bluetooth callback
      void callbackBT(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
      {
        if (event == ESP_SPP_SRV_OPEN_EVT)
        {
          connected = true;
          Serial.println( MSG_BT_CONNECTED );
        }
        if (event == ESP_SPP_CLOSE_EVT)
        {
          tft.sleep();
          wakeup = false;
          connected = false;
          Serial.println( MSG_BT_DISCONNECTED );
        }
      }


    };
  };

};

