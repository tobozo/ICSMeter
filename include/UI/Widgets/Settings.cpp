

#include "../Widgets.hpp"


namespace ICSMeter
{

  namespace UI
  {

    namespace Settings
    {

      using namespace modules;
      using namespace CSS;

      uint32_t menu_delay = 150; // typematic button rate

      int32_t  x = 44;
      int32_t  y = 4;
      uint32_t w = 320 - (x * 2);
      uint32_t h = 185;


      const TextStyle_t SettingsStyle =
      {
        .fgColor   = TFT_MENU_SELECT,
        .bgColor   = TFT_MENU_BACK,
        .size      = 1,
        .datum     = MC_DATUM,
        .paddingX  =  w - 2
      };

      const FontStyle_t H1FontStyle = { &YELLOWCRE8pt7b, &SettingsStyle, TRANSPARENT };
      const FontStyle_t H2FontStyle = { &Font0,          &SettingsStyle, TRANSPARENT };
      const FontStyle_t ULFontStyle = { &YELLOWCRE8pt7b, &SettingsStyle, OPAQUE };
      const FontStyle_t LIFontStyle = { &tahoma8pt7b,    &SettingsStyle, OPAQUE };

      typedef void (*settings_callback_t)();
      struct settings_handler_t
      {
        const char* label;
        const settings_callback_t callback;
      };

      void onChangeMeasuredValues ();
      void onChangeTransverterMode();
      void onSwitchTheme          ();
      void onAdjustBrightness     ();
      void onAdjustBeepVolume     ();
      void onSetScreensaverDelay  ();
      void onShowIPAddress        ();
      void onShowMacAddress       ();
      void onExitSettingsMenu     ();

      const settings_handler_t menuchoices[] =
      {
        { "Measured Values",  onChangeMeasuredValues  },
        { "Transverter Mode", onChangeTransverterMode },
        { "Themes",           onSwitchTheme           },
        { "Brightness",       onAdjustBrightness      },
        { "Beep",             onAdjustBeepVolume      },
        { "Screensaver",      onSetScreensaverDelay   },
        { "IP Address",       onShowIPAddress         },
        { "Mac Address",      onShowMacAddress        },
        { "Shutdown",         ScreenSaver::shutdown   },
        { "Exit",             onExitSettingsMenu      },
      };



      void drawMenu(uint8_t x, uint8_t y, uint16_t w, uint8_t h)
      {
        tft.fillRoundRect(x, y, w, h, 8, TFT_MENU_BACK);
        tft.drawRoundRect(x, y, w, h, 8, TFT_MENU_BORDER);

        CSS::drawStyledString( &tft, "SETTINGS", 160, 14 + y, &H1FontStyle );
        CSS::drawStyledString( &tft, APP_TITLE, 160, 28 + y, &H2FontStyle );

        tft.drawFastHLine(120, 3, 80, Theme::layout->bgcolor);
        tft.drawFastHLine(x + 1, 36 + y, w - 2, TFT_MENU_SELECT);
        tft.drawFastHLine(x + 1, (y + h) - 24, w - 2, TFT_MENU_SELECT);
      }


      void draw()
      {
        // Enter settings menu
        if(btnB) {
          mode = true;
          while(lock == true) {
            vTaskDelay(10);
          }
          drawMenu(x, y, w, h);
          drawOption(choice, select, x, y, w);
        }
      }


      void drawOption(int8_t settingsChoice, bool settingsSelect, uint8_t x, uint8_t y, uint16_t w)
      {
        uint8_t start = 0;
        uint8_t i, j;
        size_t stop = sizeof(menuchoices) / sizeof(settings_handler_t);

        if(settingsChoice > 6) {
          start = settingsChoice - 6;
        } else {
          start = 0;
        }

        stop = start + 7;
        j = 0;

        CSS::setFontStyle( &tft, &LIFontStyle );

        for(i = start; i < stop; i++) {

          if(settingsChoice == i ) {
            if( settingsSelect ) {
              tft.setTextColor(TFT_BLACK, TFT_MENU_SELECT);
            } else {
              tft.setTextColor(TFT_MENU_BACK, TFT_MENU_SELECT);
            }
          } else {
            tft.setTextColor(TFT_MENU_SELECT, TFT_MENU_BACK);
          }

          CSS::drawStyledString( &tft, menuchoices[i].label, 160, 45 + y + (j * 18), nullptr );

          j++;
          if(j > 7) j = 7;
        }
      }


      void handle() // called repeatedly from a task loop
      {
        if( !mode ) { // Render settings list
          draw();
          menu_delay = 300;
        } else {
          if( !select ) { // Browse setting list
            handleBrowsing();
          } else { // Manage setting
            handleSettings();
          }
        }
        vTaskDelay(pdMS_TO_TICKS(menu_delay));
      }


      void handleSettings()
      {
        if( menuchoices[choice].callback ) {
          CSS::setFontStyle( &tft, &ULFontStyle );
          menuchoices[choice].callback();
        }
      }


