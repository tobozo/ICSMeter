#include "Controls.hpp"
#include "../Utils/Maths.hpp"
#include "../UI/UI.hpp"
#include "../UI/Widgets.hpp"

// Web site Screen Capture stuff
#define GET_unknown 0
#define GET_index_page  1
#define GET_screenshot  2


namespace ICSMeter
{

  namespace net
  {

    using namespace wifi;
    using namespace bluetooth;
    using namespace Utils;
    using namespace UI;


    // Flags for button presses via Web site Screen Capture
    bool buttonLeftPressed   = false;
    bool buttonCenterPressed = false;
    bool buttonRightPressed  = false;


    void setupNetwork()
    {
      if( strcmp( WIFI_SSID, "YOUR WIFI SSID" )==0 || strcmp( WIFI_SSID, "YOUR WIFI PASSWORD" )==0 ) {
        WiFi.begin(); // no login/password defined, maybe the ESP32 has saved credentials from a previous connection?
      } else {
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      }
      uint8_t attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts <= 10) {
        vTaskDelay(250);
        attempts += 1;
      }

      httpServer.begin(); // Start server (for Web site Screen Capture)

      // now start BT if needed
      if(IC_MODEL == 705 && IC_CONNECT == BT) {
        CAT.register_callback(bluetooth::callbackBT);

        if (!CAT.begin(NAME)) {
          Serial.println("An error occurred initializing Bluetooth");
        } else {
          Serial.println("Bluetooth initialized");
        }
      } else {
        if (WiFi.status() == WL_CONNECTED) wifi::connected = true;
      }
    }


    // Send CI-V Command dispatcher
    void sendCommand(char *request, size_t n, char *buffer, uint8_t limit)
    {
      if (IC_MODEL == 705 && IC_CONNECT == BT)
        sendCommandBt(request, n, buffer, limit);
      else
        sendCommandWifi(request, n, buffer, limit);
    }


    void getSmeter()
    {
      String valString;

      static char buffer[6];
      char request[] = {0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x15, 0x02, 0xFD};
      char str[2];

      uint8_t val0 = 0;
      float_t val1 = 0;
      static uint8_t val2 = 0;

      float_t angle = 0;

      size_t n = sizeof(request) / sizeof(request[0]);

      sendCommand(request, n, buffer, 6);

      sprintf(str, "%02x%02x", buffer[4], buffer[5]);
      val0 = atoi(str);

      if (val0 <= 120) { // 120 = S9 = 9 * (40/3)
        val1 = val0 / (40 / 3.0f);
      } else { // 240 = S9 + 60
        val1 = (val0 - 120) / 2.0f;
      }

      if (abs(val0 - val2) > 1 || reset == true) {
        val2 = val0;
        reset = false;

        if (val0 <= 13) {
          angle = 42.50f;
          valString = "S " + String(int(round(val1)));
        } else if (val0 <= 120) {
          angle = mapFloat(val0, 14, 120, 42.50f, -6.50f); // SMeter image start at S1 so S0 is out of image on the left...
          valString = "S " + String(int(round(val1)));
        } else {
          angle = mapFloat(val0, 121, 241, -6.50f, -43.0f);
          if (int(round(val1) < 10))
            valString = "S 9 + 0" + String(int(round(val1))) + " dB";
          else
            valString = "S 9 + " + String(int(round(val1))) + " dB";
        }

        // Debug trace
        if (DEBUG) {
          Serial.print("Get S");
          Serial.print(val0);
          Serial.print(" ");
          Serial.print(val1);
          Serial.print(" ");
          Serial.println(angle);
        }

        // Draw line
        Needle::draw(angle);

        // Write Value
        Measure::setValue(valString);

        // If led strip...
        /*
        uint8_t limit = map(val0, 0, 241, 0, NUM_LEDS_STRIP);

        for (uint8_t i = 0; i < limit; i++)
        {
          if (i < NUM_LEDS_STRIP / 2)
          {
            strip[i] = CRGB::Blue;
          }
          else
          {
            strip[i] = CRGB::Red;
          }
        }

        for (uint8_t i = limit; i < NUM_LEDS_STRIP; i++)
        {
          strip[i] = CRGB::White;
        }
        FastLED.show();
        */
      }
    }


