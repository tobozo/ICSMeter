#pragma once

#include "../../core/ICSMeter.hpp"


namespace ICSMeter
{

  namespace CSS
  {

    typedef const std::uint32_t boxcolor_t; // just a shorhand so the next struct declaration has less code density :)
    typedef std::uint32_t textcolor_t; // just a shorhand so the next struct declaration has less code density :)
    typedef const bool transparency_t;

    constexpr transparency_t TRANSPARENT = true;
    constexpr transparency_t OPAQUE      = false;

    // same struct as lgfx::TextStyle without the defaults
    struct TextStyle_t
    {
      const textcolor_t fgColor  ;// = 0xFFFFFFU;
      const textcolor_t bgColor  ;// = 0;
      const float       size     ;// = 1;
      const textdatum_t datum    ;// = textdatum_t::top_left;
      const uint32_t    paddingX ;// = 0;
    };

    // holder for font face + font style
    struct FontStyle_t
    {
      const lgfx::IFont    *fontFace;
      const TextStyle_t    *textStyle;
      const transparency_t isTransparent;
    };

    // Text box renderer
    struct TextBoxStyle_t
    {
      const FontStyle_t* fontStyle;
      boxcolor_t         fillColor;
      boxcolor_t         borderColor;
      boxcolor_t         borderWidth;  // set to 0 to disable border
      boxcolor_t         borderRadius;
      uint32_t           paddingX;
      uint32_t           paddingY;
    };


    void setTextStyle( M5GFX *dst, TextStyle_t *style );
    void setFontStyle( M5GFX *dst, FontStyle_t *myFontStyle );
    void drawStyledString( M5GFX* dst, const char* str, int32_t x, int32_t y, const FontStyle_t *style );
    void drawStyledBox( M5GFX* gfx, const char* str, int32_t x, int32_t y, uint32_t w, uint32_t h, const TextBoxStyle_t* boxStyle );

  };

};
