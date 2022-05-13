#pragma once

// #define SDU_HEADLESS // saves a few bytes but disables the progress bar when a binary is loading

#define GZIP_BINLOADER // enables updates from gzipped binaries

#include "../settings.h"

#include <Preferences.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SD.h>
#include <FS.h>
#include <SPIFFS.h>
#include <M5Unified.h>


#define DEBUG 0
#define BT    1
#define USB   2

#if IC_CONNECT==BT && IC_MODEL==705

  #if defined ARDUINO_PARTITION_default

  #endif



  #include <BluetoothSerial.h>
#endif

#if defined GZIP_BINLOADER
  #define DEST_FS_USES_SPIFFS
  #define DEST_FS_USES_SD
  #include <ESP32-targz.h>
#endif

#include <M5StackUpdater.h>



#define VERSION  "0.1.5"
#define AUTHOR   "F4HWN"
#define NAME     "ICSMeter"
#define REPO_URL "https://github.com/armel/ICSMeter"