    void getSWR()
    {
      String valString;

      static char buffer[6];
      char request[] = {0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x15, 0x12, 0xFD};
      char str[2];

      uint8_t val0 = 0;
      float_t val1 = 0;
      static uint8_t val3 = 0;

      float_t angle = 0;

      size_t n = sizeof(request) / sizeof(request[0]);

      sendCommand(request, n, buffer, 6);

      sprintf(str, "%02x%02x", buffer[4], buffer[5]);
      val0 = atoi(str);

      if (val0 != val3 || reset == true) {
        val3 = val0;
        reset = false;

        if (val0 <= 48) {
          angle = mapFloat(val0, 0, 48, 42.50f, 32.50f);
          val1 = mapFloat(val0, 0, 48, 1.0, 1.5);
        } else if (val0 <= 80) {
          angle = mapFloat(val0, 49, 80, 32.50f, 24.0f);
          val1 = mapFloat(val0, 49, 80, 1.5, 2.0);
        } else if (val0 <= 120) {
          angle = mapFloat(val0, 81, 120, 24.0f, 10.0f);
          val1 = mapFloat(val0, 81, 120, 2.0, 3.0);
        } else if (val0 <= 155) {
          angle = mapFloat(val0, 121, 155, 10.0f, 0.0f);
          val1 = mapFloat(val0, 121, 155, 3.0, 4.0);
        } else if (val0 <= 175) {
          angle = mapFloat(val0, 156, 175, 0.0f, -7.0f);
          val1 = mapFloat(val0, 156, 175, 4.0, 5.0);
        } else if (val0 <= 225) {
          angle = mapFloat(val0, 176, 225, -7.0f, -19.0f);
          val1 = mapFloat(val0, 176, 225, 5.0, 10.0);
        } else {
          angle = mapFloat(val0, 226, 255, -19.0f, -30.50f);
          val1 = mapFloat(val0, 226, 255, 10.0, 50.0);
        }

        valString = "SWR " + String(val1);

        // Debug trace
        if (DEBUG) {
          Serial.print("Get SWR");
          Serial.print(val0);
          Serial.print(" ");
          Serial.print(val1);
          Serial.print(" ");
          Serial.println(angle);
        }

        // Draw line
        Needle::draw(angle);

        // Write Value
        Measure::setValue(valString);
      }
    }


    void getPower()
    {
      String valString;

      static char buffer[6];
      char request[] = {0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x15, 0x11, 0xFD};
      char str[2];

      uint8_t val0 = 0;
      float_t val1 = 0;
      float_t val2 = 0;
      static uint8_t val3 = 0;

      float_t angle = 0;

      size_t n = sizeof(request) / sizeof(request[0]);

      sendCommand(request, n, buffer, 6);

      sprintf(str, "%02x%02x", buffer[4], buffer[5]);
      val0 = atoi(str);

      if (val0 != val3 || reset == true) {
        val3 = val0;
        reset = false;

        if (val0 <= 27) {
          angle = mapFloat(val0, 0, 27, 42.50f, 30.50f);
          val1 = mapFloat(val0, 0, 27, 0, 0.5);
        } else if (val0 <= 49) {
          angle = mapFloat(val0, 28, 49, 30.50f, 23.50f);
          val1 = mapFloat(val0, 28, 49, 0.5, 1.0);
        } else if (val0 <= 78) {
          angle = mapFloat(val0, 50, 78, 23.50f, 14.50f);
          val1 = mapFloat(val0, 50, 78, 1.0, 2.0);
        } else if (val0 <= 104) {
          angle = mapFloat(val0, 79, 104, 14.50f, 6.30f);
          val1 = mapFloat(val0, 79, 104, 2.0, 3.0);
        } else if (val0 <= 143) {
          angle = mapFloat(val0, 105, 143, 6.30f, -6.50f);
          val1 = mapFloat(val0, 105, 143, 3.0, 5.0);
        } else if (val0 <= 175) {
          angle = mapFloat(val0, 144, 175, -6.50f, -17.50f);
          val1 = mapFloat(val0, 144, 175, 5.0, 7.0);
        } else {
          angle = mapFloat(val0, 176, 226, -17.50f, -30.50f);
          val1 = mapFloat(val0, 176, 226, 7.0, 10.0);
        }

        val2 = round(val1 * 10);
        if (IC_MODEL == 705)
          valString = "PWR " + String((val2 / 10)) + " W";
        else
          valString = "PWR " + String(val2) + " W";

        // Debug trace
        if (DEBUG) {
          Serial.print("Get PWR");
          Serial.print(val0);
          Serial.print(" ");
          Serial.print(val1);
          Serial.print(" ");
          Serial.println(angle);
        }

        // Draw line
        Needle::draw(angle);

        // Write Value
        Measure::setValue(valString);
      }
    }


