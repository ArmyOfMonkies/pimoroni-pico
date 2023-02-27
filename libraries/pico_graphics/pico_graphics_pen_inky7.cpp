#include "pico_graphics.hpp"

namespace pimoroni {
  PicoGraphics_PenInky7::PicoGraphics_PenInky7(uint16_t width, uint16_t height, IDirectDisplayDriver<uint8_t> &direct_display_driver)
  : PicoGraphics(width, height, nullptr),
    driver(direct_display_driver) {
      this->pen_type = PEN_INKY7;
  }
  void PicoGraphics_PenInky7::set_pen(uint c) {
    color = c & 0x7;
  }
  void PicoGraphics_PenInky7::set_pen(uint8_t r, uint8_t g, uint8_t b) {
  }
  int PicoGraphics_PenInky7::create_pen(uint8_t r, uint8_t g, uint8_t b) {
    return 0;
  }
  void PicoGraphics_PenInky7::set_pixel(const Point &p) {
    driver.write_pixel(p, color);
  }
  void PicoGraphics_PenInky7::set_pixel_span(const Point &p, uint l) {
    driver.write_pixel_span(p, l, color);
  }
  void PicoGraphics_PenInky7::get_dither_candidates(const RGB &col, const RGB *palette, size_t len, std::array<uint8_t, 16> &candidates) {
    RGB error;
    for(size_t i = 0; i < candidates.size(); i++) {
        candidates[i] = (col + error).closest(palette, len);
        error += (col - palette[candidates[i]]);
    }

    // sort by a rough approximation of luminance, this ensures that neighbouring
    // pixels in the dither matrix are at extreme opposites of luminence
    // giving a more balanced output
    std::sort(candidates.begin(), candidates.end(), [palette](int a, int b) {
        return palette[a].luminance() > palette[b].luminance();
    });
  }
  void PicoGraphics_PenInky7::set_pixel_dither(const Point &p, const RGB &c) {
    if(!bounds.contains(p)) return;

    if(!cache_built) {
        for(uint i = 0; i < 512; i++) {
            uint r = (i & 0x1c0) >> 1;
            uint g = (i & 0x38) << 2;
            uint b = (i & 0x7) << 5;
            RGB cache_col(
                r | (r >> 3) | (r >> 6),
                g | (g >> 3) | (g >> 6),
                b | (b >> 3) | (b >> 6)
            );
            get_dither_candidates(cache_col, palette, palette_size, candidate_cache[i]);
        }
        cache_built = true;
    }

    uint cache_key = ((c.r & 0xE0) << 1) | ((c.g & 0xE0) >> 2) | ((c.b & 0xE0) >> 5);
    //get_dither_candidates(c, palette, 256, candidates);

    // find the pattern coordinate offset
    uint pattern_index = (p.x & 0b11) | ((p.y & 0b11) << 2);

    // set the pixel
    //color = candidates[pattern[pattern_index]];
    color = candidate_cache[cache_key][dither16_pattern[pattern_index]];
    set_pixel(p);
  }
  void PicoGraphics_PenInky7::frame_convert(PenType type, conversion_callback_func callback) {
    if(type == PEN_INKY7) {
      uint byte_count = bounds.w/2;
      uint8_t buffer[bounds.w];

      for(int32_t r = 0; r < bounds.h; r++)
      {
        driver.read_pixel_span(Point(0, r), bounds.w, buffer);
        // for(int y=0; y < 800; y++)
        //   buffer[y] = rand() % 7;

        // convert byte storage to nibble storage
        uint8_t *pDst = buffer;
        uint8_t *pSrc = buffer;
                
        for(uint c = 0; c < byte_count; c++)
        {
          uint8_t nibble = ((*pSrc++) << 4);
          nibble |= ((*pSrc++) & 0xf);
          *pDst++ = nibble;
        }

        callback(buffer, byte_count);
      }
    }
  }
}