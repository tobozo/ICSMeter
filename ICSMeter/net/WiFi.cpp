#include "WiFi.hpp"
#include "Controls.hpp"

namespace ICSMeter
{
  namespace net
  {

    namespace wifi
    {

      // Send CI-V Command by Wifi
      void sendCommandWifi(char *request, size_t n, char *buffer, uint8_t limit)
      {
        static uint8_t proxyError = 0;

        HTTPClient http;
        uint16_t httpCode;

        String command = "";
        String response = "";

        char s[4];

        for (uint8_t i = 0; i < n; i++)
        {
          sprintf(s, "%02x,", request[i]);
          command += String(s);
        }

        command += BAUDE_RATE + String(",") + SERIAL_DEVICE;

        http.begin(civClient, PROXY_URL + String(":") + PROXY_PORT + String("/") + String("?civ=") + command); // Specify the URL
        http.addHeader("User-Agent", "M5Stack");                                                               // Specify header
        http.addHeader("Connection", "keep-alive");                                                            // Specify header
        http.setTimeout(100);                                                                                  // Set Time Out
        httpCode = http.GET();                                                                                 // Make the request

        if (httpCode == 200)
        {
          proxyConnected = true;
          proxyError = 0;

          response = http.getString(); // Get data
          response.trim();
          response = response.substring(4);

          if (response == "")
          {
            txConnected = false;
          }
          else
          {
            txConnected = true;
            startup = false;

            for (uint8_t i = 0; i < limit; i++)
            {
              buffer[i] = strtol(response.substring(i * 2, (i * 2) + 2).c_str(), NULL, 16);
            }

            if (DEBUG)
            {
              Serial.println("-----");
              Serial.print(response);
              Serial.print(" ");
              Serial.println(response.length());

              for (uint8_t i = 0; i < limit; i++)
              {
                Serial.print(int(buffer[i]));
                Serial.print(" ");
              }
              Serial.println(" ");
              Serial.println("-----");
            }
          }
        }
        else
        {
          proxyError++;
          if (proxyError > 10)
          {
            proxyError = 10;
            proxyConnected = false;
          }
        }
        http.end(); // Free the resources
      }


      // Wifi callback On
      void callbackWifiOn(WiFiEvent_t event, WiFiEventInfo_t info)
      {
        wifiConnected = true;
        Serial.println("Wifi Client Connected");
      }

      // Wifi callback Off
      void callbackWifiOff(WiFiEvent_t event, WiFiEventInfo_t info)
      {
        wifiConnected = false;
        Serial.println("Wifi Client disconnected");

        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      }


    };
  };
};