    void getDataMode()
    {
      static char buffer[6];
      char request[] = {0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x1A, 0x06, 0xFD};

      size_t n = sizeof(request) / sizeof(request[0]);

      sendCommand(request, n, buffer, 6);

      dataMode = buffer[4];
    }


    void getFrequency()
    {
      String frequency;
      String frequencyNew;

      static char buffer[8];
      char request[] = {0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x03, 0xFD};

      double freq; // Current frequency in Hz
      const uint32_t decMulti[] = {1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1};

      uint8_t lenght = 0;

      size_t n = sizeof(request) / sizeof(request[0]);

      sendCommand(request, n, buffer, 8);

      freq = 0;
      for (uint8_t i = 2; i < 7; i++) {
        freq += (buffer[9 - i] >> 4) * decMulti[(i - 2) * 2];
        freq += (buffer[9 - i] & 0x0F) * decMulti[(i - 2) * 2 + 1];
      }

      if (Transverter::value > 0)
        freq += double(Transverter::choices[Transverter::value]);

      frequency = String(freq);
      lenght = frequency.length();

      if (frequency != "0") {
        int8_t i;

        for (i = lenght - 6; i >= 0; i -= 3) {
          frequencyNew = "." + frequency.substring(i, i + 3) + frequencyNew;
        }

        if (i == -3) {
          frequencyNew = frequencyNew.substring(1, frequencyNew.length());
        } else {
          frequencyNew = frequency.substring(0, i + 3) + frequencyNew;
        }
        Measure::subValue(frequencyNew);
      } else {
        Measure::subValue("-");
      }
    }


    void getMode()
    {
      String valString;

      static char buffer[5];
      char request[] = {0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x04, 0xFD};

      const char *mode[] = {"LSB", "USB", "AM", "CW", "RTTY", "FM", "WFM", "CW-R", "RTTY-R", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "DV"};

      size_t n = sizeof(request) / sizeof(request[0]);

      sendCommand(request, n, buffer, 5);

      tft.setFont(0);
      tft.setTextPadding(24);
      tft.setTextColor(TFT_WHITE);
      tft.setTextDatum(CC_DATUM);

      valString = "FIL" + String(uint8_t(buffer[4]));
      if (valString != filterOld) {
        filterOld = valString;
        tft.fillRoundRect(40, 198, 40, 15, 2, Theme::TFT_MODE_BACK);
        tft.drawRoundRect(40, 198, 40, 15, 2, Theme::TFT_MODE_BORDER);
        tft.drawString(valString, 60, 206);
      }

      valString = String(mode[(uint8_t)buffer[3]]);

      getDataMode(); // Data ON or OFF ?

      if (dataMode == 1) {
        valString += "-D";
      }

      if (valString != modeOld) {
        modeOld = valString;
        tft.fillRoundRect(240, 198, 40, 15, 2, Theme::TFT_MODE_BACK);
        tft.drawRoundRect(240, 198, 40, 15, 2, Theme::TFT_MODE_BORDER);
        tft.drawString(valString, 260, 206);
      }
    }


    uint8_t getTX()
    {
      uint8_t value;

      static char buffer[5];
      char request[] = {0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x1C, 0x00, 0xFD};

      size_t n = sizeof(request) / sizeof(request[0]);

      sendCommand(request, n, buffer, 5);

      if (buffer[4] <= 1) {
        value = buffer[4];
      } else {
        value = 0;
      }

      return value;
    }


