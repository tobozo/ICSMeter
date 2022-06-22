#line 1 "/home/runner/work/ICSMeter/ICSMeter/include/net/services/Screenshots.cpp"
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


      struct BMPStreamReader_t
      {
        public:
          const uint16_t image_width;
          const uint16_t image_height;
          size_t yscan; // the line being scanned
          const uint8_t bit_depth;

          size_t filesize; // whole bmp size, as sent by Content-Lenght HTTP header

          BMPStreamReader_t( uint16_t w, uint16_t h, uint8_t d=24 ) : image_width(w), image_height(h), bit_depth(d) {  };

          ~BMPStreamReader_t()
          {
            if( line_buffer != nullptr) {
              free( line_buffer );
            }
            line_buffer = nullptr;
          }

          void setup()
          {
            assert( bit_depth==24 || bit_depth==16 );
            uint8_t bytewidth = bit_depth/8; // 2=16bits, 3=24bits
            row_size = ( bytewidth * image_width + 3) & ~ 3;

            if( line_buffer != nullptr ) {
              free( line_buffer );
            }
            filesize    = sizeof(lgfx::bitmap_header_t) + row_size * image_height;
            line_buffer = (uint8_t *)calloc( row_size, 1 );

            yscan = image_height;
            headers_sent = false;
            index = 0;
            memset( line_buffer, 0, row_size );
            ready = false;
            xTaskCreatePinnedToCore( muxTask, "muxTask", 2048, (void*)this, 1, NULL, 0);
          }


          static void muxTask( void* _reader )
          {
            assert( _reader );
            BMPStreamReader_t *reader = (BMPStreamReader_t*)_reader;
            takeLcdMux();
            reader->ready = true; // tell the BMPStreamReader SPI is ready for readings
            while( reader->yscan > 0 ) {
              log_w("yscan=%d", reader->yscan );
              vTaskDelay(100);
            }
            giveLcdMux();
            vTaskDelete( NULL );
          }



          size_t read( uint8_t *buffer, size_t maxLen, size_t idx )
          {
            assert( line_buffer );
            assert( row_size > 0 );
            assert( filesize > 0 );
            assert( idx == index );

            if( !headers_sent ) return setHeaders( buffer );

            while(! ready ) vTaskDelay(1); // wait display mux

            size_t buffindex = 0;
            while( buffindex < maxLen ) {
              buffer[buffindex] = readByte();
              buffindex++;
            }
            return buffindex;
          }


        private:

          const size_t header_size = sizeof( lgfx::bitmap_header_t );
          uint8_t *line_buffer = nullptr; // pointer to line buffer (char[row_size])
          size_t row_size;
          size_t index; // internal use: read index
          bool headers_sent;
          bool ready;

          size_t setHeaders( uint8_t* buffer )
          {
            assert( buffer );
            assert( bit_depth==24 || bit_depth==16 );

            lgfx::bitmap_header_t bmpheader;

            bmpheader.bfType        = 0x4D42;
            bmpheader.bfSize        = row_size * image_height + header_size;
            bmpheader.bfOffBits     = header_size;

            bmpheader.biSize        = 40;
            bmpheader.biWidth       = image_width;
            bmpheader.biHeight      = image_height;
            bmpheader.biPlanes      = 1;

            switch( bit_depth ) {
              case 24:
                bmpheader.biBitCount    = 24;
                bmpheader.biCompression = 0;
              break;
              case 16:
                bmpheader.biBitCount    = 16;
                bmpheader.biCompression = 3;
              break;
              default:
                log_e("Bad bit depth");
                return 0;
              break;
            }

            memcpy( buffer, &bmpheader, header_size );
            headers_sent = true;
            index = header_size;
            return header_size;
          }


          uint8_t readByte()
          {
            assert( line_buffer );
            assert( row_size > 0 );
            assert( filesize > 0 );
            assert( index >= header_size );
            size_t pos_in_line = (index-header_size)%row_size;
            if( pos_in_line==0 ) { // feed buffer
              readLine();
            }
            index++;
            return line_buffer[pos_in_line];
          }


          void readLine()
          {
            assert( line_buffer );
            assert( image_width > 0 );
            assert( image_height > 0 );
            uint8_t swap;

            if( yscan == 0 ) {
              log_e("No more lines to scan");
              return;
            }

            switch( bit_depth ) {
              case 24: tft.readRect(0, yscan - 1, image_width, 1, (lgfx::rgb888_t*)line_buffer); break;
              case 16: tft.readRect(0, yscan - 1, image_width, 1, (lgfx::rgb565_t*)line_buffer); break;
              default: return; break;
            }
            yscan--;
          }

      };



    };

  };

};

