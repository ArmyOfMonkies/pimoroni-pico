#include "bitmap_fonts.hpp"

namespace bitmap {
  int32_t measure_character(const font_t *font, const char c, const uint8_t scale) {
    uint8_t char_index = c - 32;
    return font->widths[char_index] * scale;
  }

  int32_t measure_text(const font_t *font, const std::string &t, const uint8_t scale) {
    int32_t text_width = 0;
    for(auto c : t) {
      text_width += measure_character(font, c, scale);
    }
    return text_width;
  }

  void character(const font_t *font, rect_func rectangle, const char c, const int32_t x, const int32_t y, const uint8_t scale) {
    uint8_t char_index = c - 32;

    if(font->height <= 8){
      const uint8_t *d = &((uint8_t *)font->data)[char_index * font->max_width];
      for(uint8_t cx = 0; cx < font->widths[char_index]; cx++) {
        for(uint8_t cy = 0; cy < font->height; cy++) {
          if((1U << cy) & *d) {
            rectangle(x + (cx * scale), y + (cy * scale), scale, scale);
          }
        }

        d++;
      }
    } else {
      const uint16_t *d = &font->data[char_index * font->max_width];
      for(uint8_t cx = 0; cx < font->widths[char_index]; cx++) {
        for(uint8_t cy = 0; cy < font->height; cy++) {
          if((1U << cy) & *d) {
            rectangle(x + (cx * scale), y + (cy * scale), scale, scale);
          }
        }

        d++;
      }
    }
  }

  void text(const font_t *font, rect_func rectangle, const std::string &t, const int32_t x, const int32_t y, const int32_t wrap, const uint8_t scale) {
    uint32_t co = 0, lo = 0; // character and line (if wrapping) offset

    size_t i = 0;
    while(i < t.length()) {
      // find length of current word
      size_t next_space = t.find(' ', i + 1);

      if(next_space == std::string::npos) {
        next_space = t.length();
      }

      uint16_t word_width = 0;
      for(size_t j = i; j < next_space; j++) {
        word_width += font->widths[t[j] - 32] * scale;
      }

      // if this word would exceed the wrap limit then
      // move to the next line
      if(co != 0 && co + word_width > (uint32_t)wrap) {
        co = 0;
        lo += (font->height + 1) * scale;
      }

      // draw word
      for(size_t j = i; j < next_space; j++) {
        character(font, rectangle, t[j], x + co, y + lo, scale);
        co += font->widths[t[j] - 32] * scale;
      }

      // move character offset to end of word and add a space
      co += font->widths[0] * scale;
      i = next_space + 1;
    }
  }
}