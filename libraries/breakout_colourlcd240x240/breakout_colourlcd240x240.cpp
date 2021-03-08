#include "breakout_colourlcd240x240.hpp"

namespace pimoroni {

  BreakoutColourLCD240x240::BreakoutColourLCD240x240(uint16_t *buf)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf)  {
    __fb = buf;
  }

  BreakoutColourLCD240x240::BreakoutColourLCD240x240(uint16_t *buf,  spi_inst_t *spi,
      uint8_t cs, uint8_t dc, uint8_t sck, uint8_t mosi, uint8_t miso)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf, spi, cs, dc, sck, mosi, miso)  {
    __fb = buf;
  }

  void BreakoutColourLCD240x240::init() {
    // initialise the screen
    screen.init();
  }

  spi_inst_t* BreakoutColourLCD240x240::get_spi() const {
    return screen.get_spi();
  }

  int BreakoutColourLCD240x240::get_cs() const {
    return screen.get_cs();
  }

  int BreakoutColourLCD240x240::get_dc() const {
    return screen.get_dc();
  }

  int BreakoutColourLCD240x240::get_sck() const {
    return screen.get_sck();
  }

  int BreakoutColourLCD240x240::get_mosi() const {
    return screen.get_mosi();
  }

  int BreakoutColourLCD240x240::get_miso() const {
    return screen.get_miso();
  }

  void BreakoutColourLCD240x240::update() {
    screen.update();
  }

  void BreakoutColourLCD240x240::set_backlight(uint8_t brightness) {
    screen.set_backlight(brightness);
  }

}