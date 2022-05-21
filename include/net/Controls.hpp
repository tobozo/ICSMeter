#pragma once

#include "../core/ICSMeter.hpp"

namespace ICSMeter
{

  namespace net
  {

    static const char *message = nullptr; // connection message
    constexpr const char* USER_AGENT = "M5Stack";


    namespace daemon
    {
      void setup();
      void netTask(void *pvParameters);
      void check();
      void ICScan();
      bool connected();

      bool hasBluetooth();
      bool needsPairing();
      bool needsWiFiChecked();
      bool needsProxyChecked();
      bool canSleep();
      bool canWakeup();

      bool dispatchCommand(char *request, size_t n, char *buffer, uint8_t limit); // Send CI-V Command dispatcher

      // /!\ verbs are UI relative
      void setDataMode();  // counter/label values
      void setFrequency(); // counter/label values
      void setSmeter();    // needle values
      void setSWR();       // needle values
      void setPower();     // needle values
      void setMode();      // filter and mode

      void clearData();
    };


    namespace CIV
    {
      typedef struct command_t { char bytes[3]; } command_t;

      struct GaugeMeasure_t
      {
        float angle;
        String label;
      };

      struct FilterMode_t
      {
        String filter;
        String mode;
      };
      // /!\ verbs are IC relative
      uint8_t getTX();
      char getDataMode();  // counter/label values
      // gauge measures
      GaugeMeasure_t getSmeter();
      GaugeMeasure_t getSWR();
      GaugeMeasure_t getPower();
      // filter and mode
      FilterMode_t getMode();
      String getFrequency(); // counter/label values
    };


    namespace screenshot
    {
      void setup();
      void M5Screen24bmp( WiFiClient *client ); // Get screenshot
      void check(); // manage screenshot
      void handle404();
      void handleRoot();
      void sendFavicon();
    };


    namespace wifi
    {
      void setup();
      bool available();
      void WiFiEvent(WiFiEvent_t event);
      bool sendCommand(char *request, size_t n, char *buffer, uint8_t limit);
    };


    namespace bluetooth
    {
      void setup();
      bool available();
      bool sendCommand(char *request, size_t n, char *buffer, uint8_t limit);
    };


    typedef enum proxy_flags_t
    {
      PROXY_ONLINE,
      PROXY_OFFLINE,
      TX_ONLINE,
      TX_OFFLINE
    } proxy_flags;

    namespace proxy
    {
      void setup();
      bool available();
      bool connected();
      void setFlag( proxy_flags_t flag );
      uint32_t errors_count = 0;
      const uint32_t max_errors = 100; // mark the proxy as offline when this threshold is reached
      const char* checkStatus();
    };


  };

};



