#pragma once


namespace ICSMeter
{

  namespace UI
  {

    namespace ScreenSaver
    {
      uint32_t timer;
      uint32_t countdown;
      uint32_t countdownOld;
      bool mode = false;

      void shutdown();

      void handle(); // Manage screensaver
    };

  };

};
