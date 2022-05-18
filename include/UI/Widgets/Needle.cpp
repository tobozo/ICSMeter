


namespace ICSMeter
{

  namespace UI
  {


    namespace ICS_Gauge
    {

      // define a color palette
      static constexpr gauge_palette_t palette =
      {
        0xffffffU, /*.transparent_color  */
        0x222222U, /*.fill_color         */
        0xff4444U, /*.warn_color         */
        0x00ff00U, /*.ok_color           */
        0xff2222U, /*.arrow_color        */
        0xaaaaaaU, /*.arrow_border_color */
        0x888888U, /*.arrow_shadow_color */
      };

      static constexpr gauge_t cfg =
      {
        .items       = nullptr,
        .items_count = 0,
        .palette     = palette,
        .start       = -45.0f,
        .end         =  45.0f
      };

    }; // end namespace IC705_Gauge




    namespace Needle
    {

      using namespace Theme;

      float angle, oldAngle;

      const uint32_t TFT_NEEDLE_0      = 0xff0000U;
      const uint32_t TFT_NEDDLE_1      = 0xf17864U;
      const uint32_t TFT_NEDDLE_2      = 0xf18c78U;

      const int32_t axis_x = 160, axis_y = 220;

      const int32_t GaugeWidth  = 320;
      const int32_t GaugeHeight = 120;
      const int32_t GaugePosX   = 0;
      const int32_t GaugePosY   = 20;

      Gauge_Class  *ICSGauge    = nullptr;

      void setup()
      {
        auto cfg = LGFXMeter::config( ICS_Gauge::cfg );

        // attach the gauge to TFT
        cfg.display = &M5.Lcd;
        // set gauge geometry
        cfg.clipRect  = {
          .x         = GaugePosX,
          .y         = GaugePosY,
          .w         = GaugeWidth,
          .h         = GaugeHeight
        };
        cfg.needle.axis = { axis_x, axis_y-GaugePosY };
        ICSGauge = new Gauge_Class( cfg );
        onThemeChange();
      }


      void onThemeChange()
      {
        // use theme's bgcolor as needle transparency color
        needle::cfg.transparent_color = Theme::layout->bgcolor;
        // toggle needle shadow (useless in dark mode)
        needle::cfg.drop_shadow       = (Theme::theme == 1) ? false : true;
        // force needle redraw
        ICSGauge->createNeedle();
         // attach themed background to the gauge,
        utils::drawImage( ICSGauge->getGaugeSprite(), Theme::layout->middleImage, 0, 0 );
      }



      void set( float _angle )
      {
        angle = _angle;
      }


      bool needs_redraw()
      {
        return ( angle!=oldAngle );
      }


      void draw( bool force_redraw )
      {
        if( force_redraw || oldAngle != angle ) {
          ICSGauge->drawNeedle( angle + 45.0 );
          //draw(angle);
          oldAngle = angle;
        }
      }


      void reset()
      {
        oldAngle = 0;
      }

      void draw(float_t angle, uint16_t a, uint16_t b, uint16_t c, uint16_t d)
      {
        ICSGauge->animateNeedle( angle );
      }


/*
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
*/

    };

  };

};

