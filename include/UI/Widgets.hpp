#pragma once

#include "Controls.hpp"
#include "Themes/Themes.hpp"


namespace ICSMeter
{

  namespace UI
  {

    namespace Battery
    {
      void   reset();
      bool   needs_redraw();
      int8_t getLevel(bool type);
      bool   isCharging();
      void   draw( bool force_redraw = false );
    };


    namespace Measure
    {
      void   reset();
      bool   needs_redraw();
      int8_t value = 1;
      void   drawValues( bool force_redraw = false );
      void   drawLabels( bool force_redraw = false ); // Print Measure
      void   setPrimaryValue( String value );
      void   setSecondaryValue( String value );
      void   drawPrimaryValue(String valString, uint8_t x = 160, uint8_t y = 180); // Print value
      void   drawSecondaryValue(String valString, uint8_t x = 160, uint8_t y = 206); // Print sub value
    };


    namespace Needle
    {
      void setup();
      void reset();
      bool needs_redraw();
      void onThemeChange();
      void set( float _angle );
      void draw( bool force_redraw = false );
      void draw(float_t angle, uint16_t a = 0, uint16_t b = 200, uint16_t c = 0, uint16_t d = 100); // Print needle
    };


    namespace Transverter
    {
      bool   needs_redraw();
      int8_t get();
      void   set( int8_t _value );
      void   save();
      void   draw( bool force_redraw = false );
      constexpr const double choices[] = { 0, TRANSVERTER_LO_1, TRANSVERTER_LO_2, TRANSVERTER_LO_3, TRANSVERTER_LO_4, TRANSVERTER_LO_5 };
    };


    namespace DataMode
    {
      char mode = 0;
      bool needs_redraw();
      void setFilter( String value );
      void setMode( String value );
      void draw( bool force_redraw = false );
    };


    namespace Settings
    {
      bool   dialog_enabled = false;
      int8_t choice = 0;
      void   draw();
      void   handle(); // called from task
      void   handleSettings();
      void   handleBrowsing();
      void   drawMenu();
      void   drawOptions(); // option choice decorator
    };


    namespace ScreenSaver
    {
      bool enabled = false;
      void shutdown();
      void reset();
      void handle();
    };


  };

};
