#pragma once

#include "../main/core.h"

namespace ICSMeter
{

  namespace net
  {

    // Bluetooth connector
    BluetoothSerial CAT;
    boolean btConnected = false;

    namespace bluetooth
    {
      // Send CI-V Command by Bluetooth
      void sendCommandBt(char *request, size_t n, char *buffer, uint8_t limit);
      // Bluetooth callback
      void callbackBT(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
    };
  };

};
