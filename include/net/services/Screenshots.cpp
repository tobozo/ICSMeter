#include "../../assets/index_html.hpp"
#include "../../assets/images.hpp"
#include "../../UI/Themes/Themes.hpp"
#include "../Daemon.hpp"

namespace ICSMeter
{

  namespace net
  {

    namespace screenshot
    {

      using namespace modules::buttons;
      using namespace daemon;

      WebServer httpServer(80);

      void setup()
      {
        if (MDNS.begin( NAME )) {
          Serial.println("MDNS responder started");
        }

        httpServer.on("/", handleRoot);
        httpServer.onNotFound( handle404 );

        httpServer.on("/screenshot.bmp", HTTP_GET, []() {
          WiFiClient client = httpServer.client();
          M5Screen24bmp( &client );
        });


        httpServer.on("/favicon.ico", HTTP_GET, []() {
          sendFavicon();
        });


        httpServer.on("/buttonLeft", HTTP_GET, []() {
          buttonLeftPressed = true;
          handleRoot();
        });

        httpServer.on("/buttonCenter", HTTP_GET, []() {
          buttonCenterPressed = true;
          handleRoot();
        });

        httpServer.on("/buttonRight", HTTP_GET, []() {
          buttonRightPressed = true;
          handleRoot();
        });

        httpServer.begin();

      }

      void handleRoot()
      {
        httpServer.sendHeader("Cache-Control"," max-age=84600, public");
        httpServer.send(200, "text/html", index_html );
      }

      void handle404() {
        httpServer.send(404, "text/plain", "404 Page not found.<br>");
      }


      void sendFavicon()
      {
        httpServer.sendHeader("Cache-Control","public, max-age=84600, immutable"); // seems useless with Firefox
        httpServer.send_P( 200, "image/x-icon", (const char*)favicon_png, sizeof(favicon_png) );
      }


      void check()
      {
        if( !wifi::available() ) return;
        httpServer.handleClient();
      }



      void M5Screen24bmp( WiFiClient *client )
      {
        uint16_t image_height = tft.height();
        uint16_t image_width  = tft.width();
        const uint16_t pad = (4 - (3 * image_width) % 4) % 4;
        uint32_t filesize = 54 + (3 * image_width + pad) * image_height;

        takeLcdMux(); // wait for UI to finish drawing

        client->println("HTTP/1.1 200 OK");
        client->println("Content-type:image/bmp");
        client->println("Content-Length	"+String(filesize));
        client->println("Connection:close");
        client->println();

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
        client->write(header, 54);

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
          client->write(line_data, (image_width * 3) + pad);
        }

        client->stop();
        giveLcdMux(); // resume normal UI operations
      }


    };

  };

};

