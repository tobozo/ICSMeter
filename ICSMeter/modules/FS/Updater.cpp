#include "Updater.hpp"

#define TIMEOUT_BIN_LOADER 3 // 3 sec


namespace ICSMeter
{


  namespace fs
  {

    // Bin loader
    static File root;
    static String binFilename[128];
    static uint8_t binIndex = 0;

    // List files on SPIFFS or SD
    void getBinaryList(File dir, String type) {
      while (true) {
        File entry = dir.openNextFile();
        if (!entry) {
          // no more files
          break;
        }

        if (strstr(entry.name(), "/.") == NULL && strstr(entry.name(), ".bin") != NULL) {
          // Serial.println(type + "_" + entry.name());
          binFilename[binIndex] = type + "_" + entry.name();
          binIndex++;
        }

        if (entry.isDirectory() && strstr(entry.name(), "/.") == NULL) {
          getBinaryList(entry, type);
        }

        entry.close();
      }
    }

    // Bin Loader
    void binLoader()
    {
      bool click = 0;
      int8_t cursor = 0;
      int8_t start = 0;
      int8_t stop = 0;
      int8_t limit = 8;
      int8_t change = 255;
      String tmpName;

      if (!SPIFFS.begin()) {

        Serial.println("SPIFFS Mount Failed");

        tft.setTextFont(1);
        tft.setTextSize(2);

        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextDatum(CC_DATUM);
        tft.drawString("Flash File System", 160, 20);
        tft.drawString("needs to be formated.", 160, 50);
        tft.drawString("It takes around 4 minutes.", 160, 100);
        tft.drawString("Please, wait until ", 160, 150);
        tft.drawString("the application starts !", 160, 180);

        Serial.println("SPIFFS Formating...");

        SPIFFS.format(); // Format SPIFFS...

        tft.setTextFont(0);
        tft.setTextSize(0);

        return;
      }

      root = SPIFFS.open("/");
      getBinaryList(root, "SP");

      if (SD.begin(GPIO_NUM_4, SPI, 25000000)) {
        root = SD.open("/");
        getBinaryList(root, "SD");
      }
      root.close();

      if (binIndex != 0) {
        // QRCode
        tft.qrcode("https://github.com/armel/ICSMeter", 90, 80, 140, 6);

        tft.setTextFont(1);
        tft.setTextSize(1);

        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextDatum(CC_DATUM);

        for (uint8_t i = TIMEOUT_BIN_LOADER * 10; i > 0; i--) {

          checkButtons();

          if (i % 10 == 0) {
            tmpName += ".";
            tft.drawString(tmpName, 160, 20);
          }

          if (btnA || btnC) {
            SD.end(); // If not Bluetooth doesn't work !!!
            return;
          } else if (btnB) {
            click = 1;
            tft.fillRect(0, 0, 320, 240, TFT_BLACK);
            break;
          }

          vTaskDelay(100);
        }
      }

      while (click == 1) {
        while (btnB != 0) {

          checkButtons();
          vTaskDelay(100);
        }

        tft.setTextFont(1);
        tft.setTextSize(2);

        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextDatum(CC_DATUM);
        tft.drawString("Bin Loader V0.2", 160, 20);

        checkButtons();

        if (btnA) {
          cursor--;
        } else if (btnC) {
          cursor++;
        } else if (btnB) {
          if (binFilename[cursor].substring(0, 4) == "SP_/") {
            updateFromFS(SPIFFS, binFilename[cursor].substring(3));
          } else {
            updateFromFS(SD, binFilename[cursor].substring(3));
          }
          ESP.restart();
        }

        cursor = (cursor < 0) ? binIndex - 1 : cursor;
        cursor = (cursor > binIndex - 1) ? 0 : cursor;

        start = cursor / limit;

        stop = (start * limit) + limit;

        /*
        Serial.print(cursor);
        Serial.print("-");
        Serial.print(start);
        Serial.print("-");
        Serial.print(stop);
        Serial.println("----------");
        */

        if (change != cursor) {
          change = cursor;
          tft.setTextPadding(320);

          uint8_t i = 0;
          for (uint8_t j = (start * limit); j < stop; j++) {
            tmpName = binFilename[j].substring(4);

            if (cursor == j) {
              tmpName = ">> " + tmpName + " <<";

              if (binFilename[cursor].substring(0, 4) == "SP_/") {
                tft.setTextSize(1);
                tft.drawString("SPI Flash File Storage", 160, 50);
              } else {
                tft.setTextSize(1);
                tft.drawString("SD Card Storage", 160, 50);
              }
            }

            tft.setTextSize(2);
            tft.drawString(tmpName, 160, 80 + i * 20);
            i++;
          }
        }
        vTaskDelay(100);
      }
      SD.end(); // If not Bluetooth doesn't work !!!
    }

  };
};


