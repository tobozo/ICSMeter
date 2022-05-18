#pragma once

#include "../core/ICSMeter.hpp"


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

    void setup();
    void ICScan();
    bool connected(); // Manage connexion error

    void sendCommand(char *request, size_t n, char *buffer, uint8_t limit); // Send CI-V Command dispatcher

    // verbs from getters and setters are UI relative (not IC relative unless specified)

    uint8_t getTX();     // get last response from IC
    void setDataMode();  // counter/label values
    void setFrequency(); // counter/label values

    // needle values
    void setSmeter();
    void setSWR();
    void setPower();

    // filter and mode
    void setMode();

    namespace screenshot
    {
      bool M5Screen24bmp(); // Get screenshot
      void check(); // manage screenshot
    };


    namespace wifi
    {
      bool connected = false;
      // Send CI-V Command by Wifi
      void sendCommandWifi(char *request, size_t n, char *buffer, uint8_t limit);
      // Wifi callback On
      void callbackWifiOn(WiFiEvent_t event, WiFiEventInfo_t info);
      // Wifi callback Off
      void callbackWifiOff(WiFiEvent_t event, WiFiEventInfo_t info);
    };


    namespace bluetooth
    {
      bool connected = false;
      // Send CI-V Command by Bluetooth
      void sendCommandBt(char *request, size_t n, char *buffer, uint8_t limit);
      // Bluetooth callback
      void begin();
    };


  };

};



