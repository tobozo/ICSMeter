


namespace ICSMeter
{

  namespace UI
  {

    namespace Needle
    {

      using namespace Theme;

      float angle, oldAngle;

      const uint32_t TFT_NEEDLE_0      = 0xff0000U;
      const uint32_t TFT_NEDDLE_1      = 0xf17864U;
      const uint32_t TFT_NEDDLE_2      = 0xf18c78U;

      const int32_t axis_x = 160, axis_y = 220;


      void set( float _angle )
      {
        angle = _angle;
      }


      bool needs_redraw()
      {
        return ( angle!=oldAngle );
      }


      void draw()
      {
        draw(angle);
        oldAngle = angle;
      }


      void reset()
      {
        oldAngle = 0;
      }


      void draw(float_t angle, uint16_t a, uint16_t b, uint16_t c, uint16_t d)
      {
        uint16_t x, y;

        if (angle != oldAngle) {
          oldAngle = angle;

          x = a;
          y = b;

          rotate(&x, &y, angle);

          a = axis_x + x;
          b = axis_y - y;

          x = c;
          y = d;

          rotate(&x, &y, angle);

          c = axis_x + x;
          d = axis_y - y;

          drawMiddle();

          // tft.drawFastHLine(0, 150, 320, TFT_BLACK);

          tft.drawLine(a + 2, b, c + 3, d, TFT_NEDDLE_2);
          tft.drawLine(a + 2, b, c + 2, d, TFT_NEDDLE_1);
          tft.drawLine(a + 1, b, c + 1, d, TFT_NEEDLE_0);
          tft.drawLine(a,     b, c,     d, TFT_NEEDLE_0);
          tft.drawLine(a - 1, b, c - 1, d, TFT_NEEDLE_0);
          tft.drawLine(a - 2, b, c - 2, d, TFT_NEDDLE_1);
          tft.drawLine(a - 2, b, c - 3, d, TFT_NEDDLE_2);
        }
      }

    };

  };

};

