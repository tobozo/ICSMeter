
namespace ICSMeter
{

  namespace UI
  {

    namespace DataMode
    {

      using namespace net;

      constexpr const char *datamode  = &CIV::status.datamode;
      constexpr const uint8_t *mode   = &CIV::status.mode;
      constexpr const uint8_t *filter = &CIV::status.filter;
      constexpr clipRect_t _modeCR   = { 240, 198, 40, 15 };
      constexpr clipRect_t _filterCR = { 40, 198, 40, 15 };

      // cache
      uint8_t old_datamode;
      uint8_t old_mode;
      uint8_t old_filter;

      String valueFilter, valueMode;


      bool filter_needs_redraw()
      {
        return *datamode != old_datamode || *filter != old_filter;
      }

      bool mode_needs_redraw()
      {
        return *mode != old_mode;
      }


      bool needs_redraw()
      {
        updateValues();
        if( filter_needs_redraw() || mode_needs_redraw() ) {
          return true;
        }
        return false;
      }


      void updateValues()
      {
        setFilter( CIV::getFilterLabel() );
        setMode(    CIV::getModeLabel()  );
      }


      void setFilter( String value )
      {
        valueFilter = value;
      }


      void setMode( String value )
      {
        valueMode = value;
      }


      void draw( bool force_redraw )
      {
        if( !needs_redraw() && !force_redraw ) return;
        if( filter_needs_redraw() || force_redraw ) {
          if( valueFilter!="" )
            CSS::drawStyledBox( &tft, valueFilter.c_str(), _modeCR.x, _modeCR.y, _modeCR.w, _modeCR.h, &Theme::BadgeBoxStyle );
          old_filter   = *filter;
        }
        if( mode_needs_redraw() || force_redraw ) {
          if( valueMode!="" )
            CSS::drawStyledBox( &tft, valueMode.c_str(), _filterCR.x, _filterCR.y, _filterCR.w, _filterCR.h, &Theme::BadgeBoxStyle );
          old_datamode = *datamode;
          old_mode     = *mode;
        }
      }


    };

  };

};

