#pragma once

#include "../main/core.h"

namespace ICSMeter
{
  // Get button
  void getButton();

  namespace fs
  {
   // List files on SPIFFS or SD
    void getBinaryList(File dir, String type);
    // Bin Loader
    void binLoader();
  };
};

