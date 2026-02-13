/**
 * @file ws2812_effects.cpp
 * @brief LED animation effects implementation
 * @copyright Copyright (c) 2024-2025 HardFOC. All rights reserved.
 */

#include "ws2812_effects.hpp"
#include "driver/rmt_types.h"

#ifdef __cplusplus

WS2812Animator::WS2812Animator(WS2812Strip& strip, uint32_t virtual_length)
    : strip_(strip), virtual_length_(virtual_length) {}

void WS2812Animator::SetEffect(Effect effect, uint32_t color) {
  effect_ = effect;
  color_ = color;
  step_ = 0;
  dir_ = 1;
  brightness_ = 0;
}

void WS2812Animator::SetVirtualLength(uint32_t length) {
  virtual_length_ = length;
}

void WS2812Animator::SetStep(uint16_t step) {
  step_ = step;
}

uint16_t WS2812Animator::Step() const {
  return step_;
}

void WS2812Animator::Tick() {
  if (!initialized_) {
    if (strip_.Begin() == ESP_OK) {
      initialized_ = true;
    } else {
      return;
    }
  }

  uint32_t strip_len = strip_.Length();
  uint32_t virt_len = virtual_length_ ? virtual_length_ : strip_len;

  switch (effect_) {
  case Effect::SolidColor:
    for (uint32_t i = 0; i < strip_len; ++i) {
      strip_.SetPixel(i, color_);
    }
    strip_.Show();
    break;

  case Effect::Rainbow:
    for (uint32_t i = 0; i < strip_len; ++i) {
      uint32_t v_index = (virt_len == strip_len) ? i : (i * virt_len / strip_len);
      uint8_t pos = static_cast<uint8_t>((v_index * 256 / virt_len + step_) & 0xFF);
      strip_.SetPixel(i, WS2812Strip::ColorWheel(pos));
    }
    strip_.Show();
    step_ = static_cast<uint16_t>((step_ + 1) & 0xFF);
    break;

  case Effect::Chase:
    for (uint32_t i = 0; i < strip_len; ++i) {
      if (((i + step_) % 3) == 0) {
        strip_.SetPixel(i, color_);
      } else {
        strip_.SetPixel(i, 0);
      }
    }
    strip_.Show();
    step_ = static_cast<uint16_t>((step_ + 1) % 3);
    break;

  case Effect::Blink:
    for (uint32_t i = 0; i < strip_len; ++i) {
      strip_.SetPixel(i, (step_ & 1) ? color_ : 0);
    }
    strip_.Show();
    step_++;
    break;

  case Effect::Breath:
    brightness_ = (step_ < 256) ? step_ : 511 - step_;
    strip_.SetBrightness(brightness_);
    for (uint32_t i = 0; i < strip_len; ++i) {
      strip_.SetPixel(i, color_);
    }
    strip_.Show();
    step_ = static_cast<uint16_t>((step_ + 4) % 512);
    break;

  case Effect::Larson:
    for (uint32_t i = 0; i < strip_len; ++i) {
      strip_.SetPixel(i, (i == step_) ? color_ : 0);
    }
    strip_.Show();
    if ((step_ == 0 && dir_ < 0) || (step_ == strip_len - 1 && dir_ > 0)) {
      dir_ = -dir_;
    }
    step_ = static_cast<uint16_t>(step_ + dir_);
    break;

  default:
    for (uint32_t i = 0; i < strip_len; ++i) {
      strip_.SetPixel(i, 0);
    }
    strip_.Show();
    break;
  }
}

#endif // __cplusplus
