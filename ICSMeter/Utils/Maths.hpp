#pragma once

namespace ICSMeter
{
  namespace Utils
  {

    // Manage rotation
    static void rotate(uint16_t *x, uint16_t *y, float angle)
    {
      float xNew;
      float yNew;

      // Check that these are not null pointer
      assert(x);
      assert(y);

      angle = angle * PI / 180;

      xNew = *x * cos(angle) - *y * sin(angle);
      yNew = *x * sin(angle) + *y * cos(angle);

      *x = xNew;
      *y = yNew;
    }


    // Manage map float
    static float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
    {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }


  };

};
