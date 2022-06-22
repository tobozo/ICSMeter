#include "../Widgets.hpp"
#include "../Themes/Themes.hpp"
#include "../../core/ICSMeter.hpp"

namespace ICSMeter
{

  namespace UI
  {

    namespace Settings
    {

      using namespace Utils;
      using namespace modules;
      using namespace Theme;

      uint32_t menu_delay = 150; // variable typematic button rate
      bool setting_selected = false;

      constexpr const char* SETTINGS_LABEL = "SETTINGS";
      const uint8_t MAX_ITEMS_IN_SETTINGS_MENU = 7;
      const uint32_t w = 232;
      const uint32_t h = 185;           // TODO: dynamize this
      const int32_t  x = (320/2)-(w/2); // TODO: dynamize this
      const int32_t  y = 4;
      const uint32_t hmiddle = 160;
      const uint32_t titleYpos = 179;
      const uint32_t rulerTitleYpos = y + 36;
      const uint32_t rulerValueYpos = y + (h - 24);

      const CSS::TextStyle_t SettingsStyle =
      {
        .fgColor   = SettingsMenuLightColor,
        .bgColor   = SettingsMenuBgColor,
        .size      = 1,
        .datum     = MC_DATUM,
        .paddingX  = w - 2
      };

      const CSS::FontStyle_t H1FontStyle = { &YELLOWCRE8pt7b, &SettingsStyle, CSS::TRANSPARENT };
      const CSS::FontStyle_t H2FontStyle = { &Font0,          &SettingsStyle, CSS::TRANSPARENT };
      const CSS::FontStyle_t ULFontStyle = { &YELLOWCRE8pt7b, &SettingsStyle, CSS::OPAQUE };
      const CSS::FontStyle_t LIFontStyle = { &tahoma8pt7b,    &SettingsStyle, CSS::OPAQUE };

      typedef void (*settings_callback_t)();
      struct settings_handler_t
      {
        const char* label;
        const settings_callback_t callback;
      };

      void onChangeICModel        ();
      void onChangeICID           ();
      void onChangeMeasuredValues ();
      void onChangeTransverterMode();
      void onSwitchTheme          ();
      void onAdjustBrightness     ();
      void onAdjustBeepVolume     ();
      void onSetScreensaverDelay  ();
      void onShowIPAddress        ();
      void onShowMacAddress       ();
      void exitSettingsMenu       ();
      void onShutdown             ();
      void onExit                 ();
      #if defined UPDATER_URL
        void onCheckForUpdate       ();
      #endif

      const settings_handler_t menuchoices[] =
      {
        { "ICOM Model",       onChangeICModel         },
        { "CI-V Address",     onChangeICID            },
        { "Measured Values",  onChangeMeasuredValues  },
        { "Transverter Mode", onChangeTransverterMode },
        { "Themes",           onSwitchTheme           },
        { "Brightness",       onAdjustBrightness      },
        { "Beep",             onAdjustBeepVolume      },
        { "Screensaver",      onSetScreensaverDelay   },
        { "IP Address",       onShowIPAddress         },
        { "Mac Address",      onShowMacAddress        },
        { "Shutdown",         onShutdown              },
      #if defined UPDATER_URL
        { "Update",           onCheckForUpdate        },
      #endif
        { "Exit",             onExit                  },
      };



      void setMenuDelay( uint32_t delay )
      {
        using namespace modules;

        menu_delay = delay;
        Beeper::beepPause = delay/2;
      }


      void loop() // called repeatedly from a task loop
      {
        if( ScreenSaver::isEnabled() || ScreenSaver::isAsleep() ) return;

        if( !dialog_enabled ) {
          if( buttons::btnB ) { // entering settings menu
            dialog_enabled   = true;
            setting_selected = false;
            //choice = 0;
            Settings::draw( true );
          }
          return;
        }

        if( buttons::hasBubble() ) {
          if( setting_selected ) { // editing a setting
            handleSettings();
          } else {
            handleBrowsing(); // browsing settings/selecting item
          }
        }
      }


