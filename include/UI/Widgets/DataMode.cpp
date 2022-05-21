
namespace ICSMeter
{

  namespace UI
  {

    namespace DataMode
    {

      String valueFilter, valueMode;
      String oldValueFilter, oldValueMode;


      bool needs_redraw()
      {
        return (valueFilter!=oldValueFilter || valueMode!=oldValueMode);
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
        if( force_redraw || valueFilter!=oldValueFilter ) {
          if( valueFilter!="" )
            CSS::drawStyledBox( &tft, valueFilter.c_str(), 40, 198, 40, 15, &Theme::BadgeBoxStyle );
          oldValueFilter = valueFilter;
        }
        if( force_redraw || valueMode!=oldValueMode ) {
          if( valueMode!="" )
            CSS::drawStyledBox( &tft, valueMode.c_str(), 240, 198, 40, 15, &Theme::BadgeBoxStyle );
          oldValueMode = valueMode;
        }
      }


    };

  };

};

