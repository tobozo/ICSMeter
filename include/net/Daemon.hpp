#pragma once

#include "../core/ICSMeter.hpp"

namespace ICSMeter
{

  namespace net
  {

    constexpr const char* USER_AGENT = "M5Stack";


    namespace daemon
    {
      void setup();
      void loop();
      void netTask(void *pvParameters);
      void check();
      void ICScan();
      bool connected();

      bool hasBluetooth();
      bool needsPairing();
      bool needsWiFiChecked();
      bool needsProxyChecked();

      bool dispatchCommand(char *request, size_t n, char *buffer, uint8_t limit); // Send CI-V Command dispatcher

      // /!\ verbs are UI relative
      void setDataMode();  // counter/label values
      void setFrequency(); // counter/label values
      void setSmeter();    // needle values
      void setSWR();       // needle values
      void setPower();     // needle values
      void setMode();      // filter and mode

      void clearData();

      static const char *message = "Connecting"; // connection message
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
      void begin();
      bool available();
      bool saveSSID( const char* new_ssid );
      bool savePass( const char* new_pass );
      bool saveCredentials( const char* new_wifi_ssid, const char* new_wifi_pass );
      bool loadCredentials( char* dest_wifi_ssid, char* dest_wifi_pass );
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



