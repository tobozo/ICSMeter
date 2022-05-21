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
        .paddingX  =  w - 2
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
        { "Shutdown",         onShutdown              },
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
            choice = 0;
            Settings::draw();
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

        if( !dialog_enabled ) { // exiting settings menu
          exitSettingsMenu();
        }
      }


      void draw()
      {
        drawMenu();
        drawOptions();
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
          log_d("Leaving settings menu");
          dialog_enabled   = false;
          UI::drawWidgets( true );
        }
      }

      void onShutdown()
      {
        CSS::drawStyledString( &tft, "", hmiddle, titleYpos, &ULFontStyle );
        if(buttons::btnB) {
          shutdown();
        }
      }


      void onExit()
      {
        CSS::drawStyledString( &tft, "", hmiddle, titleYpos, &ULFontStyle );
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


      void onChangeMeasuredValues()
      {
        setMenuDelay( 300 );
        if(buttons::btnA || buttons::btnC) {
          if(buttons::btnA) {
            Measure::value -= 1;
            if(Measure::value < 0) Measure::value = 2;
          }
          if(buttons::btnC) {
            Measure::value += 1;
            if(Measure::value > 2) Measure::value = 0;
          }
          Measure::drawLabels( true );
        }

        CSS::drawStyledString( &tft, Measure::choices[Measure::value], hmiddle, titleYpos, &ULFontStyle );

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
          UI::drawWidgets( true );
          drawMenu();
        }

        CSS::drawStyledString( &tft, Theme::choices[Theme::theme], hmiddle, titleYpos, &ULFontStyle );

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
        CSS::drawStyledString( &tft, textbox, hmiddle, titleYpos, &ULFontStyle );
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
          CSS::drawStyledString( &tft, "OFF", hmiddle, titleYpos, &ULFontStyle );
        } else {
          char transverter_value_str[17];
          format_number( Transverter::choices[value], 16, transverter_value_str, '.' );
          CSS::drawStyledString( &tft, transverter_value_str, hmiddle, titleYpos, &ULFontStyle );
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
        CSS::drawStyledString( &tft, textbox, hmiddle, titleYpos, &ULFontStyle );
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
        CSS::drawStyledString( &tft, textbox, hmiddle, titleYpos, &ULFontStyle );
      }


      void onShowIPAddress()
      {
        setMenuDelay( 300 );

        CSS::drawStyledString( &tft, WiFi.localIP().toString().c_str(), hmiddle, titleYpos, &ULFontStyle );

        if(buttons::btnB) {
          exitSettingsMenu();
          return;
        }
      }


      void onShowMacAddress()
      {
        setMenuDelay( 300 );

        CSS::drawStyledString( &tft, WiFi.macAddress().c_str(), hmiddle, titleYpos, &ULFontStyle );

        if(buttons::btnB) {
          exitSettingsMenu();
          return;
        }
      }


      void drawMenu(/*uint8_t x, uint8_t y, uint16_t w, uint8_t h*/)
      {
        tft.fillRoundRect( x, y, w, h, 8, SettingsMenuBgColor );
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
