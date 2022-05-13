


namespace ICSMeter
{

  namespace UI
  {

    namespace Transverter
    {

      void draw()
      {
        if (ScreenSaver::mode || Settings::mode ) return;

        if (value > 0) {
          tft.fillRoundRect(62, 4, 26, 13, 2, TFT_MODE_BACK);
          tft.drawRoundRect(62, 4, 26, 13, 2, TFT_MODE_BORDER);
          tft.setTextColor(TFT_WHITE);
          tft.drawString("LO" + String(value), 76, 11);
        }
      }

    };

  };
};
