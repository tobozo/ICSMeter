#pragma once


namespace ICSMeter
{

  namespace UI
  {


    namespace Battery
    {
      //void   reset();
      bool   needs_redraw();
      int8_t getLevel();
      bool   isCharging();
      void   draw( bool force_redraw = false );
    };


    namespace DataMode
    {
      //char mode = 0;
      bool needs_redraw();
      void updateValues();
      void setFilter( String value );
      void setMode( String value );
      void draw( bool force_redraw = false );
    };


    namespace Measure
    {
      enum MeasureModes_t
      {
        MODE_PWR, // PWR
        MODE_SMT, // SMeter
        MODE_SWR  // SWR
      };
      void   setup();
      //void   reset();
      bool   needs_redraw();
      MeasureModes_t mode = MODE_SMT;
      void   drawValues( bool force_redraw = false );
      void   drawLabels( bool force_redraw = false ); // Print Measure
      void   setUnitValue( String value );
      void   drawUnitValue(String valString, uint8_t x = 160, uint8_t y = 206); // Print sub value
    };


    namespace Needle
    {
      void setup();
      void end();
      //void reset();
      //bool force_reset = true;
      bool needs_redraw();
      void onThemeChange();
      void setAngle( float _angle );
      void setLabel( String label );
      void draw( bool force_redraw = false );
      void drawLabel( String label );
      void drawNeedle(float_t angle ); // Print needle
    };


    namespace Network
    {
      bool needs_redraw();
      void drawMessage();
      void drawTX();
      void drawComm();
      void draw( bool force_redraw = false );
    };


    namespace ScreenSaver
    {
      void setup();
      void loop();
      void enable();
      void disable();
      void sleep();
      void wakeup();
      void resetTimer();
      void setTimer( uint32_t minutes );
      void draw();
      bool isEnabled();  // !isDisabled()
      bool isDisabled(); // !isEnabled()
      bool isAwake();    // !isAsleep()
      bool isAsleep();   // !isAwake()
      bool shouldSleep();  // true when IC disconnects from proxy, or when sleep timer > getSleepDelay()
      bool shouldWakeup(); // true when IC connects to proxy or when a button is pushed
      uint32_t getDelay();      // milliseconds
      uint32_t getSleepDelay(); // milliseconds, = 2*getDelay()
    };


    namespace Settings
    {
      bool   dialog_enabled = false;
      int8_t choice = 0;
      void   draw( bool force_redraw = false );
      void   loop(); // called from task
      void   handleSettings();
      void   handleBrowsing();
      void   drawMenu();
      void   drawOptions(); // option choice decorator
    };


    namespace Transverter
    {
      void   setup();
      bool   needs_redraw();
      int8_t get();
      void   set( int8_t _value );
      void   save();
      void   draw( bool force_redraw = false );
      constexpr const int64_t choices[] = { 0, TRANSVERTER_LO_1, TRANSVERTER_LO_2, TRANSVERTER_LO_3, TRANSVERTER_LO_4, TRANSVERTER_LO_5 };
      constexpr const size_t choices_count = sizeof( Transverter::choices );
    };


  };

};
