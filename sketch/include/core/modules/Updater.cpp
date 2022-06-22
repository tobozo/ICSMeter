#line 1 "/home/runner/work/ICSMeter/ICSMeter/include/core/modules/Updater.cpp"
#include "../ICSMeter.hpp"
#include "../../UI/UI.hpp"
#include <vector>

namespace ICSMeter
{

  namespace FSUpdater
  {

    using namespace UI;
    using namespace CSS;
    using namespace modules;
    using namespace modules::buttons;

    constexpr const char* SPIFFS_MSG1           = "Flash File System";
    constexpr const char* SPIFFS_MSG2           = "needs to be formatted.";
    constexpr const char* SPIFFS_MSG3           = "It takes up to 4 minutes.";
    constexpr const char* SPIFFS_MSG4           = "Please, wait until ";
    constexpr const char* SPIFFS_MSG5           = "the application starts !";
    constexpr const char* SPIFFS_MESSAGES[]     = {SPIFFS_MSG1, SPIFFS_MSG2, SPIFFS_MSG3, SPIFFS_MSG4, SPIFFS_MSG5 };
    constexpr const char* SD_MSG_MOUNTFAIL      = "SD Mount Failed";
    constexpr const char* SPIFFS_MSG_MOUNTFAIL  = "SPIFFS Mount Failed";
    constexpr const char* SPIFFS_MSG_FORMATTING = "SPIFFS Formating...";
    #if !defined GZIP_BINLOADER // no gzip support, skip gzipped files
      constexpr const char* BIN_LOADER_VERSION    = "Bin Loader V0.2";
    #else
      constexpr const char* BIN_LOADER_VERSION    = "Bin Loader V0.3";
    #endif
    constexpr const char* BIN_LOADER_SPI        = "SPI Flash File Storage";
    constexpr const char* BIN_LOADER_SD         = "SD Card Storage";

    constexpr const uint16_t MAX_ITEMS_IN_MENU = 128;

    struct loadable_bin_t
    {
      fs::FS *fs;
      String path;
    };


    std::vector<loadable_bin_t> binFileNames; // not optimal but better than blindly allocating 128*sizeof(loadable_bin_t)

    // CSS declarations

    const TextStyle_t BWCenteredFullPaddingH1 =
    {
      .fgColor   = 0xffffffU,
      .bgColor   = 0x000000U,
      .size      = 2,
      .datum     = MC_DATUM,
      .paddingX  = 320
    };
    const FontStyle_t StyleH1 = { &Font0, &BWCenteredFullPaddingH1, OPAQUE };


    const TextStyle_t BWCenteredFullPaddingH2 =
    {
      .fgColor   = 0xffffffU,
      .bgColor   = 0x000000U,
      .size      = 1,
      .datum     = MC_DATUM,
      .paddingX  = 320
    };
    const FontStyle_t StyleH2 = { &Font0, &BWCenteredFullPaddingH2, OPAQUE };



    void getBinaryList( fs::FS *sourceFS )
    {
      fs::File dir = sourceFS->open("/");
      if( !dir ) return;

      while (true) {
        fs::File entry = dir.openNextFile();
        if (!entry) break; // no more files

        const char *entry_name = SDUpdater::fs_file_path( &entry );

        if (strstr(entry_name, "/.") != NULL ) { // skip dotfiles / dotfolders
          entry.close();
          continue;
        }

        if ( strstr(entry_name, ".bin") != NULL ) { // found a .bin or .bin.gz file

          #if !defined GZIP_BINLOADER // no gzip support, skip gzipped files
            if( String(entry_name).endsWith(".gz") ) continue;
          #endif

          // Serial.println(type + "_" + entry_name);
          binFileNames.push_back( { sourceFS, String(entry_name) } );
          if( binFileNames.size()+1 >= MAX_ITEMS_IN_MENU ) {
            Serial.printf("Menu full after adding %d items, change MAX_ITEMS_IN_MENU value to extend that", binFileNames.size() );
            break; // avoid stack overflow
          }
        }
        entry.close();
      }
      dir.close();
    }


    #if defined GZIP_BINLOADER
    void updateProgressCallback( uint8_t progress )
    {
      if( SDUCfg.onProgress ) SDUCfg.onProgress( progress, 100 );
      else BaseUnpacker::defaultProgressCallback( progress );
    }
    #endif



