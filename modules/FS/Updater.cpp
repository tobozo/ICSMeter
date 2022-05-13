#include "../core/ICSMeter.hpp"

#include <vector>

namespace ICSMeter
{

  namespace updater
  {

    struct loadable_bin_t
    {
      fs::FS *fs;
      String path;
    };

    constexpr const uint16_t MAX_ITEMS_IN_MENU = 128;
    std::vector<loadable_bin_t> binFileNames; // not optimal but better than wasting 128*sizeof(loadable_bin_t)

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


    constexpr const char* SPIFFS_MSG1           = "Flash File System";
    constexpr const char* SPIFFS_MSG2           = "needs to be formatted.";
    constexpr const char* SPIFFS_MSG3           = "It takes up to 4 minutes.";
    constexpr const char* SPIFFS_MSG4           = "Please, wait until ";
    constexpr const char* SPIFFS_MSG5           = "the application starts !";
    constexpr const char* SD_MSG_MOUNTFAIL      = "SD Mount Failed";
    constexpr const char* SPIFFS_MSG_MOUNTFAIL  = "SPIFFS Mount Failed";
    constexpr const char* SPIFFS_MSG_FORMATTING = "SPIFFS Formating...";
    constexpr const char* BIN_LOADER_VERSION    = "Bin Loader V0.2";
    constexpr const char* BIN_LOADER_SPI        = "SPI Flash File Storage";
    constexpr const char* BIN_LOADER_SD         = "SD Card Storage";


    #if defined GZIP_BINLOADER
    void updateProgressCallback( uint8_t progress )
    {
      if( SDUCfg.onProgress ) SDUCfg.onProgress( progress, 100 );
      else BaseUnpacker::defaultProgressCallback( progress );
    }
    #endif


    void onMessage( const String& message )
    {
      tft.clear();
      tft.setTextFont(1);
      tft.setTextSize(2);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextDatum(CC_DATUM);
      tft.drawString( message, 160, 20);
    }



    // Bin Loader
    void binLoader()
    {
      bool click = 0;
      int8_t cursor = 0;
      int8_t start = 0;
      int8_t stop = 0;
      int8_t limit = 8;
      int8_t change = 255;
      String tmpName;

      //SDUCfg.setProgressCb( SDMenuProgressUI ); // use SDUpdater built-in progress bar
      SetupSDMenuConfig(); // load SDUpdater UI defaults
      SDUCfg.setMessageCb( onMessage ); // use custom message
      if( SDUCfg.onBefore ) SDUCfg.onBefore(); // capture text styles

      SDUCfg.onMessage( "Scanning Filesystem" );

      if (SPIFFS.begin()) {
        getBinaryList( &SPIFFS );
      } else {
        Serial.println( SPIFFS_MSG_MOUNTFAIL );
        SDUCfg.onMessage( SPIFFS_MSG1 );
        //tft.drawString( SPIFFS_MSG1, 160, 20  );
        tft.drawString( SPIFFS_MSG2, 160, 50  );
        tft.drawString( SPIFFS_MSG3, 160, 100 );
        tft.drawString( SPIFFS_MSG4, 160, 150 );
        tft.drawString( SPIFFS_MSG5, 160, 180 );
        Serial.println( SPIFFS_MSG_FORMATTING );
        SPIFFS.format(); // Format SPIFFS...
      }

      if (SD.begin(GPIO_NUM_4, SPI, 25000000)) {
        getBinaryList( &SD );
      } else {
        Serial.println( SD_MSG_MOUNTFAIL );
      }

      if ( binFileNames.size() > 0) {
        SDUCfg.onMessage( "Press the 'Any' Key" );
        // QRCode
        tft.qrcode( REPO_URL, 90, 80, 140, 6 );

        for (uint8_t i = TIMEOUT_BIN_LOADER * 10; i > 0; i--) {

          checkButtons();

          if (i % 10 == 0) {
            tmpName += ".";
            tft.drawString(tmpName, 160, 40);
          }

          if (btnA || btnC) {
            SD.end(); // If not Bluetooth doesn't work !!!
            return;
          } else if (btnB) {
            click = 1;
            tft.fillRect(0, 0, 320, 240, TFT_BLACK);
            break;
          }

          vTaskDelay(100);
        }
      }

      SDUCfg.onMessage( BIN_LOADER_VERSION );

      while (click == 1) {
        while (btnB != 0) {

          checkButtons();
          vTaskDelay(100);
        }

        checkButtons();

        if (btnA) {
          cursor--;
        } else if (btnC) {
          cursor++;
        } else if (btnB) {

          SDUCfg.onMessage( "Loading " + binFileNames[cursor].path );

          if( binFileNames[cursor].path.endsWith(".gz") ) { // use gzUpdater
            #if defined GZIP_BINLOADER
              GzUnpacker *GZUnpacker = new GzUnpacker();
              GZUnpacker->haltOnError( false );                                         // stop on fail ( = manual restart/reset required )
              GZUnpacker->setupFSCallbacks( targzTotalBytesFn, targzFreeBytesFn );      // prevent the partition from exploding, recommended
              GZUnpacker->setGzProgressCallback( updateProgressCallback );              // attach SDUpdater progress bar to ESP32-targz unpacker
              GZUnpacker->setLoggerCallback( BaseUnpacker::targzPrintLoggerCallback  ); // gz log verbosity for serial console
              GZUnpacker->setPsram( true );                                             // may speed up decompression if psram is available
              if( ! GZUnpacker->gzUpdater( *binFileNames[cursor].fs, binFileNames[cursor].path.c_str(), U_FLASH, /*restart on update*/true ) ) {
                log_e("gzUpdater failed with return code #%d\n", GZUnpacker->tarGzGetError() );
                SDUCfg.onMessage( "Update Failed" );
              } else {
                Serial.println("Update success, will restart");
              }
            #endif
          } else { // use sdUpdater
            updateFromFS(*binFileNames[cursor].fs, binFileNames[cursor].path );
          }

          SDUCfg.onMessage( "Restarting.." );
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
          tft.setTextPadding(320);

          uint8_t i = 0;
          for (uint8_t j = (start * limit); j < stop; j++) {
            tmpName = binFileNames[j].path.substring(1);

            if (cursor == j) {
              tmpName = ">> " + tmpName + " <<";

              if (binFileNames[cursor].fs == &SPIFFS ) {
                tft.setTextSize(1);
                tft.drawString( BIN_LOADER_SPI, 160, 50);
              } else {
                tft.setTextSize(1);
                tft.drawString( BIN_LOADER_SD, 160, 50);
              }
            }

            tft.setTextSize(2);
            tft.drawString(tmpName, 160, 80 + i * 20);
            i++;
          }
        }
        vTaskDelay(100);
      }
      SD.end(); // If not Bluetooth doesn't work !!!

      if( SDUCfg.onAfter ) SDUCfg.onAfter(); // restore text styles

    }

  };
};