      void drawSettingValue( const char* title, bool bold = false )
      {
        CSS::drawStyledString( &tft, title, hmiddle, titleYpos, bold ? &ULFontStyle : &LIFontStyle);
      }


      void draw( bool force_redraw )
      {
        if( !force_redraw || !Settings::dialog_enabled ) return;
        drawMenu();
        drawOptions();
        buttons::cancelBubble();
        menuchoices[choice].callback();
      }


      void handleSettings()
      {
        if( menuchoices[choice].callback ) {
          CSS::setFontStyle( &tft, &ULFontStyle );
          menuchoices[choice].callback();
          if( dialog_enabled ) {
            drawOptions();
            vTaskDelay( menu_delay );
          }
        }
      }


      void exitSettingsMenu()
      {
        if( dialog_enabled ) {
          setMenuDelay( 300 );
          setting_selected = false;
          dialog_enabled   = false;
          buttons::cancelBubble();
          log_d("Leaving settings menu");
          UI::draw( true );
        }
      }

      void onShutdown()
      {
        drawSettingValue(  "", true );
        if(buttons::btnB) {
          shutdown();
        }
      }


      void onExit()
      {
        drawSettingValue( "", true );
        if(buttons::btnB) {
          exitSettingsMenu();
        }
      }


      void handleBrowsing()
      {
        setMenuDelay( 300 );

        if(buttons::btnB) {
          if( strcmp( menuchoices[choice].label, "Shutdown" ) == 0 ) {
            onShutdown();
            return;
          } else if( strcmp( menuchoices[choice].label, "Exit" ) == 0 ) {
            onExit();
            return;
          }
          setting_selected = !setting_selected;
          buttons::cancelBubble();
          handleSettings();
          return;
        }

        if(buttons::btnA || buttons::btnC) { // paginating in settings categories

          size_t lastIndex = (sizeof(menuchoices) / sizeof(settings_handler_t)) - 1;

          if(buttons::btnA) {
            choice--;
            choice = (choice < 0) ? lastIndex : choice;
          } else if(buttons::btnC) {
            choice++;
            choice = (choice > lastIndex) ? 0 : choice;
          }

          buttons::cancelBubble();
          handleSettings();
          return;
        }
      }

      #if defined UPDATER_URL

        void onCheckForUpdate()
        {
          setMenuDelay( 300 );
          String _label;
          if( !wifi::available() ) {
            _label = "No WiFi";
          } else {
            _label = "Check Update";
          }

          drawSettingValue( _label.c_str() );

          if(buttons::btnB) {
            if( wifi::available() ) {
              drawSettingValue( "Checking..." );
              String updateURL = WebClient::GetLastUpdateURL();
              if( updateURL != "" ) {
                drawSettingValue( "Updating..." );
                if( FSUpdater::gzStreamUpdate( updateURL.c_str() ) ) {
                  ESP.restart();
                  while(1);
                }
              }
              drawSettingValue( "Failed..." );
              exitSettingsMenu();
              return;
            }
          }
        }
      #endif



      void onChangeICModel()
      {
        setMenuDelay( 300 );
        if(buttons::btnA || buttons::btnC) {
          if(buttons::btnA || buttons::btnC) {
            CIV::setICModel( CIV::IC_CHOICE + 1 );
          }
        }

        drawSettingValue( CIV::IC->label );

        if(buttons::btnB) {
          CIV::save();
          exitSettingsMenu();
          return;
        }
      }


      void onChangeICID()
      {
        setMenuDelay( 300 );
        if(buttons::btnA) {
          setMenuDelay( 25 );
          CIV::setICAddress( CIV::CIV_ADR - 1 );
        }
        if(buttons::btnC) {
          setMenuDelay( 25 );
          CIV::setICAddress( CIV::CIV_ADR + 1 );
        }

        char textbox[8];
        snprintf( textbox, 7, "0x%02X", CIV::CIV_ADR );
        drawSettingValue( textbox );

        if(buttons::btnB) {
          CIV::save();
          exitSettingsMenu();
        }
      }


