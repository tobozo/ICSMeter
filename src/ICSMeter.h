// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#define VERSION "0.1.3"
#define AUTHOR "F4HWN"
#define NAME "ICSMeter"

#include <Preferences.h>
#include <FastLED.h>
#include <HTTPClient.h>
#include <SD.h>
#include <FS.h>
#include <SPIFFS.h>
#include <M5Unified.h>
#include <BluetoothSerial.h>
#include <M5StackUpdater.h>

#define BT 1
#define USB 2

#define TIMEOUT_BIN_LOADER 3 // 3 sec
#define DEBUG 0

// Color
#define TFT_MODE_BORDER M5.Lcd.color565(115, 135, 159)
#define TFT_MODE_BACK M5.Lcd.color565(24, 57, 92)

#define TFT_MENU_BORDER M5.Lcd.color565(115, 135, 159)
#define TFT_MENU_BACK M5.Lcd.color565(24, 57, 92)
#define TFT_MENU_SELECT M5.Lcd.color565(255, 255, 255)

// Needle
#define TFT_BACK M5.Lcd.color565(255, 248, 236)
#define TFT_MODE M5.Lcd.color565(84, 103, 143)
#define TFT_NEDDLE_1 M5.Lcd.color565(241, 120, 100)
#define TFT_NEDDLE_2 M5.Lcd.color565(241, 170, 170)

// Web site Screen Capture stuff
#define GET_unknown 0
#define GET_index_page  1
#define GET_screenshot  2

// LED
#define FASTLED_INTERNAL // To disable pragma messages on compile
#define NUM_LEDS 10
CRGB leds[NUM_LEDS];

// Bluetooth connector
BluetoothSerial CAT;

// Preferences
Preferences preferences;

// Global Variables
WiFiServer httpServer(80);
WiFiClient httpClient, civClient;

int8_t measure = 1;
int8_t beep = 0;
int8_t transverter = 0;
int8_t screensaver = 0;
uint8_t brightness = 64;
uint8_t htmlGetRequest;

uint32_t screensaverTimer;

float angleOld = 0;

boolean reset = true;
boolean screenshot = false;
boolean screensaverMode = false;
boolean settingsMode = false;
boolean settingLock = true;
boolean btConnected = false;
boolean wifiConnected = false;
boolean proxyConnected = false;
boolean txConnected = true;
boolean startup = true;
boolean batteryCharginglOld = true;

int8_t measureOld = 5;
uint8_t sOld = 255;
uint8_t SWROld = 255;
uint8_t powerOld = 255;
uint8_t batteryLevelOld = 0;

String frequencyOld = "";
String filterOld = "";
String modeOld = "";
String valStringOld = "";
String subValStringOld = "";

char dataMode = 0;

// Flags for button presses via Web site Screen Capture
bool buttonLeftPressed = false;
bool buttonCenterPressed = false;
bool buttonRightPressed = false;

// Bin loader
File root;
String binFilename[128];
uint8_t binIndex = 0;

// Menu
const char *settings[] = {"Measured Values", "Transverter Mode", "Brightness", "Beep", "Screensaver", "IP Address", "Shutdown", "Exit"};
const char *choiceMeasures[] = {"PWR", "S", "SWR"};
const char *choiceBrightness[] = {"BRIGHTNESS"};
const char *choiceTransverter[] = {"OFF", "ON"};
const char *choiceBeep[] = {"BEEP LEVEL"};
const char *choiceScreensaver[] = {"TIMEOUT"};