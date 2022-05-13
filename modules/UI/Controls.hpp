#pragma once


namespace ICSMeter
{

  namespace UI
  {

    uint8_t brightness = 64;

    int8_t beep = 0;
    int8_t beepOld = 0;

    uint8_t brightnessOld = 64;

    String frequencyOld    = "";
    String filterOld       = "";
    String modeOld         = "";
    String valStringOld    = "";
    String subValStringOld = "";

    void clearData();

    int8_t getBatteryLevel(bool type); // Get Battery level

    bool isCharging(); // Control if charging

    void setBrightness(uint8_t value); // Manage brightness

    void shutdown(); // Shutdown

    void buttonTask(void *pvParameters); // Get Button task

  };
};
