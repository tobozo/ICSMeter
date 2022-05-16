

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
        uint8_t batteryLevel;
        bool batteryCharging;

        if(ScreenSaver::mode || Settings::mode ) return;

        // On left, view battery level
        batteryLevel = map(getLevel(1), battery_min_level, battery_max_level, 0, 16);
        batteryCharging = isCharging();

        if( !force_redraw && ( batteryLevel == oldValue && batteryCharging == oldChargingState ) ) return;

        CSS::drawStyledBox( &tft, ICText.c_str(), 4, 4, 56, 13, &Theme::BadgeBoxStyle );

        oldValue = batteryLevel;
        oldChargingState = batteryCharging;

        // draw battery icon
        tft.drawRect(294, 4, 20, 12, Theme::layout->fgcolor);
        tft.drawRect(313, 7, 4,  6,  Theme::layout->fgcolor);
        tft.fillRect(296, 6, batteryLevel, 8, Theme::layout->fgcolor);

        CSS::setFontStyle( &tft, Theme::BadgeBoxStyle.fontStyle );

        tft.setTextPadding(0);
        tft.setTextColor(Theme::layout->fgcolor);

        if (batteryCharging) {
          CSS::drawStyledString( &tft, "+", 290, 11, nullptr );
        } else {
          tft.setTextDatum(MR_DATUM);
          String val = String(getLevel(1)) + "%"; // TODO: sprintf this
          CSS::drawStyledString( &tft, val.c_str(), 290, 11, nullptr );
        }
      }


    };
  };
};
