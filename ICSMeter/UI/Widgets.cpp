#include "Widgets.hpp"

#include "../Utils/Maths.hpp"
#include "../net/Controls.hpp"

#include "Menu.hpp"


namespace ICSMeter
{

  namespace UI
  {

    using namespace net;
    using namespace Utils;


    void clearData()
    {
      angleOld = 0;

      batteryLevelOld = 0;
      sOld = 255;
      SWROld = 255;
      powerOld = 255;

      filterOld = "";
      modeOld = "";
      valStringOld = "";
      subValStringOld = "";

      batteryCharginglOld = true;

      measureOld = 5;
    }

    // View GUI
    void viewGUI()
    {
      using namespace Theme;

      resetColor();

      M5.Lcd.fillScreen(TFT_BACK);

      if(theme == 0)
      {
        M5.Lcd.drawJpg(smeterTopClassic, sizeof(smeterTopClassic), 0, 0, 320, 20);
        if (IC_MODEL == 705)
          M5.Lcd.drawJpg(smeterMiddleClassic10, sizeof(smeterMiddleClassic10), 0, 20, 320, 140);
        else
          M5.Lcd.drawJpg(smeterMiddleClassic100, sizeof(smeterMiddleClassic100), 0, 20, 320, 140);
        M5.Lcd.drawJpg(smeterBottomClassic, sizeof(smeterBottomClassic), 0, 160, 320, 80);
      }
      else
      {
        M5.Lcd.drawJpg(smeterTopDark, sizeof(smeterTopDark), 0, 0, 320, 20);
        if (IC_MODEL == 705)
          M5.Lcd.drawJpg(smeterMiddleDark10, sizeof(smeterMiddleDark10), 0, 20, 320, 140);
        else
          M5.Lcd.drawJpg(smeterMiddleDark100, sizeof(smeterMiddleDark100), 0, 20, 320, 140);
        M5.Lcd.drawJpg(smeterBottomDark, sizeof(smeterBottomDark), 0, 160, 320, 80);
      }
    }



    namespace Theme
    {

      // Reset color
      void resetColor()
      {
        switch (theme)
        {
        case 0:
          TFT_BACK  = M5.Lcd.color565(TFT_BACK_CLASSIC.r,  TFT_BACK_CLASSIC.g,  TFT_BACK_CLASSIC.b);
          TFT_FRONT = M5.Lcd.color565(TFT_FRONT_CLASSIC.r, TFT_FRONT_CLASSIC.g, TFT_FRONT_CLASSIC.b);
          break;

        case 1:
          TFT_BACK  = M5.Lcd.color565(TFT_BACK_DARK.r,  TFT_BACK_DARK.g,  TFT_BACK_DARK.b);
          TFT_FRONT = M5.Lcd.color565(TFT_FRONT_DARK.r, TFT_FRONT_DARK.g, TFT_FRONT_DARK.b);
          break;
        }
      }

    };


    namespace Battery
    {

      using namespace Theme;
      // Print battery
      void viewBattery()
      {
        uint8_t batteryLevel;
        boolean batteryCharging;

        if (screensaverMode == false && settingsMode == false)
        {
          // On left, view battery level
          batteryLevel = map(getBatteryLevel(1), 0, 100, 0, 16);
          batteryCharging = isCharging();

          if (batteryLevel != batteryLevelOld || batteryCharging != batteryCharginglOld)
          {

            if(theme == 0)
            {
              M5.Lcd.drawJpg(smeterTopClassic, sizeof(smeterTopClassic), 0, 0, 320, 20);
            }
            else
            {
              M5.Lcd.drawJpg(smeterTopDark, sizeof(smeterTopDark), 0, 0, 320, 20);
            }
            // Settings
            M5.Lcd.setFont(0);
            M5.Lcd.setTextDatum(CC_DATUM);

            M5.Lcd.fillRoundRect(4, 4, 56, 13, 2, TFT_MODE_BACK);
            M5.Lcd.drawRoundRect(4, 4, 56, 13, 2, TFT_MODE_BORDER);
            M5.Lcd.setTextColor(TFT_WHITE);

            if (IC_CONNECT == BT)
              M5.Lcd.drawString(String(IC_MODEL) + " BT", 32, 11);
            else
              M5.Lcd.drawString(String(IC_MODEL) + " USB", 32, 11);

            if (transverter > 0)
            {
              M5.Lcd.fillRoundRect(62, 4, 26, 13, 2, TFT_MODE_BACK);
              M5.Lcd.drawRoundRect(62, 4, 26, 13, 2, TFT_MODE_BORDER);
              M5.Lcd.setTextColor(TFT_WHITE);
              M5.Lcd.drawString("LO" + String(transverter), 76, 11);
            }

            // M5.Lcd.drawFastHLine(0, 20, 320, TFT_BLACK);

            batteryLevelOld = batteryLevel;
            batteryCharginglOld = batteryCharging;

            M5.Lcd.drawRect(294, 4, 20, 12, TFT_FRONT);
            M5.Lcd.drawRect(313, 7, 4, 6, TFT_FRONT);
            M5.Lcd.fillRect(296, 6, batteryLevel, 8, TFT_FRONT);

            if (batteryCharging)
            {
              M5.Lcd.setTextColor(TFT_FRONT);
              M5.Lcd.setFont(0);
              M5.Lcd.setTextDatum(CC_DATUM);
              M5.Lcd.setTextPadding(0);
              M5.Lcd.drawString("+", 290, 11);
            }
            else
            {
              M5.Lcd.setTextColor(TFT_FRONT);
              M5.Lcd.setFont(0);
              M5.Lcd.setTextDatum(CR_DATUM);
              M5.Lcd.setTextPadding(0);
              M5.Lcd.drawString(String(getBatteryLevel(1)) + "%", 290, 11);
            }
          }
        }
      }


    };