    // Manage connexion error
    bool checkConnection()
    {
      HTTPClient http;

      uint16_t httpCode;

      String message = "";
      String command = "";
      String response = "";

      char request[] = {0xFE, 0xFE, CI_V_ADDRESS, 0xE0, 0x03, 0xFD};
      char s[4];

      Settings::lock = false;

      for (uint8_t i = 0; i < 6; i++) {
        sprintf(s, "%02x,", request[i]);
        command += String(s);
      }

      command += BAUDE_RATE + String(",") + SERIAL_DEVICE;

      if (screenshot == false) {
        if (IC_MODEL == 705 && IC_CONNECT == BT && bluetooth::connected == false)
          message = "Need Pairing";
        else if (IC_CONNECT == USB && wifi::connected == false)
          message = "Check Wifi";
        else if (IC_CONNECT == USB && (proxyConnected == false || txConnected == false))
        {
          http.begin(civClient, PROXY_URL + String(":") + PROXY_PORT + String("/") + String("?civ=") + command); // Specify the URL
          http.addHeader("User-Agent", "M5Stack");                                                               // Specify header
          http.addHeader("Connection", "keep-alive");                                                            // Specify header
          http.setTimeout(100);                                                                                  // Set Time Out
          httpCode = http.GET();                                                                                 // Make the request
          if (httpCode == 200) {
            proxyConnected = true;

            response = http.getString(); // Get data
            response.trim();

            if (response != "") {
              Serial.println("TX connected");
              txConnected = true;
              message = "";
            } else {
              Serial.println("TX disconnected");
              txConnected = false;
              message = "Check TX";
            }
          } else {
            message = "Check Proxy";
          }
          http.end();
        }

        // Shutdown screen if no TX connexion
        if (wakeup == true && startup == false) {
          if ((IC_CONNECT == BT && bluetooth::connected == false) || (IC_CONNECT == USB && txConnected == false)) {
            tft.sleep();
            wakeup = false;
          }
        } else if (wakeup == false && startup == false) {
          if ((IC_CONNECT == BT && bluetooth::connected == true) || (IC_CONNECT == USB && txConnected == true)) {
            clearData();
            UI::draw();
            tft.wakeup();
            wakeup = true;
            ScreenSaver::timer = millis();
          }
        }

        // View message
        if (message != "") {
          Settings::lock = true;

          if (ScreenSaver::mode == false && Settings::mode == false) {
            tft.setTextDatum(CC_DATUM);
            tft.setFont(&stencilie16pt7b);
            tft.setTextPadding(194);
            tft.setTextColor(Theme::TFT_FRONT, Theme::TFT_BACK);
            tft.drawString(message, 160, 180);
            vTaskDelay(750);
            tft.drawString("", 160, 180);
            frequencyOld = "";
            Settings::lock = false;
            vTaskDelay(250);
            return false;
          } else {
            Settings::lock = false;
            vTaskDelay(1000);
            return false;
          }
        }
      }
      return true;
    }


    bool M5Screen24bmp()
    {
      uint16_t image_height = tft.height();
      uint16_t image_width = tft.width();
      const uint16_t pad = (4 - (3 * image_width) % 4) % 4;
      uint32_t filesize = 54 + (3 * image_width + pad) * image_height;
      unsigned char swap;
      unsigned char line_data[image_width * 3 + pad];
      unsigned char header[54] = {
          'B', 'M',    // BMP signature (Windows 3.1x, 95, NT, …)
          0, 0, 0, 0,  // Image file size in bytes
          0, 0, 0, 0,  // Reserved
          54, 0, 0, 0, // Start of pixel array
          40, 0, 0, 0, // Info header size
          0, 0, 0, 0,  // Image width
          0, 0, 0, 0,  // Image height
          1, 0,        // Number of color planes
          24, 0,       // Bits per pixel
          0, 0, 0, 0,  // Compression
          0, 0, 0, 0,  // Image size (can be 0 for uncompressed images)
          0, 0, 0, 0,  // Horizontal resolution (dpm)
          0, 0, 0, 0,  // Vertical resolution (dpm)
          0, 0, 0, 0,  // Colors in color table (0 = none)
          0, 0, 0, 0}; // Important color count (0 = all colors are important)

      // Fill filesize, width and heigth in the header array
      for (uint8_t i = 0; i < 4; i++) {
        header[2 + i] = (char)((filesize >> (8 * i)) & 255);
        header[18 + i] = (char)((image_width >> (8 * i)) & 255);
        header[22 + i] = (char)((image_height >> (8 * i)) & 255);
      }

      // Write the header to the file
      httpClient.write(header, 54);

      // To keep the required memory low, the image is captured line by line, initialize padded pixel with 0
      for (uint16_t i = (image_width - 1) * 3; i < (image_width * 3 + pad); i++) {
        line_data[i] = 0;
      }
      // The coordinate origin of a BMP image is at the bottom left. therefore, the image must be read from bottom to top.
      for (uint16_t y = image_height; y > 0; y--) {
        // Get one line of the screen content
        tft.readRectRGB(0, y - 1, image_width, 1, line_data);
        // BMP color order is: Blue, Green, Red
        // Return values from readRectRGB is: Red, Green, Blue
        // Therefore: R und B need to be swapped
        for (uint16_t x = 0; x < image_width; x++) {
          swap = line_data[x * 3];
          line_data[x * 3] = line_data[x * 3 + 2];
          line_data[x * 3 + 2] = swap;
        }
        // Write the line to the file
        httpClient.write(line_data, (image_width * 3) + pad);
      }
      return true;
    }


