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

      //WebServer httpServer(80);


      AsyncWebServer server(80);
      AsyncWebSocket ws("/ws");
      AsyncEventSource events("/events");

      String getContentType(String filename)
      {
        /*if (server.hasArg("download")) {
          return "application/octet-stream";
        } else*/ if (filename.endsWith(".htm")) {
          return "text/html";
        } else if (filename.endsWith(".html")) {
          return "text/html";
        } else if (filename.endsWith(".css")) {
          return "text/css";
        } else if (filename.endsWith(".js")) {
          return "application/javascript";
        } else if (filename.endsWith(".png")) {
          return "image/png";
        } else if (filename.endsWith(".gif")) {
          return "image/gif";
        } else if (filename.endsWith(".jpg")) {
          return "image/jpeg";
        } else if (filename.endsWith(".ico")) {
          return "image/x-icon";
        } else if (filename.endsWith(".xml")) {
          return "text/xml";
        } else if (filename.endsWith(".pdf")) {
          return "application/x-pdf";
        } else if (filename.endsWith(".zip")) {
          return "application/x-zip";
        } else if (filename.endsWith(".gz")) {
          return "application/x-gzip";
        }
        return "text/plain";
      }


      void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
      {
        if(type == WS_EVT_CONNECT){
          log_w("ws[%s][%u] connect", server->url(), client->id());
          client->printf("Hello Client %u :)", client->id());
          client->ping();
        } else if(type == WS_EVT_DISCONNECT){
          log_w("ws[%s][%u] disconnect", server->url(), client->id());
        } else if(type == WS_EVT_ERROR){
          log_w("ws[%s][%u] error(%u): %s", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
        } else if(type == WS_EVT_PONG){
          log_w("ws[%s][%u] pong[%u]: %s", server->url(), client->id(), len, (len)?(char*)data:"");
        } else if(type == WS_EVT_DATA){
          AwsFrameInfo * info = (AwsFrameInfo*)arg;
          String msg = "";
          if(info->final && info->index == 0 && info->len == len){
            //the whole message is in a single frame and we got all of it's data
            log_w("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
          } else {
            //message is comprised of multiple frames or the frame is split into multiple packets
            if(info->index == 0){
              if(info->num == 0)
                log_w("ws[%s][%u] %s-message start", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
              log_w("ws[%s][%u] frame[%u] start[%llu]", server->url(), client->id(), info->num, info->len);
            }

            log_w("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

            if(info->opcode == WS_TEXT){
              for(size_t i=0; i < len; i++) {
                msg += (char) data[i];
              }
            } else {
              char buff[3];
              for(size_t i=0; i < len; i++) {
                sprintf(buff, "%02x ", (uint8_t) data[i]);
                msg += buff ;
              }
            }
            //log_w("%s",msg.c_str());

            if((info->index + len) == info->len){
              log_w("ws[%s][%u] frame[%u] end[%llu]", server->url(), client->id(), info->num, info->len);
              if(info->final){
                log_w("ws[%s][%u] %s-message end", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                if(info->message_opcode == WS_TEXT)
                  client->text("I got your multiple text message");
                else
                  client->binary("I got your multiple binary message");
              }
            }
          }
        }
      }


      void handleRootAsync( AsyncWebServerRequest *request )
      {
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", index_html);
        response->addHeader("Cache-Control"," max-age=84600, public");
        request->send( response );
      }

      void sendFaviconAsync( AsyncWebServerRequest *request )
      {
        AsyncWebServerResponse *response = request->beginResponse_P( 200, "image/x-icon", (const uint8_t*)favicon_png, sizeof(favicon_png) );
        response->addHeader("Cache-Control","public, max-age=84600, immutable"); // seems useless with Firefox
        request->send( response );
      }


      struct BMPStreamReader_t
      {
        public:
          const uint16_t image_width;  // img width
          const uint16_t image_height; // img height

          size_t filesize; // whole bmp size, as sent by Content-Lenght HTTP header

          BMPStreamReader_t( uint16_t w, uint16_t h ) : image_width(w), image_height(h) {  };

          ~BMPStreamReader_t()
          {
            if( line_buffer != nullptr)
              free( line_buffer );
          }

          void setup()
          {
            if( line_buffer == nullptr ) {
              pad              = (4 - (3 * image_width) % 4) % 4;
              filesize         = 54 + (3 * image_width + pad) * image_height;
              header_size      = sizeof(headers);
              line_buffer_size = image_width * 3 + pad;
              line_buffer = (uint8_t *)calloc( line_buffer_size, 1 );
              // Fill filesize, width and heigth in the header array
              for (uint8_t i = 0; i < 4; i++) {
                headers[2 + i] = (char)((filesize >> (8 * i)) & 255);
                headers[18 + i] = (char)((image_width >> (8 * i)) & 255);
                headers[22 + i] = (char)((image_height >> (8 * i)) & 255);
              }
            }
            yscan = image_height;
            headers_sent = false;
            index = 0;
            memset( line_buffer, 0, line_buffer_size );
          }


          size_t read( uint8_t *buffer, size_t maxLen, size_t idx )
          {
            assert( headers );
            assert( line_buffer );
            assert( line_buffer_size > 0 );
            assert( filesize > 0 );
            //assert( idx == index );
            if( idx != index ) {
              log_n("local and remote index mismatch idx=%d index=%d", idx, index );
            }

            if( !headers_sent ) return send_headers( buffer );

            size_t buffindex = 0;
            while( buffindex < maxLen ) {
              buffer[buffindex] = read();
              buffindex++;
            }
            return buffindex;
          }


        private:

          size_t line_buffer_size;     // = (image_width * 3) + pad
          uint8_t *line_buffer = nullptr;        // pointer to line buffer (char[line_buffer_size])

          size_t index; // internal use: read index
          uint16_t pad;
          bool headers_sent;
          size_t header_size;
          size_t yscan;

          uint8_t headers[54] =
          {
            'B', 'M',    // BMP signature (Windows 3.1x, 95, NT, …)
            0,   0,   0,   0,  // Image file size in bytes
            0,   0,   0,   0,  // Reserved
            54,  0,   0,   0, // Start of pixel array
            40,  0,   0,   0, // Info header size
            0,   0,   0,   0,  // Image width
            0,   0,   0,   0,  // Image height
            1,   0,            // Number of color planes
            24,  0,           // Bits per pixel
            0,   0,   0,   0,  // Compression
            0,   0,   0,   0,  // Image size (can be 0 for uncompressed images)
            0,   0,   0,   0,  // Horizontal resolution (dpm)
            0,   0,   0,   0,  // Vertical resolution (dpm)
            0,   0,   0,   0,  // Colors in color table (0 = none)
            0,   0,   0,   0   // Important color count (0 = all colors are important);
          };

          size_t send_headers( uint8_t* buffer )
          {
            memcpy( buffer, headers, header_size );
            headers_sent = true;
            index = header_size;
            return header_size;
          }

          uint8_t read()
          {
            assert( line_buffer );
            assert( line_buffer_size > 0 );
            assert( filesize > 0 );
            assert( index >= header_size );
            size_t pos_in_line = (index-header_size)%line_buffer_size;
            if( pos_in_line==0 ) { // feed buffer
              readLine();
            }
            index++;
            return line_buffer[pos_in_line];
          }

          void readLine()
          {
            assert( line_buffer );
            assert( line_buffer_size > 0 );
            assert( image_width > 0 );
            assert( image_height > 0 );
            uint8_t swap;
            // Get one line of the screen content
            if( yscan == 0 ) {
              log_e("No more lines to scan");
              return;
            }
            takeLcdMux();
            tft.readRectRGB(0, yscan - 1, image_width, 1, line_buffer);
            giveLcdMux();
            yscan--;
            // BMP color order is BGR, so swap R and B from readRectRGB buffer
            for (uint16_t x = 0; x < image_width; x++) {
              swap = line_buffer[x * 3];
              line_buffer[x * 3] = line_buffer[x * 3 + 2];
              line_buffer[x * 3 + 2] = swap;
            }
          }


      };


      BMPStreamReader_t *BMPStreamReader;


      void M5Screen24bmp( AsyncWebServerRequest *request )
      {
        BMPStreamReader->setup();

        AsyncWebServerResponse *response = request->beginResponse("image/x-windows-bmp", BMPStreamReader->filesize, [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t{
          return BMPStreamReader->read(buffer, maxLen, index);
        });

        response->addHeader("Content-Disposition", "inline; filename=screenshot.bmp");
        request->send(response);
      }


      void setupAsync()
      {

        BMPStreamReader = new BMPStreamReader_t( tft.width(), tft.height() );

        //Send OTA events to the browser
        ArduinoOTA.onStart([]() { events.send("Update Start", "ota"); });
        ArduinoOTA.onEnd([]() { events.send("Update End", "ota"); });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
          char p[32];
          sprintf(p, "Progress: %u%%\n", (progress/(total/100)));
          events.send(p, "ota");
        });
        ArduinoOTA.onError([](ota_error_t error) {
          if(error == OTA_AUTH_ERROR) events.send("Auth Failed", "ota");
          else if(error == OTA_BEGIN_ERROR) events.send("Begin Failed", "ota");
          else if(error == OTA_CONNECT_ERROR) events.send("Connect Failed", "ota");
          else if(error == OTA_RECEIVE_ERROR) events.send("Recieve Failed", "ota");
          else if(error == OTA_END_ERROR) events.send("End Failed", "ota");
        });
        ArduinoOTA.setHostname( NAME );
        ArduinoOTA.begin();

        MDNS.addService("http","tcp",80);

        ws.onEvent(onWsEvent);
        server.addHandler(&ws);

        events.onConnect([](AsyncEventSourceClient *client){
          client->send("hello!",NULL,millis(),1000);
        });

        server.addHandler(&events);

        server.onNotFound([](AsyncWebServerRequest *request) {
          request->send(404, "text/html", "404 Page not found.<br>");
        });

        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
          handleRootAsync( request );
        });

        server.on("/screenshot.bmp", HTTP_GET, [](AsyncWebServerRequest *request) {
          M5Screen24bmp( request );
        });

        server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
          sendFaviconAsync( request );
        });


        server.on("/buttonLeft", HTTP_GET, [](AsyncWebServerRequest *request) {
          buttonLeftPressed = true;
          handleRootAsync( request );
        });

        server.on("/buttonCenter", HTTP_GET, [](AsyncWebServerRequest *request) {
          buttonCenterPressed = true;
          handleRootAsync( request );
        });

        server.on("/buttonRight", HTTP_GET, [](AsyncWebServerRequest *request) {
          buttonRightPressed = true;
          handleRootAsync( request );
        });

        server.begin();
        log_d("WebServer started");
      }



    };

  };

};

