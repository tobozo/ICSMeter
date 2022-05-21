#include "../Controls.hpp"
#include "../../UI/Widgets.hpp"
#include "../../Utils/Maths.hpp"

namespace ICSMeter
{

  namespace net
  {

    namespace CIV
    {

      using namespace UI;
      using namespace Utils;
      using namespace daemon;

      char buffer[8]; // shared buffer
      char request[7] = { 0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x00, 0x00, 0xfd }; // basic CIV request (3 last bytes=command)

      constexpr const command_t CIV_GET_TX        = { 0x1c, 0x00, 0xfd }; // Send/read the transceiver’s status (00=RX, 01=TX)
      constexpr const command_t CIV_GET_DATA_MODE = { 0x1a, 0x06, 0xfd }; // Send/read the DATA mode setting
      constexpr const command_t CIV_GET_SMETER    = { 0x15, 0x02, 0xfd }; // Read S-meter level (0000=S0, 0120=S9, 0241=S9+60 dB)
      constexpr const command_t CIV_GET_PWR       = { 0x15, 0x11, 0xfd }; // Read the Po meter level (0000=0% ~ 0143=50% ~ 0213=100%)
      constexpr const command_t CIV_GET_SWR       = { 0x15, 0x12, 0xfd }; // Read SWR meter level (0000=SWR1.0, 0048=SWR1.5, 0080=SWR2.0, 0120=SWR3.0)
      constexpr const command_t CIV_GET_FRQ       = { 0xe0, 0x03, 0xfd }; // ??
      constexpr const command_t CIV_GET_MOD       = { 0xe0, 0x04, 0xfd }; // ??

      constexpr const char *filterModes[] = { "LSB", "USB", "AM", "CW", "RTTY", "FM", "WFM", "CW-R", "RTTY-R", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "DV" };
      constexpr const uint32_t decMulti[] = { 1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1 };


      void setRequest( command_t cmd )
      {
        memcpy( (char*)&request[4], cmd.bytes, 3 );
      }


      uint8_t getTX()
      {
        uint8_t value;
        setRequest( CIV_GET_TX );
        size_t n = sizeof(request) / sizeof(request[0]);
        if( ! dispatchCommand((char*)request, n, buffer, 5) ) return 0;
        if (buffer[4] <= 1) {
          value = buffer[4];
        } else {
          value = 0;
        }
        return value;
      }


      char getDataMode()
      {
        setRequest( CIV_GET_DATA_MODE );
        size_t n = sizeof(request) / sizeof(request[0]);
        if( ! dispatchCommand(request, n, buffer, 6) ) return 0;
        return buffer[4];
      }


      GaugeMeasure_t getSmeter()
      {
        String valString;
        setRequest( CIV_GET_SMETER );
        char str[2];
        uint8_t val0 = 0;
        float_t val1 = 0;
        static uint8_t val2 = 0;
        float_t angle = 0;
        GaugeMeasure_t ret = {0,""};
        size_t n = sizeof(request) / sizeof(request[0]);

        if( ! dispatchCommand(request, n, buffer, 6) ) return ret;

        sprintf(str, "%02x%02x", buffer[4], buffer[5]);
        val0 = atoi(str);

        if (val0 <= 120) { // 120 = S9 = 9 * (40/3)
          val1 = val0 / (40 / 3.0f);
        } else { // 240 = S9 + 60
          val1 = (val0 - 120) / 2.0f;
        }

        if (abs(val0 - val2) > 1 || Needle::force_reset == true) {
          val2 = val0;
          Needle::force_reset = false;

          if (val0 <= 13) {
            angle = 42.50f;
            valString = "S " + String(int(round(val1)));
          } else if (val0 <= 120) {
            angle = mapFloat(val0, 14, 120, 42.50f, -6.50f); // SMeter image start at S1 so S0 is out of image on the left...
            valString = "S " + String(int(round(val1)));
          } else {
            angle = mapFloat(val0, 121, 241, -6.50f, -43.0f);
            if (int(round(val1) < 10))
              valString = "S 9 + 0" + String(int(round(val1))) + " dB";
            else
              valString = "S 9 + " + String(int(round(val1))) + " dB";
          }

          // Debug trace
          #if DEBUG==1
            Serial.println("Get S" + String (val0) + " "+ String(val1) + " " +String(angle) );
          #endif

          ret = { angle, valString };
        }
        return ret;
      }


