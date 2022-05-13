


namespace ICSMeter
{

  namespace UI
  {

    namespace Measure
    {

      using namespace Theme;


      void draw()
      {
        uint16_t i = 65;
        uint8_t j;

        if (value != valueOld) {

          valueOld = value;

          tft.setTextDatum(CC_DATUM);
          tft.setFont(&YELLOWCRE8pt7b);
          tft.setTextPadding(0);

          for (j = 0; j <= 2; j++) {
            if (value == j) {
              tft.setTextColor(Theme::fgcolor);
              reset = true;
            } else {
              tft.setTextColor(TFT_DARKGREY);
            }

            tft.drawString(choices[j], i, 230);
            i += 95;
          }
        }
      }


      void setValue(String valString, uint8_t x, uint8_t y)
      {
        if (valString != valStringOld)
        {
          valStringOld = valString;

          tft.setTextDatum(CC_DATUM);
          tft.setFont(&stencilie16pt7b);
          valString.replace(".", ",");
          //tft.setFont(&YELLOWCRE8pt7b);
          tft.setTextPadding(190);
          tft.setTextColor(Theme::fgcolor, Theme::bgcolor);
          tft.drawString(valString, x, y);
        }
      }


      void subValue(String valString, uint8_t x, uint8_t y)
      {
        if (valString != subValStringOld)
        {
          subValStringOld = valString;

          tft.setTextDatum(CC_DATUM);
          tft.setFont(&YELLOWCRE8pt7b);
          tft.setTextPadding(160);
          //tft.setTextColor(TFT_BLACK, TFT_RED);
          tft.setTextColor(Theme::fgcolor, Theme::bgcolor);
          // valString.replace(".", ",");
          tft.drawString(valString, x, y);
        }
      }


    };


  };
};

