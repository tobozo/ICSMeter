#include "Updater.hpp"

#define TIMEOUT_BIN_LOADER 3 // 3 sec


namespace ICSMeter
{

  static int btnA, btnB, btnC, btnL, btnM, btnR;

  // Get button
  void getButton()
  {
    M5.update();

    btnA = M5.BtnA.isPressed();
    btnB = M5.BtnB.isPressed();
    btnC = M5.BtnC.isPressed();

    btnL = M5.BtnA.pressedFor(2000);
    btnM = M5.BtnB.pressedFor(2000);
    btnR = M5.BtnC.pressedFor(2000);

    /*
    Serial.print(btnA);
    Serial.print(btnB);
    Serial.print(btnC);
    Serial.print(btnL);
    Serial.print(btnM);
    Serial.println(btnR);
    */
  }


  namespace fs
  {

    // Bin loader
    static File root;
    static String binFilename[128];
    static uint8_t binIndex = 0;

    // List files on SPIFFS or SD
    void getBinaryList(File dir, String type)
    {
      while (true)
      {
        File entry = dir.openNextFile();
        if (!entry)
        {
          // no more files
          break;
        }

        if (strstr(entry.name(), "/.") == NULL && strstr(entry.name(), ".bin") != NULL)
        {
          // Serial.println(type + "_" + entry.name());
          binFilename[binIndex] = type + "_" + entry.name();
          binIndex++;
        }

        if (entry.isDirectory() && strstr(entry.name(), "/.") == NULL)
        {
          getBinaryList(entry, type);
        }

        entry.close();
      }
    }

    // Bin Loader
    void binLoader()
    {
      boolean click = 0;
      int8_t cursor = 0;
      int8_t start = 0;
      int8_t stop = 0;
      int8_t limit = 8;
      int8_t change = 255;
      String tmpName;

      if (!SPIFFS.begin())
      {
        Serial.println("SPIFFS Mount Failed");

        M5.Lcd.setTextFont(1);
        M5.Lcd.setTextSize(2);

        M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
        M5.Lcd.setTextDatum(CC_DATUM);
        M5.Lcd.drawString("Flash File System", 160, 20);
        M5.Lcd.drawString("needs to be formated.", 160, 50);
        M5.Lcd.drawString("It takes around 4 minutes.", 160, 100);
        M5.Lcd.drawString("Please, wait until ", 160, 150);
        M5.Lcd.drawString("the application starts !", 160, 180);

        Serial.println("SPIFFS Formating...");

        SPIFFS.format(); // Format SPIFFS...

        M5.Lcd.setTextFont(0);
        M5.Lcd.setTextSize(0);

        return;
      }

      root = SPIFFS.open("/");
      getBinaryList(root, "SP");

      if (SD.begin(GPIO_NUM_4, SPI, 25000000))
      {
        root = SD.open("/");
        getBinaryList(root, "SD");
      }

      if (binIndex != 0)
      {
        // QRCode
        M5.Lcd.qrcode("https://github.com/armel/ICSMeter", 90, 80, 140, 6);

        M5.Lcd.setTextFont(1);
        M5.Lcd.setTextSize(1);

        M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
        M5.Lcd.setTextDatum(CC_DATUM);

        for (uint8_t i = TIMEOUT_BIN_LOADER * 10; i > 0; i--)
        {
          getButton();

          if (i % 10 == 0)
          {
            tmpName += ".";
            M5.Lcd.drawString(tmpName, 160, 20);
          }

          if (btnA || btnC)
          {
            SD.end(); // If not Bluetooth doesn't work !!!
            return;
          }
          else if (btnB)
          {
            click = 1;
            M5.Lcd.fillRect(0, 0, 320, 240, TFT_BLACK);
            break;
          }

          vTaskDelay(100);
        }
      }

      while (click == 1)
      {
        while (btnB != 0)
        {
          getButton();
          vTaskDelay(100);
        }

        M5.Lcd.setTextFont(1);
        M5.Lcd.setTextSize(2);

        M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
        M5.Lcd.setTextDatum(CC_DATUM);
        M5.Lcd.drawString("Bin Loader V0.2", 160, 20);

        getButton();

        if (btnA)
        {
          cursor--;
        }
        else if (btnC)
        {
          cursor++;
        }
        else if (btnB)
        {
          if (binFilename[cursor].substring(0, 4) == "SP_/")
          {
            updateFromFS(SPIFFS, binFilename[cursor].substring(3));
          }
          else
          {
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

        if (change != cursor)
        {
          change = cursor;
          M5.Lcd.setTextPadding(320);

          uint8_t i = 0;
          for (uint8_t j = (start * limit); j < stop; j++)
          {
            tmpName = binFilename[j].substring(4);

            if (cursor == j)
            {
              tmpName = ">> " + tmpName + " <<";

              if (binFilename[cursor].substring(0, 4) == "SP_/")
              {
                M5.Lcd.setTextSize(1);
                M5.Lcd.drawString("SPI Flash File Storage", 160, 50);
              }
              else
              {
                M5.Lcd.setTextSize(1);
                M5.Lcd.drawString("SD Card Storage", 160, 50);
              }
            }

            M5.Lcd.setTextSize(2);
            M5.Lcd.drawString(tmpName, 160, 80 + i * 20);
            i++;
          }
        }
        vTaskDelay(100);
      }
      SD.end(); // If not Bluetooth doesn't work !!!
    }

  };
};


