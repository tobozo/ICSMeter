
namespace ICSMeter
{

  namespace UI
  {

    namespace Battery
    {

      uint8_t oldValue = 0;
      bool oldChargingState = true;
      const String ICText = String(IC_MODEL) + String(IC_CONNECT == BT ? " BT" : " USB");
      const uint8_t battery_min_level = 0;
      const uint8_t battery_max_level = 100;


      int8_t getLevel(bool type) // Get Battery level
      {
        return M5.Power.getBatteryLevel();
      }


      bool isCharging() // Control if charging
      {
        return M5.Power.isCharging();
      }


      void reset()
      {
        oldValue         = 0;
        oldChargingState = true;
      }


      void draw( bool force_redraw ) // Print battery
      {
        int8_t rawLevel = getLevel(1);
        bool batteryCharging;

        if(ScreenSaver::isEnabled() || Settings::dialog_enabled ) return;

        // On left, view battery level
        uint8_t batteryLevel = map(rawLevel, battery_min_level, battery_max_level, 0, 16);
        batteryCharging = isCharging();

        if( !force_redraw && ( batteryLevel == oldValue && batteryCharging == oldChargingState ) ) return;

        CSS::drawStyledBox( &tft, ICText.c_str(), 4, 4, 56, 13, &Theme::BadgeBoxStyle );

        oldValue = batteryLevel;
        oldChargingState = batteryCharging;

        // draw battery icon
        tft.drawRect(313, 7, 4,  6,  Theme::layout->fgcolor);

        int16_t x = 294, y = 4, w=20, h=12;

        if( rawLevel>=0 ) { // battery level available, fill battery

          tft.drawRect(x, y, w, h, Theme::layout->fgcolor);
          tft.fillRect(x+2, y+2, batteryLevel, 8, Theme::layout->fgcolor);

        } else { // no battery level available, draw "broken" battery icon

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
