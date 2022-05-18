#include "CSS.hpp"

#include "../../assets/font.hpp"
//#include "../Themes/Themes.hpp"

namespace ICSMeter
{

  namespace CSS
  {
    LGFX_Sprite *spriteCSS = nullptr;

    // helper for setFontStyle
    void setTextStyle( M5GFX *dst, const TextStyle_t *style )
    {
      assert(dst);
      lgfx::TextStyle myStyle;// = { // ; // why no constructor ??
      myStyle.fore_rgb888 = style->fgColor;
      myStyle.back_rgb888 = style->bgColor;
      myStyle.size_x      = style->size;
      myStyle.size_y      = style->size;
      myStyle.datum       = style->datum;
      myStyle.padding_x   = style->paddingX;
      myStyle.utf8        = true;
      myStyle.cp437       = false;

      dst->setTextStyle( myStyle );
    }


    void setFontStyle( M5GFX *dst, const FontStyle_t *fontStyle )
    {
      assert(dst);
      if( !fontStyle ) return;
      if( fontStyle->fontFace ) dst->setFont( fontStyle->fontFace );
      if( fontStyle->textStyle ) setTextStyle( dst, fontStyle->textStyle );
      if( fontStyle->isTransparent ) dst->setTextColor( fontStyle->textStyle->fgColor );
    }


    void drawStyledString( M5GFX* dst, const char* str, int32_t x, int32_t y, const FontStyle_t *style )
    {
      assert(dst);
      if( style ) setFontStyle( dst, style );
      dst->drawString( str, x, y );
    }


    void drawStyledBox_tft( M5GFX* gfx, const char* str, int32_t x, int32_t y, uint32_t w, uint32_t h, const TextBoxStyle_t* boxStyle )
    {
      gfx->fillRoundRect(x, y, w, h, boxStyle->borderRadius, boxStyle->fillColor );
      if( boxStyle->borderWidth > 0 ) {
        gfx->drawRoundRect(x, y, w, h, boxStyle->borderRadius, boxStyle->borderColor);
      }
      setFontStyle( gfx, boxStyle->fontStyle );
      gfx->setTextDatum( MC_DATUM );
      gfx->drawString( str, x+w/2, y+h/2 );
    }


    void drawStyledBox_sprite( LGFX_Sprite* gfx, const char* str, int32_t x, int32_t y, uint32_t w, uint32_t h, const TextBoxStyle_t* boxStyle )
    {
      uint32_t transparent_color = 0x123456U;
      spriteCSS->fillSprite( transparent_color );
      spriteCSS->fillRoundRect(0, 0, w, h, boxStyle->borderRadius, boxStyle->fillColor );
      if( boxStyle->borderWidth > 0 ) {
        spriteCSS->drawRoundRect(0, 0, w, h, boxStyle->borderRadius, boxStyle->borderColor);
      }

      setFontStyle( (M5GFX*)spriteCSS, boxStyle->fontStyle );

      int32_t tx, ty;
      uint16_t tw = spriteCSS->textWidth( str );
      __attribute__((unused)) uint16_t th = spriteCSS->fontHeight();

      bool wrap = true;
      switch( boxStyle->fontStyle->textStyle->datum ) {
        case TL_DATUM: tx=boxStyle->paddingX; ty=boxStyle->paddingY; wrap = true;  break;
        case TC_DATUM: tx=w/2;                ty=boxStyle->paddingY; wrap = false; break;
        case TR_DATUM: tx=w-tw-1;             ty=boxStyle->paddingY; wrap = true;  break;
        case ML_DATUM: tx=boxStyle->paddingX; ty=h/2+1;              wrap = true;  break;
        case MC_DATUM: tx=w/2;                ty=h/2+1;              wrap = false; break;
        case MR_DATUM: tx=w-tw-1;             ty=h/2+1;              wrap = true;  break;
        case BL_DATUM: tx=boxStyle->paddingX; ty=h-1;                wrap = true;  break;
        case BC_DATUM: tx=w/2;                ty=h-1;                wrap = false; break;
        case BR_DATUM: tx=w-tw-1;             ty=h-1;                wrap = true;  break;
        default: tx=boxStyle->paddingX;       ty=boxStyle->paddingY; wrap = false; break;
      }
      spriteCSS->setTextWrap(wrap);
      spriteCSS->drawString( str, tx, ty );
      spriteCSS->pushSprite( x, y, transparent_color );
    }


    void drawStyledBox( M5GFX* gfx, const char* str, int32_t x, int32_t y, uint32_t w, uint32_t h, const TextBoxStyle_t* boxStyle )
    {
      assert(gfx);
      assert(boxStyle);

      if( spriteCSS == nullptr ) {
        spriteCSS = new LGFX_Sprite( gfx );
        spriteCSS->setColorDepth( gfx->getColorDepth() );
      }
      if( !spriteCSS->createSprite( w, h ) ) {
        log_e("Not enough memory, rendering without text datum");
        drawStyledBox_tft( gfx, str, x, y, w, h, boxStyle );
        return;
      } else {
        drawStyledBox_sprite( spriteCSS, str, x, y, w, h, boxStyle );
        spriteCSS->deleteSprite();
      }
    }


  };

};
