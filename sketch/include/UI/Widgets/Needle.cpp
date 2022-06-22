#line 1 "/home/runner/work/ICSMeter/ICSMeter/include/UI/Widgets/Needle.cpp"

namespace ICSMeter
{

  namespace UI
  {

    namespace Needle
    {

      using namespace net;

      float angle, oldAngle;
      String label, oldLabel;

      const uint32_t TFT_NEEDLE_0      = 0xff0000U;
      const uint32_t TFT_NEDDLE_1      = 0xf17864U;
      const uint32_t TFT_NEDDLE_2      = 0xf18c78U;

      // needle axis (under clip horizon)
      const int32_t axis_x = 160, axis_y = 220;

      // gauge geometry
      constexpr clipRect_t GaugeClip  =
      {
        .x = 0,
        .y = 20,
        .w = 320,
        .h = 120
      };

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

      }; // end namespace ICS_Gauge


      constexpr clipRect_t LabelClip =
      {
        .x = 160,
        .y = 180,
        .w = 194,
        .h = 0
      };

      const CSS::TextStyle_t labelTextStyle =
      {
        .fgColor   = 0x000000U,
        .bgColor   = 0xffffffU,
        .size      = 1,
        .datum     = MC_DATUM,
        .paddingX  = LabelClip.w
      };

      const CSS::FontStyle_t labelFontStyle   = { &stencilie16pt7b, &labelTextStyle  , CSS::OPAQUE };


      Gauge_Class  *ICSGauge    = nullptr;

      void setup()
      {
        auto cfg = LGFXMeter::config( ICS_Gauge::cfg );

        // attach the gauge to TFT
        cfg.display = &M5.Lcd;
        // set gauge geometry
        cfg.clipRect  = GaugeClip;
        cfg.needle.axis = { axis_x, axis_y-GaugeClip.y };
        ICSGauge = new Gauge_Class( cfg );
        onThemeChange();
      }


      void onThemeChange()
      {
        needle::cfg.transparent_color = Theme::layout->bgcolor; // use theme's bgcolor as needle transparency color
        needle::cfg.drop_shadow       = Theme::theme == 0;      // toggle needle shadow (useless in dark mode)
        ICSGauge->createNeedle();                               // force needle redraw
        ICSGauge->setNeedle( ICS_Gauge::cfg.start );
        //angle = ICS_Gauge::cfg.start;
        utils::drawImage( ICSGauge->getGaugeSprite(), Theme::layout->middleImage, 0, 0 ); // attach themed background to the gauge,
      }


      void setAngle( float _angle )
      {
        angle = _angle;
      }


      void updateValues()
      {
        switch( Measure::mode ) {
          case Measure::MODE_PWR: angle = CIV::getPWRAngle();    label = CIV::getPWRLabel();    break;
          case Measure::MODE_SMT: angle = CIV::getSmeterAngle(); label = CIV::getSmeterLabel(); break;
          case Measure::MODE_SWR: angle = CIV::getSWRAngle();    label = CIV::getSWRLabel();    break;
          default:
            // this can't happen since Measure::mode is enum, but who knows?
            log_e("Unsupported measure mode: 0x%02x",  Measure::mode );
          break;
        }
      }


      void setLabel( String _label )
      {
        label = _label;
      }


      bool angle_needs_redraw()
      {
        return ( angle!=oldAngle );
      }


      bool label_needs_redraw()
      {
        return (label!=oldLabel);
      }


      bool needs_redraw()
      {
        updateValues();
        return angle_needs_redraw() || label_needs_redraw();
      }


      void draw( bool force_redraw )
      {
        if( !needs_redraw() && !force_redraw ) return;
        if( force_redraw || label_needs_redraw() ) {
          drawLabel( label );
        }
        if( force_redraw || angle_needs_redraw() ) {
          drawNeedle( angle );
          //ICSGauge->drawNeedle( angle + 45.0 );
          //oldAngle = angle;
        }
      }


      void drawLabel( String _label )
      {
        oldLabel = _label;
        setFontStyle( &tft, &labelFontStyle );
        _label.replace(".", ",");
        tft.setTextColor(Theme::layout->fgcolor, Theme::layout->bgcolor);
        if( _label != "" )
          CSS::drawStyledString( &tft, _label.c_str(), LabelClip.x, LabelClip.y, nullptr );
      }



      void drawNeedle( float_t angle )
      {
        oldAngle = angle;
        //ICSGauge->drawNeedle( angle + 45.0 );
        //ICSGauge->animateNeedle( angle + 45.0 );
        ICSGauge->setNeedle( angle + 45.0 );
        uint32_t now = millis();
        while( now + 300 > millis() ) {
          ICSGauge->easeNeedle( 300 );
        }

      }


    };

  };

};