      void handleBrowsing()
      {
        if(btnA || btnC) {
          if(btnA) {
            choice--;
          } else if(btnC) {
            choice++;
          }
          // settings items pagination
          size_t stop = sizeof(menuchoices) / sizeof(settings_handler_t);
          stop--;

          choice = (choice < 0) ? stop : choice;
          choice = (choice > stop) ? 0 : choice;

          drawOption(choice, select, x, y, w);

          menu_delay = 150;

        } else if(btnB) {
          select = true;
          drawOption(choice, select, x, y, w);
          if( strcmp( menuchoices[choice].label, "Shutdown" ) == 0 ) {
            ScreenSaver::shutdown();
          } else if( strcmp( menuchoices[choice].label, "Exit" ) == 0 ) {
            onExitSettingsMenu();
          }
        }
      }


      void onExitSettingsMenu()
      {
        clearData();
        select = false;
        mode = false;
        UI::drawWidgets( true );
        vTaskDelay(pdMS_TO_TICKS(300));
      }


      void onChangeMeasuredValues()
      {
        CSS::drawStyledString( &tft, Measure::choices[Measure::value], 160, h - 6, &ULFontStyle );

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
          Measure::drawLabels( true );
          menu_delay = 300;
        } else if(btnB == 1) {
          Measure::save();
          onExitSettingsMenu();
        }
      }


      void onSwitchTheme()
      {
        CSS::drawStyledString( &tft, Theme::choices[Theme::theme], 160, h - 6, &ULFontStyle );

        if(btnA || btnC) {

          if(btnA == 1) {
            if( Theme::theme > 0 ) Theme::theme--;
            else Theme::theme = Theme::THEMES_COUNT-1;
          } else if(btnC == 1) {
            if( Theme::theme+2 <= Theme::THEMES_COUNT ) Theme::theme++;
            else Theme::theme = 0;
          }
          menu_delay = 300;
          Theme::set();
          UI::drawWidgets( true );
          drawMenu(x, y, w, h);
          drawOption(choice, select, x, y, w);

        } else if(btnB == 1) {
          if(Theme::themeOld != Theme::theme) {
            Theme::themeOld = Theme::theme;
            Theme::save();
          }
          onExitSettingsMenu();
        }
      }


      void onAdjustBrightness()
      {
        char textbox[20];
        snprintf( textbox, 19, "%s %d%s", BackLight::label, BackLight::brightness, "%" );
        CSS::drawStyledString( &tft, textbox, 160, h - 6, &ULFontStyle );

        menu_delay = 25;

        if(btnA == 1) {
          BackLight::decrease();
        }
        if(btnC == 1) {
          BackLight::increase();
        }

        if(btnB == 1) {
          if(BackLight::brightnessOld != BackLight::brightness) {
            BackLight::brightnessOld = BackLight::brightness;
            BackLight::save();
          }
          onExitSettingsMenu();
        }
      }


      void onChangeTransverterMode()
      {

        menu_delay = 300;
        int8_t value = Transverter::get();

        if(value == 0) {
          CSS::drawStyledString( &tft, "OFF", 160, h - 6, &ULFontStyle );
        } else {
          char transverter_value_str[17];
          format_number( Transverter::choices[value], 16, transverter_value_str, '.' );
          CSS::drawStyledString( &tft, transverter_value_str, 160, h - 6, &ULFontStyle );
        }

        size_t lastIndex = ( sizeof(Transverter::choices) / sizeof(Transverter::choices[0]) ) - 1;

        if(btnA || btnC) {
          if(btnA == 1) {
            value--;
            Transverter::set( (value < 0) ? lastIndex : value);
          } else if(btnC == 1) {
            value++;
            Transverter::set( (value > lastIndex) ? 0 : value );
          }
        } else if(btnB == 1) {
          Transverter::save();
          onExitSettingsMenu();
        }
      }


      void onAdjustBeepVolume()
      {
        menu_delay = 25;

        char textbox[20];
        snprintf( textbox, 19, "%s %d%s", Beeper::label, Beeper::beepVolume, "%" );
        CSS::drawStyledString( &tft, textbox, 160, h - 6, &ULFontStyle );

        if(btnA || btnC) {
          if(btnA == 1) {
            Beeper::decrease();
          } else if(btnC == 1) {
            Beeper::increase();
          }
        } else if(btnB == 1) {
          Beeper::save();
          onExitSettingsMenu();
        }
      }


      void onSetScreensaverDelay()
      {
        menu_delay = 75;

        char textbox[20];
        snprintf( textbox, 19, "%s %d%s", ScreenSaver::label, ScreenSaver::countdown, " MIN" );
        CSS::drawStyledString( &tft, textbox, 160, h - 6, &ULFontStyle );

        if(btnA || btnC) {
          if(btnA == 1) {
            ScreenSaver::decrease();
          } else if(btnC == 1) {
            ScreenSaver::increase();
          }
        } else if(btnB == 1) {
          ScreenSaver::save();
          onExitSettingsMenu();
        }
      }


      void onShowIPAddress()
      {
        menu_delay = 300;

        CSS::drawStyledString( &tft, WiFi.localIP().toString().c_str(), 160, h - 6, &ULFontStyle );

        if(btnB == 1) {
          onExitSettingsMenu();
        }
      }


      void onShowMacAddress()
      {
        menu_delay = 300;

        CSS::drawStyledString( &tft, WiFi.macAddress().c_str(), 160, h - 6, &ULFontStyle );

        if(btnB == 1) {
          onExitSettingsMenu();
        }
      }



    };

  };

};
