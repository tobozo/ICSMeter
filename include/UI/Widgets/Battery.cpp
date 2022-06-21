
namespace ICSMeter
{

  namespace UI
  {

    namespace Battery
    {

      using namespace net;

      uint8_t oldValue = 0;
      bool oldChargingState = true;
      //String ICText;// = String(IC_MODEL) + String(IC_CONNECT == BT ? " BT" : " USB");
      const uint8_t battery_min_level = 0;
      const uint8_t battery_max_level = 100;

      constexpr clipRect_t ComClip =
      {
        .x = 4,
        .y = 4,
        .w = 56,
        .h = 13
      };

      constexpr clipRect_t BattClip =
      {
        .x = 313,
        .y = 7,
        .w = 4,
        .h = 6
      };

      constexpr clipRect_t ChargeClip =
      {
        .x = 294,
        .y = 4,
        .w = 20,
        .h = 12
      };


      int8_t getLevel() // Get Battery level
      {
        return M5.Power.getBatteryLevel();
      }


      bool isCharging() // Control if charging
      {
        return M5.Power.isCharging();
      }


      void draw( bool force_redraw ) // Print battery
      {
        int8_t rawLevel = Battery::getLevel();

        // TODO: move this condition to UI controller
        if(ScreenSaver::isEnabled() || Settings::dialog_enabled ) return;

        // On left, view battery level
        uint8_t batteryLevel = map(rawLevel, battery_min_level, battery_max_level, 0, 16);
        bool batteryCharging = isCharging();

        if( !force_redraw && ( batteryLevel == oldValue && batteryCharging == oldChargingState ) ) return;

        String ICText = String( CIV::IC->model ) + String( CIV::IC->type == IC_COMM_BLUETOOTH ? " BT" : " USB");
        CSS::drawStyledBox( &tft, ICText.c_str(), ComClip.x, ComClip.y, ComClip.w, ComClip.h, &Theme::BadgeBoxStyle );

        oldValue = batteryLevel;
        oldChargingState = batteryCharging;

        // draw battery connector
        tft.drawRect( BattClip.x, BattClip.y, BattClip.w, BattClip.h,  Theme::layout->fgcolor);

        if( rawLevel>=0 ) { // battery level available, fill battery

          tft.drawRect( ChargeClip.x, ChargeClip.y, ChargeClip.w, ChargeClip.h, Theme::layout->fgcolor);
          tft.fillRect( ChargeClip.x+2, ChargeClip.y+2, batteryLevel, 8, Theme::layout->fgcolor);

        } else { // no battery level available, draw "broken" battery icon

          int16_t x = ChargeClip.x,
                  y = ChargeClip.y,
                  w = ChargeClip.w,
                  h = ChargeClip.h
          ;

          const int16_t crackwidth = 4; // px
          const int16_t xoffset    = 5; // horizontal offset, changes the angle
          const int16_t x1 = x+w/2;     // put the crack in the middle
          const int16_t x2 = x1-xoffset;
          const int16_t y1 = y+h-1;
          const int16_t l1 = x1-x;
          const int16_t l2 = 20-(l1+crackwidth);
          const int16_t l3 = x2-x;
          const int16_t l4 = 20-(l3+crackwidth);

          tft.drawFastHLine( x,             y, l1, Theme::layout->fgcolor );
          tft.drawFastHLine( x1+crackwidth, y, l2, Theme::layout->fgcolor );

          tft.drawFastHLine( x,             y1, l3, Theme::layout->fgcolor );
          tft.drawFastHLine( x2+crackwidth, y1, l4, Theme::layout->fgcolor );

          tft.drawFastVLine( x,     y, h, Theme::layout->fgcolor );
          tft.drawFastVLine( x+w-1, y, h, Theme::layout->fgcolor );

          tft.drawLine( x1,            y, x2,            y1, Theme::layout->fgcolor );
          tft.drawLine( x1+crackwidth, y, x2+crackwidth, y1, Theme::layout->fgcolor );

        }

        CSS::setFontStyle( &tft, Theme::BadgeBoxStyle.fontStyle );

        tft.setTextPadding(0);
        tft.setTextColor(Theme::layout->fgcolor);

        String val;

        if (batteryCharging) {
          val = "+";
        } else {

          if( rawLevel<0 ) { // battery broken or module can't be queried
            val = "??";
          } else {
            val = String(rawLevel) + "%"; // TODO: snprintf this
          }
          tft.setTextDatum(MR_DATUM);
        }
        CSS::drawStyledString( &tft, val.c_str(), 290, 11, nullptr );
      }


    };
  };
};
