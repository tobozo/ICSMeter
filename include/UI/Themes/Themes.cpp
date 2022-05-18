#include "Themes.hpp"


namespace ICSMeter
{

  namespace UI
  {

    namespace Theme
    {

      using namespace CSS;

      int8_t theme = 0;
      int8_t themeOld = 0;


      void setup()
      {
        theme = getPref("theme", 0 );
        set();
      }


      void save()
      {
        int8_t tmp = getPref("theme", 0 );
        if( theme != tmp ) {
          setPref("theme", theme);
          themeOld = theme;
        }
      }


      void set()
      {
        layout = layouts[theme];
        H3FontStyle = H3FontStyleTheme[theme];
      }


      const image_t middle10Classic   = { 16, smeterMiddleClassic10 , sizeof(smeterMiddleClassic10  ), IMAGE_JPG, 320, 140 };
      const image_t middle100Classic  = { 16, smeterMiddleClassic100, sizeof(smeterMiddleClassic100 ), IMAGE_JPG, 320, 140 };
      const image_t topClassic        = { 16, smeterTopClassic      , sizeof(smeterTopClassic       ), IMAGE_JPG, 320, 20  };
      const image_t bottomClassic     = { 16, smeterBottomClassic   , sizeof(smeterBottomClassic    ), IMAGE_JPG, 320, 80  };
      const image_t *middleClassicPtr = (IC_MODEL == 705) ? &middle10Classic : &middle100Classic;

      const image_t middle10Dark      = { 16, smeterMiddleDark10 ,    sizeof(smeterMiddleDark10     ), IMAGE_JPG, 320, 140 };
      const image_t middle100Dark     = { 16, smeterMiddleDark100,    sizeof(smeterMiddleDark100    ), IMAGE_JPG, 320, 140 };
      const image_t topDark           = { 16, smeterTopDark         , sizeof(smeterTopDark          ), IMAGE_JPG, 320, 20  };
      const image_t bottomDark        = { 16, smeterBottomDark      , sizeof(smeterBottomDark       ), IMAGE_JPG, 320, 80  };
      const image_t *middleDarkPtr    = (IC_MODEL == 705) ? &middle10Dark : &middle100Dark;

      const layout_t layoutClassic = { &topClassic, middleClassicPtr, &bottomClassic, TFT_FRONT_CLASSIC, TFT_BACK_CLASSIC };
      const layout_t layoutDark    = { &topDark,    middleDarkPtr,    &bottomDark,    TFT_FRONT_DARK,    TFT_BACK_DARK    };

      //const image_t bgImageClassic   = { 16, smeterFullClassic,      smeterFullClassic_len,           IMAGE_JPG, 320, 240 };
      //const image_t bgImageDark      = { 16, smeterFullClassic,      smeterFullClassic_len,           IMAGE_JPG, 320, 240 };

      // CSS declarations

      const TextStyle_t BadgeTextStyle =
      {
        .fgColor   = SettingsMenuLightColor,
        .bgColor   = SettingsMenuBgColor,
        .size      = 1,
        .datum     = MC_DATUM,
        .paddingX  = 24
      };
      const FontStyle_t BadgeFontStyle = { &Font0, &BadgeTextStyle, OPAQUE };

      const TextBoxStyle_t BadgeBoxStyle =
      {
        .fontStyle    = &BadgeFontStyle,
        .fillColor    = SettingsMenuBgColor,
        .borderColor  = SettingsMenuBorderColor,
        .borderWidth  = 1,  // set to 0 to disable border
        .borderRadius = 2,
        .paddingX     = 0,
        .paddingY     = 0
      };

      // dark/classic text styles
      const TextStyle_t ConnectH3Classic =
      {
        .fgColor   = 0x000000U,
        .bgColor   = 0xfff8ecU,
        .size      = 1,
        .datum     = MC_DATUM,
        .paddingX  = 194
      };
      const TextStyle_t ConnectH3Dark =
      {
        .fgColor   = 0xfff8ecU,
        .bgColor   = 0x000000U,
        .size      = 1,
        .datum     = MC_DATUM,
        .paddingX  = 194
      };

      // dark/classic font styles
      const FontStyle_t H3FontStyleDark    = { &stencilie16pt7b, &ConnectH3Dark   , OPAQUE  };
      const FontStyle_t H3FontStyleClassic = { &stencilie16pt7b, &ConnectH3Classic, OPAQUE  };

      // Actual themed values
      const char        *choices[THEMES_COUNT]          = {"CLASSIC",            "DARK"}; // labels for settings menu
      const layout_t    *layouts[THEMES_COUNT]          = { &layoutClassic,      &layoutDark }; // pointers to layouts
      const FontStyle_t *H3FontStyleTheme[THEMES_COUNT] = { &H3FontStyleClassic, &H3FontStyleDark }; // pointers to font styles
      //const image_t *bgImageTheme[THEMES_COUNT] = { &bgImageClassic, &bgImageDark };


    };

  };

};

