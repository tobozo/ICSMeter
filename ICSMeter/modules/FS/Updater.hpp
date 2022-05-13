#pragma once

#include "../core/ICSMeter.hpp"

namespace ICSMeter
{

  namespace fs
  {
   // List files on SPIFFS or SD
    void getBinaryList(File dir, String type);
    // Bin Loader
    void binLoader();
  };
};
