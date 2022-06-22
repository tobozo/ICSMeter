#line 1 "/home/runner/work/ICSMeter/ICSMeter/include/UI/Widgets/Network.cpp"

namespace ICSMeter
{

  namespace UI
  {

    namespace Network
    {
      using namespace net;

      bool last_comm = false;
      bool last_tx   = true;
      bool msg_state = false; // message blink state
      const char* last_msg = nullptr;


      constexpr clipRect_t DaemonClip =
      {
        .x = 4,
        .y = 19,
        .w = 16,
        .h = 13
      };

      constexpr clipRect_t TXClip =
      {
        .x = 22,
        .y = 19,
        .w = 25,
        .h = 13
      };


      bool comm_needs_redraw()
      {
        if( !daemon::agent || last_comm == daemon::agent->available() ) return false;
        return true;
      }


      bool tx_needs_redraw()
      {
        if( last_tx == CIV::txConnected ) return false;
        return true;
      }


      bool msg_needs_redraw()
      {
        if( last_msg != daemon::message ) {
          return true;
        }
        static bool active_state = false;
        if( daemon::message != nullptr && active_state != msg_state ) {
          // blinking state changed
          active_state = msg_state;
          return true;
        }

        return false;
      }


      void drawComm()
      {
        bool available = daemon::agent ? daemon::agent->available() : false;
        CSS::drawStyledBox( &tft, "D", DaemonClip.x, DaemonClip.y, DaemonClip.w, DaemonClip.h, available ? &Theme::BadgeBoxStyle : &Theme::BadgeBoxStyleErr );
        last_comm = available;
      }


      void drawTX()
      {
        CSS::drawStyledBox( &tft, "TX", TXClip.x, TXClip.y, TXClip.w, TXClip.h, CIV::txConnected ? &Theme::BadgeBoxStyle : &Theme::BadgeBoxStyleWarn );
        last_tx = CIV::txConnected;
      }


      void blinkMessage()
      {
        setFontStyle( &tft, &Needle::labelFontStyle );
        tft.setTextColor( Theme::layout->fgcolor, Theme::layout->bgcolor );
        const char *message = daemon::message && msg_state ? daemon::message : "";
        CSS::drawStyledString( &tft, message, Needle::LabelClip.x, Needle::LabelClip.y, nullptr );
        last_msg = daemon::message;
      }


      bool needs_redraw()
      {
        msg_state = (millis()/773)%2==0;
        return msg_needs_redraw() || comm_needs_redraw() || tx_needs_redraw();
      }


      void draw( bool force_redraw )
      {
        if( needs_redraw() || force_redraw ) {
          drawComm();
          drawTX();
          blinkMessage();
        }
      }

    };

  };

};
