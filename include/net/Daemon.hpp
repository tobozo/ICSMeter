#pragma once

#include "../core/ICSMeter.hpp"

namespace ICSMeter
{

  namespace net
  {

    constexpr const char* USER_AGENT = "M5Stack";

    struct civ_subscription_t;

    struct comm_struct_t
    {
      void (*setup)();
      void (*loop)();
      bool (*available)();
      bool (*sendCommand)( char *request, size_t request_size, char *response, uint8_t response_size );
      const char* message;
    };


    namespace daemon
    {
      void setup();
      void loop();
      void daemonTask(void *pvParameters);
      //void check();
      void ICScan();
      bool connected();
      comm_struct_t *agent = nullptr;
      bool dispatchCommand( char *request, size_t request_size, char *response, uint8_t response_size ); // Send CI-V Command dispatcher
      static const char *message = "Connecting"; // connection message
    };


    // protocols

    namespace CIV
    {
      typedef struct command_t { char bytes[3]; } command_t;

      constexpr const uint8_t CIV_NUL = 0x00; // Null byte
      // bytes 0 and 1
      constexpr const uint8_t CIV_HDR = 0xfe; // header byte
      // byte 2
                      uint8_t CIV_ADR;// = CI_V_ADDRESS; // CIV address
      // byte 3/4
      constexpr const uint8_t CIV_STA = 0xe0; // Read value  (needs byte 1)
      // byte 4
      constexpr const uint8_t CIV_CHK = 0x03; // ping IC
      constexpr const uint8_t CIV_GTX = 0x1c; // Send/read the transceiver’s status (00=RX, 01=TX)
      constexpr const uint8_t CIV_DAT = 0x1a; // Send/read the a setting
      constexpr const uint8_t CIV_MTR = 0x15; // Read X-meter (needs byte 1)
      // byte 5
      constexpr const uint8_t CIV_STX = 0x00; // CIV_GTX + TX Status
      constexpr const uint8_t CIV_DTM = 0x06; // CIV_DAT + setting "DATA MODE"
      constexpr const uint8_t CIV_SMT = 0x02; // CIV_MTR + S-meter level (0000=S0, 0120=S9, 0241=S9+60 dB)
      constexpr const uint8_t CIV_PWR = 0x11; // CIV_MTR + Po meter level (0000=0% ~ 0143=50% ~ 0213=100%)
      constexpr const uint8_t CIV_SWR = 0x12; // CIV_MTR + SWR meter level (0000=SWR1.0, 0048=SWR1.5, 0080=SWR2.0, 0120=SWR3.0)
      constexpr const uint8_t CIV_FRQ = 0x03; // CIV_STA + frequency ( 145.500.000 ... 434.975.000 )
      constexpr const uint8_t CIV_MOD = 0x04; // CIV_STA + mode (040501, 040502 or 040503 for FIL1+FM, FIL2+FM or FIL3+USB)
      // terminator byte
      constexpr const uint8_t CIV_END = 0xfd; // terminator

      // commands
      constexpr const command_t CIV_CHECK         = { CIV_CHK, CIV_END, CIV_NUL }; // ping IC
      constexpr const command_t CIV_GET_TX        = { CIV_GTX, CIV_STX, CIV_END }; // Send/read the transceiver’s status (00=RX, 01=TX)
      constexpr const command_t CIV_GET_DATA_MODE = { CIV_DAT, CIV_DTM, CIV_END }; // Send/read the DATA mode setting
      constexpr const command_t CIV_GET_SMETER    = { CIV_MTR, CIV_SMT, CIV_END }; // Read S-meter level (0000=S0, 0120=S9, 0241=S9+60 dB)
      constexpr const command_t CIV_GET_PWR       = { CIV_MTR, CIV_PWR, CIV_END }; // Read the Po meter level (0000=0% ~ 0143=50% ~ 0213=100%)
      constexpr const command_t CIV_GET_SWR       = { CIV_MTR, CIV_SWR, CIV_END }; // Read SWR meter level (0000=SWR1.0, 0048=SWR1.5, 0080=SWR2.0, 0120=SWR3.0)
      constexpr const command_t CIV_GET_FRQ       = { CIV_STA, CIV_FRQ, CIV_END }; // Read frequency ( 145.500.000 ... 434.975.000 )
      constexpr const command_t CIV_GET_MOD       = { CIV_STA, CIV_MOD, CIV_END }; // Read IC mode (040501, 040502 or 040503 for FIL1+FM, FIL2+FM or FIL3+USB)

      constexpr const command_t *CIV_COMMANDS[]   = { &CIV_CHECK, &CIV_GET_TX, &CIV_GET_DATA_MODE, &CIV_GET_SMETER, &CIV_GET_PWR, &CIV_GET_SWR, &CIV_GET_FRQ, &CIV_GET_MOD };

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


