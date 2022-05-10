#pragma once

#include "../main/core.h"

#include "Bluetooth.hpp"
#include "WiFi.hpp"


namespace ICSMeter
{

  namespace net
  {

    uint8_t htmlGetRequest;
    boolean proxyConnected = false;
    boolean txConnected = true;
    boolean startup = true;
    boolean wakeup = true;

    // Flags for button presses via Web site Screen Capture
    bool buttonLeftPressed = false;
    bool buttonCenterPressed = false;
    bool buttonRightPressed = false;

    uint32_t screensaverTimer;
    boolean screensaverMode = false;

    boolean reset = true;
    char dataMode = 0;

    // Send CI-V Command dispatcher
    void sendCommand(char *request, size_t n, char *buffer, uint8_t limit);

    // Get Smeter
    void getSmeter();

    // Get SWR
    void getSWR();

    // Get Power
    void getPower();

    // Get Data Mode
    void getDataMode();

    // Get Frequency
    void getFrequency();

    // Get Mode
    void getMode();

    // Get TX
    uint8_t getTX();

    // Manage connexion error
    boolean checkConnection();

    bool M5Screen24bmp();

    // Get screenshot
    void getScreenshot();


  };

};



