#include "Battery.hpp"

#include "../../Utils/Maths.hpp"
#include "../../net/Controls.hpp"
#include "../Controls.hpp"
#include "../UI.hpp"


namespace ICSMeter
{

  namespace UI
  {


    namespace Battery
    {

      using namespace Theme;


      // Get Battery level
      int8_t getLevel(bool type)
      {
        return M5.Power.getBatteryLevel();
      }

      // Control if charging
      bool isCharging()
      {
        return M5.Power.isCharging();
      }


      // Print battery
      void draw()
      {
        uint8_t batteryLevel;
        bool batteryCharging;

        if (ScreenSaver::mode || Settings::mode ) return;

        // On left, view battery level
        batteryLevel = map(getBatteryLevel(1), 0, 100, 0, 16);
        batteryCharging = isCharging();

        if( batteryLevel == levelOld && batteryCharging == charginglOld ) return;

        /*
        if(theme == 0) {
          tft.drawJpg(smeterTopClassic, sizeof(smeterTopClassic), 0, 0, 320, 20);
        } else {
          tft.drawJpg(smeterTopDark, sizeof(smeterTopDark), 0, 0, 320, 20);
        }
        */

        drawTop();

        // Settings
        tft.setFont(0);
        tft.setTextDatum(CC_DATUM);

        tft.fillRoundRect(4, 4, 56, 13, 2, TFT_MODE_BACK);
        tft.drawRoundRect(4, 4, 56, 13, 2, TFT_MODE_BORDER);
        tft.setTextColor(TFT_WHITE);

        if (IC_CONNECT == BT) {
          tft.drawString(String(IC_MODEL) + " BT", 32, 11);
        } else {
          tft.drawString(String(IC_MODEL) + " USB", 32, 11);
        }

        /*
        if (transverter > 0) {
          tft.fillRoundRect(62, 4, 26, 13, 2, TFT_MODE_BACK);
          tft.drawRoundRect(62, 4, 26, 13, 2, TFT_MODE_BORDER);
          tft.setTextColor(TFT_WHITE);
          tft.drawString("LO" + String(transverter), 76, 11);
        }
        */

        // tft.drawFastHLine(0, 20, 320, TFT_BLACK);

        levelOld = batteryLevel;
        charginglOld = batteryCharging;

        tft.drawRect(294, 4, 20, 12, TFT_FRONT);
        tft.drawRect(313, 7, 4, 6, TFT_FRONT);
        tft.fillRect(296, 6, batteryLevel, 8, TFT_FRONT);

        if (batteryCharging) {
          tft.setTextColor(TFT_FRONT);
          tft.setFont(0);
          tft.setTextDatum(CC_DATUM);
          tft.setTextPadding(0);
          tft.drawString("+", 290, 11);
        } else {
          tft.setTextColor(TFT_FRONT);
          tft.setFont(0);
          tft.setTextDatum(CR_DATUM);
          tft.setTextPadding(0);
          tft.drawString(String(getBatteryLevel(1)) + "%", 290, 11);
        }

      }


    };
  };
};
