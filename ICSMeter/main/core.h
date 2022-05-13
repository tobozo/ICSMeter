#pragma once

#define SDU_HEADLESS // saves a few bytes

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
  #include <BluetoothSerial.h>
#endif



#include <M5StackUpdater.h>



#define VERSION "0.1.5"
#define AUTHOR "F4HWN"
#define NAME "ICSMeter"



