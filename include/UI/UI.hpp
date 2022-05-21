#pragma once


namespace ICSMeter
{

  namespace UI
  {
    void setup();
    void draw();
    void drawTop();
    void drawMiddle();
    void drawBottom();
    void drawWidgets( bool force_redraw = false );
    bool canDrawUI();
  };

};