    void binLoader()
    {
      bool click = 0;
      int8_t cursor = 0;
      int8_t start = 0;
      int8_t stop = 0;
      int8_t limit = 8;
      int8_t change = 255;
      String tmpName;

      SetupSDMenuConfig(); // load SDUpdater UI defaults (e.g. progressbar)

      CSS::drawStyledString( &tft, "Scanning Filesystem", 160, 20, &StyleH1 );

      if (FLASH_FS.begin()) {
        getBinaryList( &FLASH_FS );
      } else {

        size_t msgCount = sizeof( SPIFFS_MESSAGES ) / sizeof( const char* );
        int posy = 20;
        for( int i=0; i<msgCount; i++ ) {
          CSS::drawStyledString( &tft, SPIFFS_MESSAGES[i], 160, posy, i==0 ? &StyleH1 : nullptr );
          posy += (i==0 || i==msgCount-2 ) ? 30 : 50;
        }
        Serial.println( SPIFFS_MSG_MOUNTFAIL );
        Serial.println( SPIFFS_MSG_FORMATTING );
        FLASH_FS.format(); // Format SPIFFS...
        tft.clear();
      }

      if (SD.begin(GPIO_NUM_4, SPI, 25000000)) {
        getBinaryList( &SD );
      } else {
        Serial.println( SD_MSG_MOUNTFAIL );
      }

      if ( binFileNames.size() > 0) {
        CSS::drawStyledString( &tft, "", 160, 20, &StyleH1 ); // clear last message
        // QRCode
        tft.qrcode( REPO_URL, 90, 80, 140, 6 );

        for (uint8_t i = TIMEOUT_BIN_LOADER * 10; i > 0; i--) {

          buttons::loop();

          if (i % 10 == 0) {
            tmpName += ".";
            CSS::drawStyledString( &tft, tmpName.c_str(), 160, 40, nullptr );
          }

          if (btnA || btnC) {
            SD.end(); // If not Bluetooth doesn't work !!!
            return;
          } else if (btnB) {
            click = 1;
            tft.clear();
            break;
          }

          vTaskDelay(100);
        }
      }

      CSS::drawStyledString( &tft, BIN_LOADER_VERSION, 160, 20, &StyleH1 );

      while (click == 1) {
        while (btnB != 0) {
          buttons::loop();
          vTaskDelay(100);
        }

        buttons::loop();

        if (btnA) {
          cursor--;
        } else if (btnC) {
          cursor++;
        } else if (btnB) {

          CSS::drawStyledString( &tft, String("Loading " + binFileNames[cursor].path).c_str(), 160, 20, &StyleH1 );

          if( binFileNames[cursor].path.endsWith(".gz") ) { // use gzUpdater
            #if defined GZIP_BINLOADER
              GzUnpacker *GZUnpacker = new GzUnpacker();
              GZUnpacker->haltOnError( false );                                         // stop on fail ( = manual restart/reset required )
              GZUnpacker->setupFSCallbacks( targzTotalBytesFn, targzFreeBytesFn );      // prevent the partition from exploding, recommended
              GZUnpacker->setGzProgressCallback( updateProgressCallback );              // attach SDUpdater progress bar to ESP32-targz unpacker
              GZUnpacker->setLoggerCallback( BaseUnpacker::targzPrintLoggerCallback  ); // gz log verbosity for serial console
              GZUnpacker->setPsram( false );                                            // psram may slow down decompression
              if( ! GZUnpacker->gzUpdater( *binFileNames[cursor].fs, binFileNames[cursor].path.c_str(), U_FLASH, /*restart on update*/true ) ) {
                log_e("gzUpdater failed with return code #%d\n", GZUnpacker->tarGzGetError() );
                CSS::drawStyledString( &tft, "Update Failed", 160, 20, &StyleH1 );
              } else {
                Serial.println("Update success, will restart");
              }
            #endif
          } else { // use sdUpdater
            updateFromFS(*binFileNames[cursor].fs, binFileNames[cursor].path );
          }

          CSS::drawStyledString( &tft, "Restarting..", 160, 20, &StyleH1 );
          ESP.restart();
          // this code should be unreachable since the ESP has received a restart signal
          // however espressif doesn't feel the urge to halt the core when this happens
          while(1);
        }

        // pagination

        cursor = (cursor < 0) ? binFileNames.size() - 1 : cursor;
        cursor = (cursor > binFileNames.size() - 1) ? 0 : cursor;

        start = cursor / limit;
        stop  = (start * limit) + limit;

        if (change != cursor) {
          change = cursor;

          uint8_t i = 0;
          for (uint8_t j = (start * limit); j < stop; j++) {
            if( j>= binFileNames.size() ) continue; // when speculated pagination exceeds catalog size :-)
            tmpName = binFileNames[j].path.substring(1); // strip leading slash

            if (cursor == j) {
              tmpName = ">> " + tmpName + " <<";

              if (binFileNames[cursor].fs == &FLASH_FS ) {
                CSS::drawStyledString( &tft, BIN_LOADER_SPI, 160, 50, &StyleH2 );
              } else {
                CSS::drawStyledString( &tft, BIN_LOADER_SD, 160, 50, &StyleH2 );
              }
            }
            CSS::drawStyledString( &tft, tmpName.c_str(), 160, 80 + i * 20, &StyleH1 );
            i++;
          }
        }
        vTaskDelay(100);
      }
      SD.end(); // If not Bluetooth doesn't work !!!
    }

  };

};


