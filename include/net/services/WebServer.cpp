#include "../../assets/index_html.hpp"
#include "../../assets/images.hpp"
#include "../../UI/Themes/Themes.hpp"
#include "../Daemon.hpp"

namespace ICSMeter
{

  namespace net
  {

    namespace WebServer
    {
      using namespace daemon;

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


      //void parseMessageTask( void* param )
      void handleMsgQueue()
      {
        if( msg_queue.size() > 0 ) {
          while( CIV::parsing ) vTaskDelay(100);
          CIV::parsing = true;
          String msg = msg_queue.front();
          if( CIV::parse( msg ) ) {
            proxy::setFlag( PROXY_ONLINE );
            proxy::setFlag( TX_ONLINE );
            CIV::last_poll = millis();
          }
          msg_queue.pop();
          CIV::parsing = false;
        }
      }



      void processWsMessage( String msg )
      {
        static String lastMsg = "";

        if( msg == "UART DOWN" ) {
          proxy::setFlag( TX_OFFLINE );
          log_e("UART DOWN");
        } else if( lastMsg != msg ) { // no spam
          msg_queue.push( msg );
        }

        lastMsg = msg;
      }



      void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
      {
        const size_t msg_max_len = 128; // we're dealing with short strings here
        proxy::last_check = millis();
        String WS_msg = "";

        if(type == WS_EVT_CONNECT){
          log_d("ws[%s][%u] connect", server->url(), client->id());
          client->printf("Hello Proxy Client %u :)", client->id());
          //client->ping();
          wsClient = client;
        } else if(type == WS_EVT_DISCONNECT){
          log_d("ws[%s][%u] disconnect", server->url(), client->id());
          wsClient = nullptr;
        } else if(type == WS_EVT_ERROR){
          log_d("ws[%s][%u] error(%u): %s", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
        } else if(type == WS_EVT_PONG){
          log_d("ws[%s][%u] pong[%u]: %s", server->url(), client->id(), len, (len)?(char*)data:"");
        } else if(type == WS_EVT_DATA) {
          AwsFrameInfo * info = (AwsFrameInfo*)arg;
          WS_msg = "";
          if(info->final && info->index == 0 && info->len == len) {
            // message is complete
            for(size_t i=0; i < info->len; i++) {
              WS_msg += (char) data[i];
              if( WS_msg.length()>=msg_max_len ) break;
            }

            if( WS_msg == "ping" ) {
              client->text("pong");
            } else {
              client->text("ok");
              Serial.printf("Deferring message (%d in queue, %d bytes free, msg=%s, clid=%d)\n", msg_queue.size(), ESP.getFreeHeap(), WS_msg.c_str(), client->id() );
              processWsMessage( WS_msg );
            }

          } else {
            // message or frame is split
            if(info->index == 0){
              if(info->num == 0)
                log_d("ws[%s][%u] %s-message start", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
              log_d("ws[%s][%u] frame[%u] start[%llu]", server->url(), client->id(), info->num, info->len);
            }

            log_d("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

            if(info->opcode == WS_TEXT){
              for(size_t i=0; i < len; i++) {
                WS_msg += (char) data[i];
                if( WS_msg.length()>=msg_max_len ) break;
              }
            } else {
              char buff[3];
              for(size_t i=0; i < len; i++) {
                sprintf(buff, "%02x ", (uint8_t) data[i]);
                WS_msg += buff;
                if( WS_msg.length()>=msg_max_len ) break;
              }
            }

            if((info->index + len) == info->len) {
              log_d("ws[%s][%u] frame[%u] end[%llu]", server->url(), client->id(), info->num, info->len);
              if(info->final) {
                client->text("ok");
                processWsMessage( WS_msg );
                log_d("ws[%s][%u] %s-message end", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
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


      void M5Screen24bmp( AsyncWebServerRequest *request )
      {
        using namespace screenshot;
        BMPStreamReader->setup();

        AsyncWebServerResponse *response = request->beginResponse("image/x-windows-bmp", BMPStreamReader->filesize, [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
          return BMPStreamReader->read(buffer, maxLen, index);
        });

        response->addHeader("Content-Disposition", "inline; filename=screenshot.bmp");
        request->send(response);
      }


      void setupOTA()
      {
        //Send OTA events to the browser
        ArduinoOTA.onStart([]() { events.send("Update Start", "ota"); });
        ArduinoOTA.onEnd([]() { events.send("Update End", "ota"); });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
          char p[32];
          sprintf(p, "Progress: %u%%\n", (progress/(total/100)));
          events.send(p, "ota");
        });
        ArduinoOTA.onError([](ota_error_t error) {
          if(     error == OTA_AUTH_ERROR)    events.send("Auth Failed",    "ota");
          else if(error == OTA_BEGIN_ERROR)   events.send("Begin Failed",   "ota");
          else if(error == OTA_CONNECT_ERROR) events.send("Connect Failed", "ota");
          else if(error == OTA_RECEIVE_ERROR) events.send("Receive Failed", "ota");
          else if(error == OTA_END_ERROR)     events.send("End Failed",     "ota");
          // else events.send("Unknown error",     "ota");
        });
        ArduinoOTA.setHostname( NAME );
        ArduinoOTA.begin();
      }




      void setupAsync()
      {
        using namespace screenshot;
        using namespace modules;

        WebServer::OTA_enabled = prefs::get("ota-enabled", 1 );

        uint32_t bytes_free_before = ESP.getFreeHeap();

        BMPStreamReader = new BMPStreamReader_t( tft.width(), tft.height(), 24 );

        // TODO:
        if( OTA_enabled ) {
          setupOTA();
        }

        MDNS.addService("http","tcp",80);

        ws.onEvent(onWsEvent);
        server.addHandler(&ws);

        events.onConnect([]( AsyncEventSourceClient *client ) {
          client->send( "hello!", NULL, millis(), 1000 );
        });

        server.addHandler( &events );

        server.onNotFound( []( AsyncWebServerRequest *request ) {
          request->send( 404, "text/html", "404 Page not found.<br>" );
        });

        server.on( "/", HTTP_GET, []( AsyncWebServerRequest *request ) {
          handleRootAsync( request );
        });

        server.on( "/screenshot.bmp", HTTP_GET, []( AsyncWebServerRequest *request ) {
          M5Screen24bmp( request );
        });

        server.on( "/favicon.ico", HTTP_GET, []( AsyncWebServerRequest *request ) {
          sendFaviconAsync( request );
        });


        server.on( "/buttonLeft", HTTP_GET, []( AsyncWebServerRequest *request ) {
          handleRootAsync( request );
          buttons::buttonLeftPressed = true;
          buttons::last_poll = millis() + 200;
          vTaskDelay(1);
        });

        server.on( "/buttonCenter", HTTP_GET, []( AsyncWebServerRequest *request ) {
          handleRootAsync( request );
          buttons::buttonCenterPressed = true;
          buttons::last_poll = millis() + 200;
          vTaskDelay(1);
        });

        server.on( "/buttonRight", HTTP_GET, []( AsyncWebServerRequest *request ) {
          handleRootAsync( request );
          buttons::buttonRightPressed = true;
          buttons::last_poll = millis() + 200;
          vTaskDelay(1);
        });

        server.begin();
        uint32_t bytes_free_after = ESP.getFreeHeap();
        log_i( "WebServer started (used %d bytes, now %d bytes free", uint32_t(int(bytes_free_after) - int(bytes_free_before)), bytes_free_after );
        WebServer::isRunning = true;
      }




    };

  };

};