      struct SMeterPacket_t
      {
        uint8_t val0;
        float   val1;
        float   angle;
      };

      struct GaugeMap_t
      {
        uint8_t range_start;
        uint8_t range_end;
        float angle_start;
        float angle_end;
        float value_start;
        float value_end;
      };

      struct CIVStatus_t
      {
        char datamode  = 0;
        uint8_t filter = 0;
        uint8_t mode   = 0;
        uint8_t tx     = 0;
        uint8_t SMeter = 0;
        uint8_t SWR    = 0;
        uint8_t PWR    = 0;
        double freq    = 0;
      };

      CIVStatus_t status;
      bool txConnected = false;
      bool had_success = false;
      uint32_t last_poll = millis();
      uint32_t poll_timeout = 10000;

      extern char buffer[8];
      extern char request[7];

      void setup();
      void loop();
      void setICModel( uint8_t choice );
      void setICAddress( uint8_t address );
      void setIC( uint8_t choice, uint8_t address );
      int8_t IC_CHOICE = 0; // for settings menu, populated by prefs
      IC_DAEMON_t *IC = nullptr;

      uint8_t getDec( uint8_t b1, uint8_t b2, bool hex = true );
      void setRequest( command_t cmd );
      void buildRequest( char request[7], String *command, const char* separator="" );
      void buildRequest( const command_t *cmd, String *command, const char* separator="" );
      bool parse( String packet );
      bool parse( uint8_t* buffer, size_t buffer_size );

      // /!\ verbs are IC relative
      uint8_t getTX();
      char getDataMode();

      float getSmeterAngle();
      String getSmeterLabel();
      GaugeMeasure_t getSmeter();

      float getSWRAngle();
      String getSWRLabel();
      GaugeMeasure_t getSWR();

      float getPWRAngle();
      String getPWRLabel();
      GaugeMeasure_t getPower();

      String getFilterLabel();
      String getModeLabel();
      FilterMode_t getMode();

      String getFrequencyLabel();
      String getFrequency();
    };


    namespace wifi
    {
      void setup();
      void loop();
      void begin();
      bool available();
      bool saveSSID( const char* new_ssid );
      bool savePass( const char* new_pass );
      bool saveCredentials( const char* new_wifi_ssid, const char* new_wifi_pass );
      bool loadCredentials( char* dest_wifi_ssid, char* dest_wifi_pass );
      void WiFiEvent(WiFiEvent_t event);
    };


    namespace bluetooth
    {
      void setup();
      void loop();
      bool available();
      bool sendCommand( char *request, size_t request_size, char *response, uint8_t response_size );
    };


    // services


    typedef enum proxy_flags_t
    {
      PROXY_ONLINE,
      PROXY_OFFLINE,
      TX_ONLINE,
      TX_OFFLINE
    } proxy_flags;


    struct civ_subscription_t
    {
      const CIV::command_t *command;
      uint8_t response_size; // bytes
      float polling_freq;    // seconds
      uint32_t last_update;  // millis
      bool subscribed;
      uint32_t subscription_id;
    };


    namespace WebClient
    {
      void setup();
      bool subscribeAll();
      bool subscribe( civ_subscription_t *sub );
      bool sendCommand( char *request, size_t request_size, char *resp, uint8_t response_size );
      bool has_subscriptions = false;
      #if defined UPDATER_URL
        WiFiClient *getFirmwareClientPtr( WiFiClientSecure *client, const char* url, const char *cert = NULL );
        extern HTTPClient http;
        extern WiFiClientSecure client;
      #endif
    }

    namespace WebServer
    {
      void setupAsync();
      bool isRunning   = false;
      bool OTA_enabled = false;
      AsyncWebServer server(80);
      AsyncWebSocket ws("/ws");
      AsyncEventSource events("/events");
      AsyncWebSocketClient* wsClient;
    }


    namespace proxy
    {
      constexpr const char* MSG_CHECKTX     = "Check TX";
      constexpr const char* MSG_CHECKPROXY  = "Check Proxy";
      constexpr const char* MSG_TX_UP       = "TX connected";
      constexpr const char* MSG_TX_DOWN     = "TX disconnected";
      constexpr const char* MSG_MAX_ERRORS  = "Bad network";
      constexpr const char* MSG_LINK_DOWN   = "Link Down";

      void setup();
      bool available();
      bool connected();
      void setFlag( proxy_flags_t flag );
      uint32_t errors_count = 0;
      const uint32_t max_errors = 100; // mark the proxy as offline when this threshold is reached
      uint32_t check_frequency = 5000; // delay between end of last request and next request
      uint32_t last_check = millis();
      extern civ_subscription_t *subscriptions[8];
      extern size_t subscriptions_count;
    };



    namespace screenshot
    {
      void setup();
      void check(); // manage screenshot
      struct BMPStreamReader_t;
      BMPStreamReader_t *BMPStreamReader;
    };



  };

};



