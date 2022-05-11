#include "ScreenSaver.hpp"

#include "../../Utils/Maths.hpp"
#include "../../net/Controls.hpp"
#include "../Controls.hpp"
#include "../UI.hpp"


namespace ICSMeter
{

  namespace UI
  {

    namespace Settings
    {


      void drawMenu(uint8_t x, uint8_t y, uint16_t w, uint8_t h)
      {
        tft.fillRoundRect(x, y, w, h, 8, TFT_MENU_BACK);
        tft.drawRoundRect(x, y, w, h, 8, TFT_MENU_BORDER);

        tft.setTextDatum(CC_DATUM);
        tft.setFont(&YELLOWCRE8pt7b);
        tft.setTextPadding(w - 2);
        tft.setTextColor(TFT_MENU_SELECT);
        tft.drawString("SETTINGS", 160, 14 + y);

        tft.setTextDatum(CC_DATUM);
        tft.setFont(0);
        tft.setTextPadding(w - 2);
        tft.setTextColor(TFT_MENU_SELECT);
        tft.drawString(String(NAME) + " V" + String(VERSION) + " by " + String(AUTHOR), 160, 28 + y);

        tft.drawFastHLine(120, 3, 80, TFT_BACK);
        tft.drawFastHLine(x + 1, 36 + y, w - 2, TFT_MENU_SELECT);
        tft.drawFastHLine(x + 1, (y + h) - 24, w - 2, TFT_MENU_SELECT);
      }


      void draw()
      {
        // Enter settings
        if(btnB) {
          mode = true;
          while(lock == true) {
            vTaskDelay(10);
          }
          drawMenu(x, y, w, h);
          drawOption(choice, select, x, y, w);
          vTaskDelay(300);
        }
        vTaskDelay(100);
      }

      void onManageSettings()
      {
        tft.setTextDatum(CC_DATUM);
        tft.setFont(&YELLOWCRE8pt7b);
        tft.setTextPadding(w - 2);
        tft.setTextColor(TFT_MENU_SELECT, TFT_MENU_BACK);

        if( menuchoices[choice].callback ) menuchoices[choice].callback();
      }


      void handle()
      {
        if(mode == false) {
          draw();
        } else if(mode == true) { // Select settings
          if(select == false) {
            onBrowse();
          } else if(select == true) { // Manage settings
            onManageSettings();
          }
        }
      }


      void onExit()
      {
        clearData();
        UI::draw();
        select = false;
        mode = false;
        vTaskDelay(pdMS_TO_TICKS(150));
      }


      void onBrowse()
      {
        if(btnA || btnC) {
          if(btnA) {
            choice--;
          } else if(btnC) {
            choice++;
          }

          size_t stop = sizeof(menuchoices) / sizeof(settings_handler_t);
          stop--;

          choice = (choice < 0) ? stop : choice;
          choice = (choice > stop) ? 0 : choice;

          drawOption(choice, select, x, y, w);
        } else if(btnB) {
          select = true;
          drawOption(choice, select, x, y, w);

          String settingsString = String(menuchoices[choice].label);
          if(settingsString == "Shutdown") {
            ScreenSaver::shutdown();
          } else if(settingsString == "Exit") {
            onExit();
          }
        }
        vTaskDelay(pdMS_TO_TICKS(150));
      }


      void onMeasured()
      {
        tft.drawString(String(Measure::choices[Measure::value]), 160, h - 6);

        if(btnA || btnC) {
          if(btnA == 1) {
            Measure::value -= 1;
            if(Measure::value < 0) {
              Measure::value = 2;
            }
          } else if(btnC == 1) {
            Measure::value += 1;
            if(Measure::value > 2) {
              Measure::value = 0;
            }
          }
        } else if(btnB == 1) {
          if(Measure::valueOld != Measure::value)
            preferences.putUInt("measure", Measure::value);
          clearData();
          UI::draw();
          select = false;
          mode = false;
          vTaskDelay(pdMS_TO_TICKS(150));
        }
        vTaskDelay(pdMS_TO_TICKS(150));
      }


      void onTheme()
      {
        tft.drawString(String(Theme::choices[Theme::theme]), 160, h - 6);

        if(btnA || btnC) {

          if(btnA == 1) {
            if( Theme::theme > 0 ) Theme::theme--;
            else Theme::theme = THEMES_COUNT-1;
            //Theme::theme -= 1;
            //if(Theme::theme < 0) {
            //  Theme::theme = 1;
            //}
          } else if(btnC == 1) {

            if( Theme::theme+2 <= THEMES_COUNT ) Theme::theme++;
            else Theme::theme = 0;

            //Theme::theme += 1;
            //if(Theme::theme > 1) {
            //  Theme::theme = 0;
            //}
          }

          Theme::set();


        } else if(btnB == 1) {
          if(Theme::themeOld != Theme::theme) {
            preferences.putUInt("theme", Theme::theme);
            Theme::themeOld = Theme::theme;
          }
          clearData();
          UI::draw();
          select = false;
          mode = false;
          vTaskDelay(pdMS_TO_TICKS(150));
        }
        vTaskDelay(pdMS_TO_TICKS(150));
      }