    namespace Needle
    {

      using namespace Theme;

      // Print needle
      void needle(float_t angle, uint16_t a, uint16_t b, uint16_t c, uint16_t d)
      {
        uint16_t x, y;

        if (angle != angleOld)
        {
          angleOld = angle;

          x = a;
          y = b;

          rotate(&x, &y, angle);

          a = 160 + x;
          b = 220 - y;

          x = c;
          y = d;

          rotate(&x, &y, angle);

          c = 160 + x;
          d = 220 - y;


          if(theme == 0)
          {
            if (IC_MODEL == 705)
              M5.Lcd.drawJpg(smeterMiddleClassic10, sizeof(smeterMiddleClassic10), 0, 20, 320, 130);
            else
              M5.Lcd.drawJpg(smeterMiddleClassic100, sizeof(smeterMiddleClassic100), 0, 20, 320, 130);
          }
          else
          {
            if (IC_MODEL == 705)
              M5.Lcd.drawJpg(smeterMiddleDark10, sizeof(smeterMiddleDark10), 0, 20, 320, 130);
            else
              M5.Lcd.drawJpg(smeterMiddleDark100, sizeof(smeterMiddleDark100), 0, 20, 320, 130);
          }

          // M5.Lcd.drawFastHLine(0, 150, 320, TFT_BLACK);

          M5.Lcd.drawLine(a + 2, b, c + 3, d, TFT_NEDDLE_2);
          M5.Lcd.drawLine(a + 2, b, c + 2, d, TFT_NEDDLE_1);
          M5.Lcd.drawLine(a + 1, b, c + 1, d, TFT_RED);
          M5.Lcd.drawLine(a, b, c, d, TFT_RED);
          M5.Lcd.drawLine(a - 1, b, c - 1, d, TFT_RED);
          M5.Lcd.drawLine(a - 2, b, c - 2, d, TFT_NEDDLE_1);
          M5.Lcd.drawLine(a - 2, b, c - 3, d, TFT_NEDDLE_2);
        }
      }
    };


    namespace Measure
    {

      using namespace Theme;

      // Print Measure
      void viewMeasure()
      {
        uint16_t i = 65;
        uint8_t j;

        if (measure != measureOld)
        {
          measureOld = measure;

          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.setFont(&YELLOWCRE8pt7b);
          M5.Lcd.setTextPadding(0);

          for (j = 0; j <= 2; j++)
          {
            if (measure == j)
            {
              M5.Lcd.setTextColor(TFT_FRONT);
              reset = true;
            }
            else
            {
              M5.Lcd.setTextColor(TFT_DARKGREY);
            }

            M5.Lcd.drawString(choiceMeasures[j], i, 230);
            i += 95;
          }
        }
      }

      // Print value
      void value(String valString, uint8_t x, uint8_t y)
      {
        if (valString != valStringOld)
        {
          valStringOld = valString;

          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.setFont(&stencilie16pt7b);
          valString.replace(".", ",");
          //M5.Lcd.setFont(&YELLOWCRE8pt7b);
          M5.Lcd.setTextPadding(190);
          M5.Lcd.setTextColor(TFT_FRONT, TFT_BACK);
          M5.Lcd.drawString(valString, x, y);
        }
      }

      // Print sub value
      void subValue(String valString, uint8_t x, uint8_t y)
      {
        if (valString != subValStringOld)
        {
          subValStringOld = valString;

          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.setFont(&YELLOWCRE8pt7b);
          M5.Lcd.setTextPadding(160);
          //M5.Lcd.setTextColor(TFT_BLACK, TFT_RED);
          M5.Lcd.setTextColor(TFT_FRONT, TFT_BACK);
          // valString.replace(".", ",");
          M5.Lcd.drawString(valString, x, y);
        }
      }


    };

    namespace ScreenSaver
    {

      // Manage screensaver
      void wakeAndSleep()
      {
        static uint16_t x = rand() % 232;
        static uint16_t y = rand() % 196;
        static boolean xDir = rand() & 1;
        static boolean yDir = rand() & 1;

        if (screensaverMode == false && millis() - screensaverTimer > screensaver * 60 * 1000)
        {
          settingsMode = false;
          screensaverMode = true;
          screensaverTimer = 0;
          M5.Lcd.fillScreen(TFT_BLACK);
        }
        else if (screensaverMode == true && screensaverTimer != 0)
        {
          M5.Lcd.fillScreen(TFT_BLACK);
          clearData();
          viewGUI();
          screensaverMode = false;
          settingsMode = false;

          vTaskDelay(100);
        }
        else if (screensaverMode == true)
        {

          M5.Lcd.fillRect(x, y, 44, 22, TFT_BLACK);

          if (xDir)
          {
            x += 1;
          }
          else
          {
            x -= 1;
          }

          if (yDir)
          {
            y += 1;
          }
          else
          {
            y -= 1;
          }

          if (x < 44)
          {
            xDir = true;
            x = 44;
          }
          else if (x > 232)
          {
            xDir = false;
            x = 232;
          }

          if (y < 22)
          {
            yDir = true;
            y = 22;
          }
          else if (y > 196)
          {
            yDir = false;
            y = 196;
          }

          M5.Lcd.drawJpg(logo, sizeof(logo), x, y, 44, 22);

          if (IC_MODEL == 705 && IC_CONNECT == BT && btConnected == false)
            vTaskDelay(75);
          else if (IC_CONNECT == USB && wifiConnected == false)
            vTaskDelay(75);
        }

        // Debug trace
        if (DEBUG)
        {
          Serial.print(screensaverMode);
          Serial.print(" ");
          Serial.println(millis() - screensaverTimer);
        }
      }
    };



  };
};

