#pragma once

#define SDU_HEADLESS // saves a few bytes

#include <Preferences.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SD.h>
#include <FS.h>
#include <SPIFFS.h>
#include <M5Unified.h>
#include <BluetoothSerial.h>
#include <M5StackUpdater.h>

#include "../settings.h"

#define VERSION "0.1.5"
#define AUTHOR "F4HWN"
#define NAME "ICSMeter"

#define DEBUG 0
#define BT    1
#define USB   2

