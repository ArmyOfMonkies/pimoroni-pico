#include "pico_vector.hpp"
#include <vector>

namespace pimoroni {
  void PicoVector::polygon(std::vector<pretty_poly::contour_t<picovector_point_type>> contours, Point origin, int scale) {
    pretty_poly::draw_polygon<picovector_point_type>(
        contours,
        pretty_poly::point_t<int>(origin.x, origin.y),
        scale);
  }

  void PicoVector::rotate(std::vector<pretty_poly::contour_t<picovector_point_type>> &contours, Point origin, float angle) {
    pretty_poly::mat3_t t2 = pretty_poly::mat3_t::translation(origin.x, origin.y);
    pretty_poly::mat3_t t1 = pretty_poly::mat3_t::translation(-origin.x, -origin.y);
    angle = 2 * M_PI * (angle / 360.0f);
    pretty_poly::mat3_t r = pretty_poly::mat3_t::rotation(angle);
    for(auto &contour : contours) {
      for(auto i = 0u; i < contour.count; i++) {
        contour.points[i] *= t1;
        contour.points[i] *= r;
        contour.points[i] *= t2;
      }
    }
  }

  void PicoVector::translate(std::vector<pretty_poly::contour_t<picovector_point_type>> &contours, Point translation) {
    pretty_poly::mat3_t t = pretty_poly::mat3_t::translation(translation.x, translation.y);
    for(auto &contour : contours) {
      for(auto i = 0u; i < contour.count; i++) {
        contour.points[i] *= t;
      }
    }
  }

  void PicoVector::rotate(pretty_poly::contour_t<picovector_point_type> &contour, Point origin, float angle) {
    pretty_poly::mat3_t t2 = pretty_poly::mat3_t::translation(origin.x, origin.y);
    pretty_poly::mat3_t t1 = pretty_poly::mat3_t::translation(-origin.x, -origin.y);
    angle = 2 * M_PI * (angle / 360.0f);
    pretty_poly::mat3_t r = pretty_poly::mat3_t::rotation(angle);
    for(auto i = 0u; i < contour.count; i++) {
      contour.points[i] *= t1;
      contour.points[i] *= r;
      contour.points[i] *= t2;
    }
  }

  void PicoVector::translate(pretty_poly::contour_t<picovector_point_type> &contour, Point translation) {
    pretty_poly::mat3_t t = pretty_poly::mat3_t::translation(translation.x, translation.y);
    for(auto i = 0u; i < contour.count; i++) {
      contour.points[i] *= t;
    }
  }

  Point PicoVector::text(std::string_view text, Point origin) {
    // TODO: Normalize types somehow, so we're not converting?
    pretty_poly::point_t<int> caret = pretty_poly::point_t<int>(origin.x, origin.y);

    // Align text from the bottom left
    caret.y += text_metrics.size;

    int16_t space_width = alright_fonts::measure_character(text_metrics, ' ').w;
    if (space_width == 0) {
      space_width = text_metrics.word_spacing;
    }

    size_t i = 0;

    while(i < text.length()) {
      size_t next_space = text.find(' ', i + 1);

      if(next_space == std::string::npos) {
        next_space = text.length();
      }

      size_t next_linebreak = text.find('\n', i + 1);

      if(next_linebreak == std::string::npos) {
        next_linebreak = text.length();
      }

      size_t next_break = std::min(next_space, next_linebreak);

      uint16_t word_width = 0;
      for(size_t j = i; j < next_break; j++) {
        word_width += alright_fonts::measure_character(text_metrics, text[j]).w;
        word_width += text_metrics.letter_spacing;
      }

      if(caret.x != 0 && caret.x + word_width > graphics->clip.w) {
        caret.x = origin.x;
        caret.y += text_metrics.line_height;
      }

      for(size_t j = i; j < std::min(next_break + 1, text.length()); j++) {
        if (text[j] == '\n') { // Linebreak
          caret.x = origin.x;
          caret.y += text_metrics.line_height;
        } else if (text[j] == ' ') { // Space
          caret.x += space_width;
        } else {
          alright_fonts::render_character(text_metrics, text[j], caret);
        }
        caret.x += alright_fonts::measure_character(text_metrics, text[j]).w;
        caret.x += text_metrics.letter_spacing;
      }

      i = next_break + 1;
    }

    return Point(caret.x, caret.y);
  }

  Point PicoVector::text(std::string_view text, Point origin, float angle) {
    // TODO: Normalize types somehow, so we're not converting?
    pretty_poly::point_t<float> caret(0, 0);

    // Prepare a transformation matrix for character and offset rotation
    angle = 2 * M_PI * (angle / 360.0f);
    pretty_poly::mat3_t transform = pretty_poly::mat3_t::rotation(angle);

    // Align text from the bottom left
    caret.y += text_metrics.line_height;
    caret *= transform;

    pretty_poly::point_t<float> space;
    pretty_poly::point_t<float> carriage_return(0, text_metrics.line_height);

    space.x = alright_fonts::measure_character(text_metrics, ' ').w;
    if (space.x == 0) {
      space.x = text_metrics.word_spacing;
    }

    space *= transform;
    carriage_return *= transform;

    size_t i = 0;

    while(i < text.length()) {
      size_t next_space = text.find(' ', i + 1);

      if(next_space == std::string::npos) {
        next_space = text.length();
      }

      size_t next_linebreak = text.find('\n', i + 1);

      if(next_linebreak == std::string::npos) {
        next_linebreak = text.length();
      }

      size_t next_break = std::min(next_space, next_linebreak);

      uint16_t word_width = 0;
      for(size_t j = i; j < next_break; j++) {
        word_width += alright_fonts::measure_character(text_metrics, text[j]).w;
        word_width += text_metrics.letter_spacing;
      }

      if(caret.x != 0 && caret.x + word_width > graphics->clip.w) {
        caret -= carriage_return;
        carriage_return.x = 0;
      }

      for(size_t j = i; j < std::min(next_break + 1, text.length()); j++) {
        if (text[j] == '\n') { // Linebreak
          caret -= carriage_return;
          carriage_return.x = 0;
        } else if (text[j] == ' ') { // Space
          caret += space;
          carriage_return += space;
        } else {
          alright_fonts::render_character(text_metrics, text[j], pretty_poly::point_t<int>(origin.x + caret.x, origin.y + caret.y), transform);
        }
        pretty_poly::point_t<float> advance(
          alright_fonts::measure_character(text_metrics, text[j]).w + text_metrics.letter_spacing,
          0
        );
        advance *= transform;
        caret += advance;
        carriage_return += advance;
      }

      i = next_break + 1;
    }

    return Point(caret.x, caret.y);
  }
}