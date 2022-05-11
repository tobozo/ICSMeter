#pragma once

#include "../../main/core.h"

#include "Bluetooth.hpp"
#include "WiFi.hpp"


namespace ICSMeter
{

  namespace net
  {
    uint8_t htmlGetRequest;

    bool proxyConnected = false;
    bool txConnected    = true;

    bool startup        = true;
    bool wakeup         = true;

    bool reset          = true;

    bool screenshot     = false;

    char dataMode       = 0;

    void setupNetwork();
    bool checkConnection(); // Manage connexion error

    void sendCommand(char *request, size_t n, char *buffer, uint8_t limit); // Send CI-V Command dispatcher

    uint8_t getTX(); // Get TX
    void getSmeter(); // Get Smeter
    void getSWR(); // Get SWR
    void getPower(); // Get Power
    void getDataMode(); // Get Data Mode
    void getFrequency(); // Get Frequency
    void getMode(); // Get Mode

    bool M5Screen24bmp(); // Get screenshot
    void checkScreenshot(); // manage screenshot

  };

};



