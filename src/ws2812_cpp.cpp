/**
 * @file ws2812_cpp.cpp
 * @brief WS2812Strip C++ wrapper implementation
 * @copyright Copyright (c) 2024-2025 HardFOC. All rights reserved.
 */

#include "ws2812_cpp.hpp"
#include "driver/rmt_types.h"

#ifdef __cplusplus

WS2812Strip::WS2812Strip(gpio_num_t gpio, int channel, uint32_t num_leds,
#if CONFIG_WS2812_LED_TYPE_RGBW
                         LedType type,
#else
                         LedType type,
#endif
                         uint16_t t0h, uint16_t t1h, uint16_t t0l, uint16_t t1l, uint8_t brightness)
    : pixels_(num_leds, 0), buffer_(num_leds * (type == LedType::RGBW ? 32 : 24)),
      rmt_(gpio, 10'000'000, 64, false, 4, RMT_CLK_SRC_DEFAULT, channel), gpio_(gpio),
      channel_(channel), type_(type), t0h_(t0h), t1h_(t1h), t0l_(t0l), t1l_(t1l),
      brightness_(brightness), num_leds_(num_leds) {
}

esp_err_t WS2812Strip::Begin() {
  return ESP_OK; // channel already active in constructor
}

void WS2812Strip::SetPixel(uint32_t index, uint32_t rgbw) {
  if (index < num_leds_) {
    pixels_[index] = rgbw;
  }
}

uint32_t WS2812Strip::Length() const {
  return num_leds_;
}

esp_err_t WS2812Strip::Show() {
  uint32_t bits_per_led = (type_ == LedType::RGBW) ? 32 : 24;
  for (uint32_t led = 0; led < num_leds_; ++led) {
    uint32_t color = pixels_[led];
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    uint32_t bits = 0;

    r = (r * brightness_) / 255;
    g = (g * brightness_) / 255;
    b = (b * brightness_) / 255;

    if (type_ == LedType::RGBW) {
      uint8_t w = (color >> 24) & 0xFF;
      w = (w * brightness_) / 255;
      bits = (w << 24) | (r << 16) | (g << 8) | b;
    } else {
      bits = (r << 16) | (g << 8) | b;
    }

    uint32_t mask = 1U << (bits_per_led - 1);
    for (uint32_t bit = 0; bit < bits_per_led; ++bit) {
      bool set = bits & mask;
      buffer_[led * bits_per_led + bit] = set ? (rmt_symbol_word_t){.duration0 = (uint16_t)t1h_,
                                                                   .level0 = 1,
                                                                   .duration1 = (uint16_t)t1l_,
                                                                   .level1 = 0}
                                             : (rmt_symbol_word_t){.duration0 = (uint16_t)t0h_,
                                                                   .level0 = 1,
                                                                   .duration1 = (uint16_t)t0l_,
                                                                   .level1 = 0};
      mask >>= 1;
    }
  }

  return rmt_.Transmit(buffer_.data(), num_leds_ * bits_per_led);
}

void WS2812Strip::SetBrightness(uint8_t value) {
  brightness_ = value;
}

void WS2812Strip::SetTimings(uint16_t t0h, uint16_t t1h, uint16_t t0l, uint16_t t1l) {
  t0h_ = t0h;
  t1h_ = t1h;
  t0l_ = t0l;
  t1l_ = t1l;
}

uint32_t WS2812Strip::ColorWheel(uint8_t pos) {
  pos = 255 - pos;
  if (pos < 85) {
    return ((255 - pos * 3) << 16) | (0 << 8) | (pos * 3);
  } else if (pos < 170) {
    pos -= 85;
    return (0 << 16) | ((pos * 3) << 8) | (255 - pos * 3);
  } else {
    pos -= 170;
    return ((pos * 3) << 16) | ((255 - pos * 3) << 8) | 0;
  }
}

#endif // __cplusplus
