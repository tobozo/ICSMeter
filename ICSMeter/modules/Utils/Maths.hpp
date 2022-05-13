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


    // format number with thousands separator
    void format_number( int64_t n, int maxlen, char *out, char thousand_separator = ',' )
    {
      int c;
      char buf[maxlen+1];
      char *p;

      snprintf(buf, maxlen, "%lld", n);
      c = 2 - strlen(buf) % 3;
      for (p = buf; *p != 0; p++) {
        *out++ = *p;
        if (c == 1) {
            *out++ = thousand_separator;
        }
        c = (c + 1) % 3;
      }
      *--out = 0;
    }

  };

};
