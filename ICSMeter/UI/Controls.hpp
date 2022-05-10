#pragma once

// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "../main/core.h"

namespace ICSMeter
{

  namespace UI
  {

    // Get Battery level
    int8_t getBatteryLevel(bool type);

    // Control if charging
    bool isCharging();

    // Manage brightness
    void setBrightness(uint8_t value);

    // Shutdown
    void shutdown();

    // Copyright (c) F4HWN Armel. All rights reserved.
    // Licensed under the MIT license. See LICENSE file in the project root for full license information.

    // Get Button
    void buttonTask(void *pvParameters);


  };
};