      void onBrightness()
      {
        tft.drawString(String(choiceBrightness) + " " + String(brightness) + "%", 160, h - 6);

        if(btnA || btnC) {
          if(btnA == 1) {
            brightness -= 1;
            if(brightness < 1) {
              brightness = 1;
            }
          } else if(btnC == 1) {
            brightness += 1;
            if(brightness > 100) {
              brightness = 100;
            }
          }
        } else if(btnB == 1) {
          if(brightnessOld != brightness)
            preferences.putUInt("brightness", brightness);
          clearData();
          UI::draw();
          select = false;
          mode = false;
          vTaskDelay(pdMS_TO_TICKS(150));
        }
        setBrightness(map(brightness, 1, 100, 1, 254));
        vTaskDelay(pdMS_TO_TICKS(25));
      }


      void onTransverter()
      {
         if(Transverter::value == 0) {
          tft.drawString("OFF", 160, h - 6);
        } else {
          String transverterStringOld = String(Transverter::choices[Transverter::value]);
          String transverterStringNew = "";
          uint8_t lenght = transverterStringOld.length();
          int8_t i;

          for(i = lenght - 6; i >= 0; i -= 3) {
            transverterStringNew = "." + transverterStringOld.substring(i, i + 3) + transverterStringNew;
          }

          if(i == -3) {
            transverterStringNew = transverterStringNew.substring(1, transverterStringNew.length());
          } else {
            transverterStringNew = transverterStringOld.substring(0, i + 3) + transverterStringNew;
          }

          tft.drawString(transverterStringNew, 160, h - 6);
        }

        size_t stop = sizeof(Transverter::choices) / sizeof(Transverter::choices[0]);
        stop--;

        if(btnA || btnC) {
          if(btnA == 1) {
            Transverter::value -= 1;
            if(Transverter::value < 0) {
              Transverter::value = stop;
            }
          } else if(btnC == 1) {
            Transverter::value += 1;
            if(Transverter::value > stop) {
              Transverter::value = 0;
            }
          }
        } else if(btnB == 1) {
          if(Transverter::valueOld != Transverter::value)
            preferences.putUInt("transverter", Transverter::value);
          clearData();
          UI::draw();
          select = false;
          mode = false;
          vTaskDelay(pdMS_TO_TICKS(150));
        }
        vTaskDelay(pdMS_TO_TICKS(150));
      }


      void onBeep()
      {
        tft.drawString(String(choiceBeep) + " " + String(beep) + "%", 160, h - 6);

        if(btnA || btnC) {
          if(btnA == 1) {
            beep -= 1;
            if(beep < 0) {
              beep = 0;
            }
          } else if(btnC == 1) {
            beep += 1;
            if(beep > 100) {
              beep = 100;
            }
          }
        } else if(btnB == 1) {
          if(beepOld != beep)
            preferences.putUInt("beep", beep);
          clearData();
          UI::draw();
          select = false;
          mode = false;
          vTaskDelay(pdMS_TO_TICKS(150));
        }
        vTaskDelay(pdMS_TO_TICKS(25));
      }


      void onScreensaver()
      {
        tft.drawString(String(choiceScreensaver) + " " + String(ScreenSaver::countdown) + " MIN", 160, h - 6);

        if(btnA || btnC) {
          if(btnA == 1) {
            ScreenSaver::countdown -= 1;
            if(ScreenSaver::countdown < 1) {
              ScreenSaver::countdown = 1;
            }
          } else if(btnC == 1) {
            ScreenSaver::countdown += 1;
            if(ScreenSaver::countdown > 60) {
              ScreenSaver::countdown = 60;
            }
          }
        } else if(btnB == 1) {
          if(ScreenSaver::countdownOld != ScreenSaver::countdown) {
            preferences.putUInt("screensaver", ScreenSaver::countdown);
            ScreenSaver::countdownOld = ScreenSaver::countdown;
          }
          clearData();
          UI::draw();
          select = false;
          mode = false;
          vTaskDelay(pdMS_TO_TICKS(150));
        }
        vTaskDelay(pdMS_TO_TICKS(25));
      }


      void onIPAddress()
      {
        tft.drawString(String(WiFi.localIP().toString().c_str()), 160, h - 6);

        if(btnB == 1) {
          clearData();
          UI::draw();
          select = false;
          mode = false;
          vTaskDelay(pdMS_TO_TICKS(150));
        }
        vTaskDelay(pdMS_TO_TICKS(150));
      }


      void drawOption(int8_t settingsChoice, bool settingsSelect, uint8_t x, uint8_t y, uint16_t w)
      {
        uint8_t start = 0;
        uint8_t i, j;

        tft.setTextDatum(CC_DATUM);
        tft.setFont(&tahoma8pt7b);
        tft.setTextPadding(w - 2);
        tft.setTextColor(TFT_MENU_SELECT, TFT_MENU_BACK);

        size_t stop = sizeof(menuchoices) / sizeof(settings_handler_t);

        if(settingsChoice > 6) {
          start = settingsChoice - 6;
        } else {
          start = 0;
        }

        stop = start + 7;

        j = 0;

        for(i = start; i < stop; i++) {

          if(settingsChoice == i ) {
            tft.setTextColor(TFT_BLACK, TFT_MENU_SELECT);
            if( settingsSelect ) {
              // TODO: actually draw something different
              tft.drawString(menuchoices[i].label, 160, 45 + y + (j * 18));
            } else {
              tft.drawString(menuchoices[i].label, 160, 45 + y + (j * 18));
            }
          } else {
            tft.setTextColor(TFT_MENU_SELECT, TFT_MENU_BACK);
            tft.drawString(menuchoices[i].label, 160, 45 + y + (j * 18));
          }

          j++;
          if(j > 7) j = 7;
        }
      }

    };

  };

};
