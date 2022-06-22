#line 1 "/home/runner/work/ICSMeter/ICSMeter/include/UI/Helpers/CSS.hpp"
#pragma once

#include "../../core/ICSMeter.hpp"
#include "../../assets/font.hpp"


namespace ICSMeter
{

  namespace CSS
  {

    typedef std::uint32_t boxcolor_t;  // shorthand for code density :)
    typedef std::uint32_t textcolor_t; // shorthand for code density :)

    typedef const bool transparency_t; // kinda stupid to alias a bool, but future-proof with opacity

    constexpr transparency_t TRANSPARENT = true;
    constexpr transparency_t OPAQUE      = false;

    // simplified version of lgfx::TextStyle ( size_x and size_y merged, no utf8/cp437 )
    struct TextStyle_t
    {
      textcolor_t       fgColor  ;
      textcolor_t       bgColor  ;
      const float       size     ;
      const textdatum_t datum    ;
      const uint32_t    paddingX ;
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
