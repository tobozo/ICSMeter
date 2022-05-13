


namespace ICSMeter
{

  namespace UI
  {

    namespace Needle
    {

      using namespace Theme;

      void draw(float_t angle, uint16_t a, uint16_t b, uint16_t c, uint16_t d)
      {
        uint16_t x, y;

        if (angle != angleOld) {
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


          drawMiddle();

          // tft.drawFastHLine(0, 150, 320, TFT_BLACK);

          tft.drawLine(a + 2, b, c + 3, d, TFT_NEDDLE_2);
          tft.drawLine(a + 2, b, c + 2, d, TFT_NEDDLE_1);
          tft.drawLine(a + 1, b, c + 1, d, TFT_RED);
          tft.drawLine(a, b, c, d, TFT_RED);
          tft.drawLine(a - 1, b, c - 1, d, TFT_RED);
          tft.drawLine(a - 2, b, c - 2, d, TFT_NEDDLE_1);
          tft.drawLine(a - 2, b, c - 3, d, TFT_NEDDLE_2);
        }
      }
    };

  };
};