      GaugeMeasure_t getSWR()
      {
        String valString;
        setRequest( CIV_GET_SWR );
        char str[2];
        uint8_t val0 = 0;
        float_t val1 = 0;
        static uint8_t val3 = 0;
        float_t angle = 0;
        size_t n = sizeof(request) / sizeof(request[0]);
        GaugeMeasure_t ret = {0,""};

        if( ! dispatchCommand(request, n, buffer, 6) ) return ret;

        sprintf(str, "%02x%02x", buffer[4], buffer[5]);
        val0 = atoi(str);

        if (val0 != val3 || Needle::force_reset == true) {
          val3 = val0;
          Needle::force_reset = false;

          if (val0 <= 48) {
            angle = mapFloat(val0, 0, 48, 42.50f, 32.50f);
            val1 = mapFloat(val0, 0, 48, 1.0, 1.5);
          } else if (val0 <= 80) {
            angle = mapFloat(val0, 49, 80, 32.50f, 24.0f);
            val1 = mapFloat(val0, 49, 80, 1.5, 2.0);
          } else if (val0 <= 120) {
            angle = mapFloat(val0, 81, 120, 24.0f, 10.0f);
            val1 = mapFloat(val0, 81, 120, 2.0, 3.0);
          } else if (val0 <= 155) {
            angle = mapFloat(val0, 121, 155, 10.0f, 0.0f);
            val1 = mapFloat(val0, 121, 155, 3.0, 4.0);
          } else if (val0 <= 175) {
            angle = mapFloat(val0, 156, 175, 0.0f, -7.0f);
            val1 = mapFloat(val0, 156, 175, 4.0, 5.0);
          } else if (val0 <= 225) {
            angle = mapFloat(val0, 176, 225, -7.0f, -19.0f);
            val1 = mapFloat(val0, 176, 225, 5.0, 10.0);
          } else {
            angle = mapFloat(val0, 226, 255, -19.0f, -30.50f);
            val1 = mapFloat(val0, 226, 255, 10.0, 50.0);
          }

          valString = "SWR " + String(val1);

          #if DEBUG==1
            Serial.println("Get SWR" + String (val0) + " "+ String(val1) + " " +String(angle) );
          #endif

          ret = { angle, valString };
        }
        return ret;
      }


      GaugeMeasure_t getPower()
      {
        String valString;
        setRequest( CIV_GET_PWR );
        char str[2];
        uint8_t val0 = 0;
        float_t val1 = 0;
        float_t val2 = 0;
        static uint8_t val3 = 0;
        float_t angle = 0;
        size_t n = sizeof(request) / sizeof(request[0]);
        GaugeMeasure_t ret = {0,""};

        if( !dispatchCommand(request, n, buffer, 6) ) return ret;

        sprintf(str, "%02x%02x", buffer[4], buffer[5]);
        val0 = atoi(str);

        if (val0 != val3 || Needle::force_reset == true) {
          val3 = val0;
          Needle::force_reset = false;

          if (val0 <= 27) {
            angle = mapFloat(val0, 0, 27, 42.50f, 30.50f);
            val1 = mapFloat(val0, 0, 27, 0, 0.5);
          } else if (val0 <= 49) {
            angle = mapFloat(val0, 28, 49, 30.50f, 23.50f);
            val1 = mapFloat(val0, 28, 49, 0.5, 1.0);
          } else if (val0 <= 78) {
            angle = mapFloat(val0, 50, 78, 23.50f, 14.50f);
            val1 = mapFloat(val0, 50, 78, 1.0, 2.0);
          } else if (val0 <= 104) {
            angle = mapFloat(val0, 79, 104, 14.50f, 6.30f);
            val1 = mapFloat(val0, 79, 104, 2.0, 3.0);
          } else if (val0 <= 143) {
            angle = mapFloat(val0, 105, 143, 6.30f, -6.50f);
            val1 = mapFloat(val0, 105, 143, 3.0, 5.0);
          } else if (val0 <= 175) {
            angle = mapFloat(val0, 144, 175, -6.50f, -17.50f);
            val1 = mapFloat(val0, 144, 175, 5.0, 7.0);
          } else {
            angle = mapFloat(val0, 176, 226, -17.50f, -30.50f);
            val1 = mapFloat(val0, 176, 226, 7.0, 10.0);
          }

          val2 = round(val1 * 10);
          if (IC_MODEL == 705)
            valString = "PWR " + String((val2 / 10)) + " W";
          else
            valString = "PWR " + String(val2) + " W";

          // Debug trace
          #if DEBUG==1
            Serial.println("Get PWR" + String (val0) + " "+ String(val1) + " " +String(angle) );
          #endif

          ret = { angle, valString };
        }
        return ret;
      }


      String getFrequency()
      {
        setRequest( CIV_GET_FRQ );
        double freq; // Current frequency in Hz
        size_t n = sizeof(request) / sizeof(request[0]);
        if( ! dispatchCommand((char*)request, n, buffer, 8) )  return "";

        freq = 0;
        for (uint8_t i = 2; i < 7; i++) {
          freq += (buffer[9 - i] >> 4) * decMulti[(i - 2) * 2];
          freq += (buffer[9 - i] & 0x0F) * decMulti[(i - 2) * 2 + 1];
        }

        if (Transverter::get() > 0) {
          freq += double(Transverter::choices[Transverter::get()]);
        }

        String ret;

        if( freq != 0 ) {
          char measure_value_str[17];
          format_number( freq, 16, measure_value_str, '.' );
          ret = String( measure_value_str );
        } else {
          ret = "-";
        }

        return ret;
      }


      FilterMode_t getMode()
      {
        setRequest( CIV_GET_MOD );
        size_t n = sizeof(request) / sizeof(request[0]);
        FilterMode_t ret = {"", ""};
        if( ! dispatchCommand((char*)request, n, buffer, 5) ) return ret;

        ret.filter = "FIL" + String(uint8_t(buffer[4]));
        ret.mode   = String(filterModes[(uint8_t)buffer[3]]);

        getDataMode(); // query CIV: is data ON or OFF ?

        if (DataMode::mode == 1) {
          ret.mode += "-D";
        }
        return ret;
      }


    };

  };

};
