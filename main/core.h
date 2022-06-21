#pragma once

// #define SDU_HEADLESS // saves a few bytes but disables the progress bar when a binary is loading
#define GZIP_BINLOADER // enables updates from gzipped binaries
#define USE_LITTLEFS   // use LittleFS rather than SPIFFS (faster, more reliable)

#define BT  1
#define USB 2

#include "../settings.h"

#if !defined FASTLED_INTERNAL
  #define FASTLED_INTERNAL // To disable pragma messages on compile
#endif

// filesystem

#if defined USE_LITTLEFS
  #include <LittleFS.h>
  #define FLASH_FS LittleFS     // for Updater.cpp
  #define DEST_FS_USES_LITTLEFS // for esp32-targz
#else
  #include <SPIFFS.h>
  #define FLASH_FS SPIFFS       // for Updater.cpp
  #define DEST_FS_USES_SPIFFS   // for esp32-targz
#endif

#include <SD.h>
#include <FS.h>

// GFX

#include <M5Unified.h>
#include <LGFXMeter.h>

// modules/services

#include <FastLED.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <ESPAsyncWebServer.h>
#include <BluetoothSerial.h>

#if defined GZIP_BINLOADER
  #define DEST_FS_USES_SD
  #include <ESP32-targz.h>
#endif

#include <M5StackUpdater.h>

#define DEBUG              0
#define VERSION            "0.1.7"
#define AUTHOR             "F4HWN"
#define NAME               "ICSMeter"
#define REPO_URL           "https://github.com/armel/ICSMeter"

#define APP_TITLE          NAME " V" VERSION  " by " AUTHOR

#define TIMEOUT_BIN_LOADER 3 // duration (seconds) of binloader screen on boot

#define NUM_LEDS           10
#define Neopixel_PIN       32 // 21
#define NUM_LEDS_STRIP     30

//#define DEMO_MODE // this is for debugging or demonstrating the needle, don't use in production!

enum IC_COMM_TYPE_t
{
  IC_COMM_WIFI,
  IC_COMM_BLUETOOTH
};

enum IC_MODEL_t
{
  IC7300 = 7300,
  IC9700 = 9700,
  IC705  = 705
};

struct IC_DAEMON_t
{
  const char* label; // text label
  IC_MODEL_t model;  // IC Model
  uint8_t id;
  IC_COMM_TYPE_t type;
};


IC_DAEMON_t IC_COMBOS[] =
{
  { "IC7300 (WiFi)",     IC7300, 0xA4, IC_COMM_WIFI },
  { "IC9700 (WiFi)",     IC9700, 0xA4, IC_COMM_WIFI },
  { "IC705 (WiFi)",      IC705,  0xA4, IC_COMM_WIFI },
  { "IC705 (Bluetooth)", IC705,  0xA4, IC_COMM_BLUETOOTH },
};
