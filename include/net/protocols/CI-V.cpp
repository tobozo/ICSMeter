#include "../Daemon.hpp"
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

      constexpr const char *filterModes[] = { "LSB", "USB", "AM", "CW", "RTTY", "FM", "WFM", "CW-R", "RTTY-R", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "DV" };
      constexpr const uint32_t decMulti[] = { 1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1 };

      const GaugeMap_t SWR_Map[7] =
      {
        {   0,  48,  42.5f,  32.5f,  1.0,  1.5 },
        {  49,  80,  32.5f,  24.0f,  1.5,  2.0 },
        {  81, 120,  24.0f,  10.0f,  2.0,  3.0 },
        { 121, 155,  10.0f,   0.0f,  3.0,  4.0 },
        { 156, 175,   0.0f,  -7.0f,  4.0,  5.0 },
        { 176, 225,  -7.0f, -19.0f,  5.0, 10.0 },
        { 226, 255, -19.0f, -30.5f, 10.0, 50.0 }
      };

      const GaugeMap_t PWR_Map[7] =
      {
        {   0,  27,  42.50f,  30.50f, 0.0,  0.5 },
        {  28,  49,  30.50f,  23.50f, 0.5,  1.0 },
        {  50,  78,  23.50f,  14.50f, 1.0,  2.0 },
        {  79, 104,  14.50f,   6.30f, 2.0,  3.0 },
        { 105, 143,   6.30f,  -6.50f, 3.0,  5.0 },
        { 144, 175,  -6.50f, -17.50f, 5.0,  7.0 },
        { 176, 255, -17.50f, -30.50f, 7.0, 10.0 }
      };


      char buffer[8]; // shared buffer
      char request[7] = { CIV_HDR, CIV_HDR, CIV_ADR, CIV_STA, CIV_NUL, CIV_NUL, CIV_END }; // basic CIV request (3 last bytes=command)
      size_t request_size = sizeof(request) / sizeof(request[0]);


      void setup()
      {
        uint8_t choice  = prefs::get("ic-choice", 0 );
        uint8_t address = prefs::get("ic-address", 0x4e );
        log_d("Loaded IC Choice #%d / Address 0x%02x from prefs", choice, address );
        setIC( choice, address );
      }


      void loop()
      {
        if( last_poll + poll_timeout < millis() ) {
          CIV::txConnected = false;
        } else {
          CIV::txConnected = true;
        }
      }


      void save()
      {
        bool saved = false;
        uint8_t old_choice = prefs::get("ic-choice", 0 );
        if( old_choice != CIV::IC_CHOICE ) {
          log_d("Saving IC Choice 0x%02x", CIV::IC_CHOICE );
          prefs::set("ic-choice", CIV::IC_CHOICE );
          saved = true;
        }
        uint8_t old_address = prefs::get("ic-address", 0x4e );
        if( old_address != CIV::CIV_ADR ) {
          log_d("Saving IC Address 0x%02x", CIV::CIV_ADR );
          prefs::set("ic-address", CIV::CIV_ADR );
          saved = true;
        }
        if( saved == true ) {
          ESP.restart();
        }
      }


      void setIC( uint8_t choice, uint8_t address )
      {
        setICModel( choice );
        setICAddress( address );
      }


      void setICModel( uint8_t choice )
      {
        int8_t old_address = CIV::IC ? CIV::IC_CHOICE : -1;
        size_t combos_count = (sizeof(IC_COMBOS)/sizeof(IC_DAEMON_t));
        CIV::IC_CHOICE = choice%combos_count; // sanitize
        log_d("Selecting IC #%d (%d) from combos", choice, CIV::IC_CHOICE );
        CIV::IC = &IC_COMBOS[CIV::IC_CHOICE];
        if( old_address > -1 ) setICAddress( old_address );
      }


      void setICAddress( uint8_t address )
      {
        CIV::CIV_ADR = address;
        CIV::IC->id = CIV::CIV_ADR;
        request[2] = CIV::CIV_ADR;
        log_d("Setting IC Address to 0x%02x", address );
      }


      uint8_t getDec( uint8_t b1, uint8_t b2, bool hex )
      {
        char str[5];
        if( hex ) {
          sprintf(str, "%02x%02x", b1, b2 );
        } else {
          sprintf(str, "%02d%02d", b1, b2 );
        }
        return atoi(str);
      }


      void buildRequest( char request[7], String *command, const char* separator )
      {
        *command = "";
        char hexStr[4];
        for (uint8_t i = 0; i < request_size; i++) {
          sprintf(hexStr, "%02x%s", request[i], separator );
          *command += String(hexStr);
        }
      }


      void buildRequest( const command_t *cmd, String *command, const char* separator )
      {
        memcpy( (char*)&request[4], cmd->bytes, 3 );
        buildRequest( request, command, separator );
      }


      void setRequest( command_t cmd )
      {
        memcpy( (char*)&request[4], cmd.bytes, 3 );
      }



      bool parse( String packet )
      {
        size_t packet_len = packet.length()/2;
        if( packet_len < 4 ) {
          log_e("invalid packet '%s' (%d chars)", packet.c_str(), packet.length() );
          return false; // invalid packet
        }

        uint8_t buffer[packet_len];

        for( int i=0;i<packet_len;i++ ) {
          char str[3] = { packet[i*2], packet[(i*2)+1], 0 };
          buffer[i] = strtol( str, NULL, 16 );
        }

        if( buffer[0] != request[0] || buffer[1] != request[1] || buffer[2] != request[3] || buffer[3] != request[2] ) {
          log_e("inconsistent packet '%s' (%d chars, in=0x%02x%02x, out=0x%02x%02x)", packet.c_str(), packet.length(), buffer[2], buffer[3], request[2], request[3] );
        }

        bool ret = parse( buffer, packet_len );

        if( ret && buffer[4]==CIV_MTR ) {
          uint8_t decvalue = getDec( buffer[6], buffer[7] );
          log_i("Needle: %d, packet=%s", decvalue, packet.c_str() );
        }

        return ret;
      }


      bool parse( uint8_t* buffer, size_t buffer_size )
      {
        if( buffer_size < 4 ) {
          return false;
        }

        switch( buffer[4] ) {
          case CIV_CHK:  /* proxy status online/offline */ break;
          case CIV_GTX:  status.tx       = buffer[4]; break;
          case CIV_DAT:  status.datamode = buffer[4]; DataMode::updateValues(); break;
          case CIV_MTR:
          {
            uint8_t decvalue = getDec( buffer[6], buffer[7] );
            switch( buffer[5] ) {
              case CIV_SMT: status.SMeter = decvalue; break;
              case CIV_PWR: status.PWR    = decvalue; break;
              case CIV_SWR: status.SWR    = decvalue; break;
              default:
                log_e("Unsupported CIV_%02x target for CIV_MTR(0x%02x)", buffer[5], decvalue );
                return false;
              break;
            }
          }
          case CIV_MOD:
            status.mode   = buffer[3];
            status.filter = buffer[4];
            DataMode::updateValues();
          break;
          break;
          case CIV_STA:
            switch( buffer[5] ) {
              case CIV_FRQ:
                if( buffer_size < 7 ) {
                  log_e("Invalid CIV_FRQ packet (expecting at least 8 bytes, got %d", buffer_size );
                  break;
                }
                status.freq = 0;
                for (uint8_t i = 2; i < 7; i++) {
                  status.freq += (buffer[9 - i] >> 4) * decMulti[(i - 2) * 2];
                  status.freq += (buffer[9 - i] & 0x0F) * decMulti[(i - 2) * 2 + 1];
                }
                if (Transverter::get() > 0) {
                  status.freq += double(Transverter::choices[Transverter::get()]);
                }
                log_i("Freq: %d", status.freq );
              break;
              default:
                log_e("Unsupported CIV_%02x target for CIV_STA()", buffer[5] );
                return false;
              break;
            }
          break;
          default:
            log_e("Unknown CIV_%02x command", buffer[4] );
            return false;
          break;
        }
        return true;
      }


      uint8_t getTX()
      {
        setRequest( CIV_GET_TX );
        if( ! dispatchCommand((char*)request, request_size, buffer, 5) ) {
          log_e("Unable to dispatch command");
          return 0;
        }
        status.tx = (buffer[4] <= 1) ? buffer[4] : 0; // expected values are 0 or 1
        return status.tx;
      }


      float getSmeterAngle()
      {
        if (status.SMeter <= 13) {
          return 42.50f;
        } else if (status.SMeter <= 120) {
          return mapFloat(status.SMeter, 14, 120, 42.50f, -6.50f); // SMeter image start at S1 so S0 is out of image on the left...
        } else {
          return mapFloat(status.SMeter, 121, 241, -6.50f, -43.0f);
        }
      }


      String getSmeterLabel()
      {
        float mapped_value = 0;

        if (status.SMeter <= 120) { // 120 = S9 = 9 * (40/3)
          mapped_value = status.SMeter / (40 / 3.0f);
        } else { // 240 = S9 + 60
          mapped_value = (status.SMeter - 120) / 2.0f;
        }

        int rounded_value = int(round(mapped_value));
        String mapped_value_str = String(rounded_value);

        if (status.SMeter <= 120) {
          return "S " + mapped_value_str;
        } else {
          if ( rounded_value < 10 )
            return  "S 9 + 0" + mapped_value_str + " dB";
          else
            return "S 9 + " + mapped_value_str + " dB";
        }
      }


      GaugeMeasure_t getSmeter()
      {
        GaugeMeasure_t ret = {0,""};
        setRequest( CIV_GET_SMETER );
        float mapped_value = 0;
        float angle = 0;

        if( ! dispatchCommand(request, request_size, buffer, 6) ) return ret;

        status.SMeter = getDec( buffer[4], buffer[5]);

        return { getSmeterAngle(), getSmeterLabel() };
      }


      float getSWRAngle()
      {
        size_t ranges = sizeof(SWR_Map)/sizeof(const GaugeMap_t);
        for( int i=0; i<ranges;i++ ) {
          const GaugeMap_t *_map = &SWR_Map[i];
          if( status.SWR >= _map->range_start && status.SWR <= _map->range_end ) {
            return mapFloat(status.SWR, _map->range_start, _map->range_end, _map->angle_start, _map->angle_end );
            break;
          }
        }
        return 0;
      }


      String getSWRLabel()
      {
        size_t ranges = sizeof(SWR_Map)/sizeof(const GaugeMap_t);
        for( int i=0; i<ranges;i++ ) {
          const GaugeMap_t *_map = &SWR_Map[i];
          if( status.SWR >= _map->range_start && status.SWR <= _map->range_end ) {
            float mapped_value = mapFloat(status.SWR, _map->range_start, _map->range_end, _map->value_start, _map->value_end );
            return "SWR " + String(mapped_value);
            break;
          }
        }
        return "";
      }


      GaugeMeasure_t getSWR()
      {
        GaugeMeasure_t ret = {0,""};
        setRequest( CIV_GET_SWR );
        float mapped_value = 0;
        float angle = 0;

        if( ! dispatchCommand(request, request_size, buffer, 6) ) return ret;

        status.SWR = getDec( buffer[4], buffer[5]);

        return { getSWRAngle(), getSWRLabel() };
      }


      float getPWRAngle()
      {
        size_t ranges = sizeof(PWR_Map)/sizeof(const GaugeMap_t);
        for( int i=0; i<ranges;i++ ) {
          const GaugeMap_t *_map = &PWR_Map[i];
          if( status.PWR >= _map->range_start && status.PWR <= _map->range_end ) {
            return mapFloat(status.PWR, _map->range_start, _map->range_end, _map->angle_start, _map->angle_end );
            break;
          }
        }
        return 0;
      }


      String getPWRLabel()
      {
        size_t ranges = sizeof(PWR_Map)/sizeof(const GaugeMap_t);
        for( int i=0; i<ranges;i++ ) {
          const GaugeMap_t *_map = &PWR_Map[i];
          if( status.PWR >= _map->range_start && status.PWR <= _map->range_end ) {
            float mapped_value = mapFloat(status.PWR, _map->range_start, _map->range_end, _map->value_start, _map->value_end );
            float translated_value = round(mapped_value * 10);
            if( CIV::IC->model == IC705 )
              return "PWR " + String((translated_value / 10)) + " W";
            else
              return "PWR " + String(translated_value) + " W";
            break;
          }
        }
        return "";
      }


      GaugeMeasure_t getPower()
      {
        GaugeMeasure_t ret = {0,""};
        setRequest( CIV_GET_PWR );
        float mapped_value = 0;
        float translated_value = 0;
        float angle = 0;

        if( ! dispatchCommand(request, request_size, buffer, 6) ) return ret;

        status.PWR = getDec( buffer[4], buffer[5]);

        return { getPWRAngle(), getPWRLabel() };
      }


      String getFrequencyLabel()
      {
        String ret;

        if( status.freq != 0 ) {
          char measure_value_str[17];
          format_number( status.freq, 16, measure_value_str, '.' );
          ret = String( measure_value_str );
        } else {
          ret = "-";
        }
        return ret;
      }


      String getFrequency()
      {
        setRequest( CIV_GET_FRQ );
        if( ! dispatchCommand((char*)request, request_size, buffer, 8) )  return "";

        status.freq = 0; // Current frequency in Hz
        for (uint8_t i = 2; i < 7; i++) {
          status.freq += (buffer[9 - i] >> 4) * decMulti[(i - 2) * 2];
          status.freq += (buffer[9 - i] & 0x0F) * decMulti[(i - 2) * 2 + 1];
        }

        if (Transverter::get() > 0) {
          status.freq += double(Transverter::choices[Transverter::get()]);
        }

        return getFrequencyLabel();
      }


      char getDataMode()
      {
        setRequest( CIV_GET_DATA_MODE );
        if( ! dispatchCommand(request, request_size, buffer, 6) ) {
          status.datamode = 0;
        } else {
          status.datamode = buffer[4];
        }
        return status.datamode;
      }


      String getModeLabel()
      {
        return "FIL" + String( uint8_t( status.filter ) ) + ( (status.datamode == 1) ? "-D" : "" );
      }


      String getFilterLabel()
      {
        return String(filterModes[ (uint8_t) status.mode ] );
      }


      FilterMode_t getMode()
      {
        setRequest( CIV_GET_MOD );
        FilterMode_t ret = {"", ""};
        if( ! dispatchCommand((char*)request, request_size, buffer, 5) ) return ret;

        status.filter = buffer[4];
        status.mode   = buffer[3];

        getDataMode(); // query CIV: is data ON or OFF ?

        return { getFilterLabel(), getModeLabel() };
      }


    };

  };

};