      void onChangeMeasuredValues()
      {
        //using namespace Measure;
        setMenuDelay( 300 );
        if(buttons::btnA || buttons::btnC) {
          if(buttons::btnA) {
            switch (Measure::mode) {
              case Measure::MODE_PWR: Measure::mode = Measure::MODE_SWR; break;
              case Measure::MODE_SMT: Measure::mode = Measure::MODE_PWR; break;
              case Measure::MODE_SWR: Measure::mode = Measure::MODE_SMT; break;
              default: /* duh? */  break;
            }
          }
          if(buttons::btnC) {
            switch (Measure::mode) {
              case Measure::MODE_PWR: Measure::mode = Measure::MODE_SMT; break;
              case Measure::MODE_SMT: Measure::mode = Measure::MODE_SWR; break;
              case Measure::MODE_SWR: Measure::mode = Measure::MODE_PWR; break;
              default: /* duh? */  break;
            }
          }
          Measure::drawLabels( true );
        }

        drawSettingValue( Measure::choices[Measure::mode], true );

        if(buttons::btnB) {
          Measure::save();
          exitSettingsMenu();
          return;
        }
      }


      void onSwitchTheme()
      {
        setMenuDelay( 300 );
        if(buttons::btnA || buttons::btnC) {
          if(buttons::btnA) {
            Theme::theme = (Theme::theme+1)%Theme::THEMES_COUNT;
          }
          if(buttons::btnC) {
            Theme::theme = (Theme::theme+Theme::THEMES_COUNT-1)%Theme::THEMES_COUNT;
          }
          Theme::set();
          Needle::onThemeChange();
          UI::draw( true );
          drawMenu();
        }

        drawSettingValue( Theme::choices[Theme::theme], true );

        if(buttons::btnB) {
          Theme::save();
          exitSettingsMenu();
          return;
        }
      }


      void onAdjustBrightness()
      {
        using namespace modules;

        setMenuDelay( 300 );

        if(buttons::btnA) {
          setMenuDelay( 25 );
          BackLight::decrease();
        }
        if(buttons::btnC) {
          setMenuDelay( 25 );
          BackLight::increase();
        }

        if(buttons::btnB) {
          BackLight::save();
          exitSettingsMenu();
        }

        char textbox[20];
        snprintf( textbox, 19, "%s %d%s", BackLight::label, BackLight::getBrightness(), "%" );
        drawSettingValue( textbox, true );
      }


      void onChangeTransverterMode()
      {
        setMenuDelay( 300 );
        int8_t value = Transverter::get();

        size_t lastIndex = ( sizeof(Transverter::choices) / sizeof(Transverter::choices[0]) ) - 1;

        if(buttons::btnA || buttons::btnC) {
          if(buttons::btnA) {
            value--;
            Transverter::set( (value < 0) ? lastIndex : value);
          }
          if(buttons::btnC) {
            value++;
            Transverter::set( (value > lastIndex) ? 0 : value );
          }
          value = Transverter::get();
        }

        if(buttons::btnB) {
          Transverter::save();
          exitSettingsMenu();
        }

        if(value == 0) {
          drawSettingValue( "OFF", true );
        } else {
          char transverter_value_str[17];
          format_number( Transverter::choices[value], 16, transverter_value_str, '.' );
          drawSettingValue( transverter_value_str, true );
        }
      }


      void onAdjustBeepVolume()
      {
        using namespace modules;

        setMenuDelay( 300 );

        if(buttons::btnA || buttons::btnC) {
          setMenuDelay( 25 );
          if(buttons::btnA) {
            Beeper::decrease();
          }
          if(buttons::btnC) {
            Beeper::increase();
          }
        }
        if(buttons::btnB) {
          Beeper::save();
          exitSettingsMenu();
        }

        char textbox[20];
        snprintf( textbox, 19, "%s %d%s", Beeper::label, Beeper::beepVolume, "%" );
        drawSettingValue( textbox, true );
      }


