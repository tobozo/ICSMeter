#pragma once

// #define SDU_HEADLESS // saves a few bytes but disables the progress bar when a binary is loading
#define GZIP_BINLOADER // enables updates from gzipped binaries

#define BT  1
#define USB 2

#include "../settings.h"

#if !defined FASTLED_INTERNAL
  #define FASTLED_INTERNAL // To disable pragma messages on compile
#endif

#include <FastLED.h>
#include <Preferences.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SD.h>
#include <FS.h>
#include <SPIFFS.h>
#include <M5Unified.h>

#if IC_CONNECT==BT && IC_MODEL==705
  #include <BluetoothSerial.h>
#endif

#if defined GZIP_BINLOADER
  #define DEST_FS_USES_SPIFFS
  #define DEST_FS_USES_SD
  #include <ESP32-targz.h>
#endif

#include <M5StackUpdater.h>

#define DEBUG              1
#define VERSION            "0.1.5"
#define AUTHOR             "F4HWN"
#define NAME               "ICSMeter"
#define REPO_URL           "https://github.com/armel/ICSMeter"

#define APP_TITLE          NAME " V" VERSION  " by " AUTHOR

#define TIMEOUT_BIN_LOADER 3 // duration (seconds) of binloader screen on boot

#define NUM_LEDS           10
#define Neopixel_PIN       32 // 21
#define NUM_LEDS_STRIP     30
