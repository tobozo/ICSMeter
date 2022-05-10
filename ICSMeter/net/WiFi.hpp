#pragma once

#include "../main/core.h"

namespace ICSMeter
{
  namespace net
  {

    WiFiServer httpServer(80);
    WiFiClient httpClient, civClient;
    boolean wifiConnected = false;

    namespace wifi
    {
      // Send CI-V Command by Wifi
      void sendCommandWifi(char *request, size_t n, char *buffer, uint8_t limit);
      // Wifi callback On
      void callbackWifiOn(WiFiEvent_t event, WiFiEventInfo_t info);
      // Wifi callback Off
      void callbackWifiOff(WiFiEvent_t event, WiFiEventInfo_t info);
    };

  };

};