      void onSetScreensaverDelay()
      {
        setMenuDelay( 300 );

        if(buttons::btnA || buttons::btnC) {
          setMenuDelay( 75 );
          if(buttons::btnA) {
            ScreenSaver::decrease();
          }
          if(buttons::btnC) {
            ScreenSaver::increase();
          }
        }

        if(buttons::btnB) {
          ScreenSaver::save();
          exitSettingsMenu();
        }

        char textbox[20];
        snprintf( textbox, 19, "%s %d%s", ScreenSaver::label, ScreenSaver::getDelay(), " MIN" );
        drawSettingValue( textbox, true );
      }


      void onShowIPAddress()
      {
        setMenuDelay( 300 );

        drawSettingValue( WiFi.localIP().toString().c_str(), true );

        if(buttons::btnB) {
          exitSettingsMenu();
          return;
        }
      }


      void onShowMacAddress()
      {
        setMenuDelay( 300 );

        drawSettingValue( WiFi.macAddress().c_str(), true );

        if(buttons::btnB) {
          exitSettingsMenu();
          return;
        }
      }


      void drawMenu()
      {
        for( int i=0; i<h; i+=2 ) {
          tft.drawRoundRect( x, y+(h/2)-(i/2), w, i, 8, SettingsMenuBgColor );
          vTaskDelay(2);
        }
        //tft.fillRoundRect( x, y, w, h, 8, SettingsMenuBgColor );
        tft.drawRoundRect( x, y, w, h, 8, SettingsMenuBorderColor );

        CSS::drawStyledString( &tft, SETTINGS_LABEL, hmiddle, 14 + y, &H1FontStyle );
        CSS::drawStyledString( &tft, APP_TITLE,      hmiddle, 28 + y, &H2FontStyle );

        tft.drawFastHLine( x + 1, rulerTitleYpos, w-2, SettingsMenuLightColor ); // <hr> between title and settings list
        tft.drawFastHLine( x + 1, rulerValueYpos, w-2, SettingsMenuLightColor ); // <hr> between settings list and setting value
      }


      void drawOptions()
      {
        uint8_t firstIndex = 0;

        // pagination
        if(choice > MAX_ITEMS_IN_SETTINGS_MENU-1) {
          firstIndex = choice - (MAX_ITEMS_IN_SETTINGS_MENU-1);
        }

        size_t lastIndex = firstIndex + MAX_ITEMS_IN_SETTINGS_MENU;

        CSS::setFontStyle( &tft, &LIFontStyle );
        uint32_t itemHeight  = (tft.fontHeight()+2);
        uint32_t itemsYStart = rulerTitleYpos + itemHeight/2;

        for(uint8_t j=0, i=firstIndex; i<lastIndex; i++) {
          uint32_t yPos = ( itemsYStart + (j * itemHeight) ) - itemHeight/2;
          if(choice == i ) {
            if( setting_selected ) {
              tft.setTextColor( SettingsMenuDarkColor, SettingsMenuLightColor );
            } else {
              tft.setTextColor( SettingsMenuBgColor, SettingsMenuLightColor );
            }
          } else {
            tft.setTextColor( setting_selected ? SettingsMenuDimmedColor : SettingsMenuLightColor, SettingsMenuBgColor);
          }
          CSS::drawStyledString( &tft, menuchoices[i].label, hmiddle, itemsYStart + (j * itemHeight), nullptr );
          if( i!=firstIndex ) {
            tft.drawFastHLine( x+1, yPos, w-2, (choice == i ) ? SettingsMenuLightColor : SettingsMenuBgColor); // padding-top: 1px for the font
          }
          j++;
          if(j > MAX_ITEMS_IN_SETTINGS_MENU) j = MAX_ITEMS_IN_SETTINGS_MENU;
        }
      }


    };

  };

};