    // Get screenshot
    void checkScreenshot()
    {
      unsigned long timeout_millis = millis() + 3000;
      String currentLine = "";

      if (WiFi.status() == WL_CONNECTED) {
        httpClient = httpServer.available();
        // httpClient.setNoDelay(1);
        if (httpClient) {
          // Force a disconnect after 3 seconds
          // Serial.println("New Client.");
          // Loop while the client's connected
          while (httpClient.connected()) {
            // If the client is still connected after 3 seconds, something is wrong. So kill the connection
            if (millis() > timeout_millis) {
              // Serial.println("Force Client stop!");
              httpClient.stop();
            }
            // If there's bytes to read from the client,
            if (httpClient.available()) {
              screenshot = true;
              char c = httpClient.read();
              Serial.write(c);
              // If the byte is a newline character
              if (c == '\n') {
                // Two newline characters in a row (empty line) are indicating the end of the client HTTP request, so send a response:
                if (currentLine.length() == 0) {
                  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK) and a content-type so the client knows what's coming, then a blank line, followed by the content:
                  ScreenSaver::timer = millis(); // Screensaver update !!!

                  switch (htmlGetRequest) {
                    case GET_index_page:
                      {
                        httpClient.println("HTTP/1.1 200 OK");
                        httpClient.println("Content-type:text/html");
                        httpClient.println();
                        if (M5.getBoard() == m5::board_t::board_M5Stack) {
                          httpClient.write_P(index_m5stack_html, sizeof(index_m5stack_html));
                        } else if (M5.getBoard() == m5::board_t::board_M5StackCore2) {
                          httpClient.write_P(index_core2_html, sizeof(index_core2_html));
                        }
                        break;
                      }
                    case GET_screenshot:
                      {
                        httpClient.println("HTTP/1.1 200 OK");
                        httpClient.println("Content-type:image/bmp");
                        httpClient.println();
                        M5Screen24bmp();
                        vTaskDelay(1000);
                        screenshot = false;
                        break;
                      }
                    default:
                      httpClient.println("HTTP/1.1 404 Not Found");
                      httpClient.println("Content-type:text/html");
                      httpClient.println();
                      httpClient.print("404 Page not found.<br>");
                      break;
                  }
                  // The HTTP response ends with another blank line:
                  // httpClient.println();
                  // Break out of the while loop:
                  break;

                } else { // if a newline is found
                  // Analyze the currentLine:
                  if (currentLine.startsWith("GET /")) { // Detect the specific GET requests:
                    htmlGetRequest = GET_unknown;

                    if (currentLine.startsWith("GET / ")) { // If no specific target is requested
                      htmlGetRequest = GET_index_page;
                    }

                    if (currentLine.startsWith("GET /screenshot.bmp")) { // If the screenshot image is requested
                      htmlGetRequest = GET_screenshot;
                    }

                    if (currentLine.startsWith("GET /buttonLeft")) { // If the button left was pressed on the HTML page
                      buttonLeftPressed = true;
                      htmlGetRequest = GET_index_page;
                    }

                    if (currentLine.startsWith("GET /buttonCenter")) { // If the button center was pressed on the HTML page
                      buttonCenterPressed = true;
                      htmlGetRequest = GET_index_page;
                    }

                    if (currentLine.startsWith("GET /buttonRight")) { // If the button right was pressed on the HTML page
                      buttonRightPressed = true;
                      htmlGetRequest = GET_index_page;
                    }
                  }
                  currentLine = "";
                }
              } else if (c != '\r') { // Add anything else than a carriage return character to the currentLine
                currentLine += c;
              }
            }
          }

          httpClient.stop(); // Close the connection
          // Serial.println("Client Disconnected.");
          vTaskDelay(100);
        }
      }
    }


  };

};

