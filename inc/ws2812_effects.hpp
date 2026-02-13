/**
 * @file ws2812_effects.hpp
 * @brief LED animation effects for WS2812 strips
 * @copyright Copyright (c) 2024-2025 HardFOC. All rights reserved.
 */
#pragma once
#include "ws2812_cpp.hpp"

#ifdef __cplusplus

/**
 * @brief Helper class implementing a few basic WS2812 LED animations.
 *
 * Call tick() periodically (for example from a FreeRTOS task)
 * to update the currently selected effect.
 */
class WS2812Animator {
public:
  enum class Effect { Off, SolidColor, Rainbow, Chase, Blink, Breath, Larson };

  explicit WS2812Animator(WS2812Strip& strip, uint32_t virtual_length = 0);

  /**
   * @brief Select the active effect.
   *
   * @param effect Desired pattern.
   * @param color  Base colour used by some effects.
   */
  void SetEffect(Effect effect, uint32_t color = 0xFFFFFF);

  /** Adjust the virtual length used for effects. */
  void SetVirtualLength(uint32_t length);

  /** Set the current step (used for external synchronization). */
  void SetStep(uint16_t step);
  uint16_t Step() const;

  /**
   * @brief Advance the animation by one step.
   */
  void Tick();

private:
  WS2812Strip& strip_;
  bool initialized_ = false;
  uint32_t virtual_length_ = 0;
  Effect effect_ = Effect::Off;
  uint32_t color_ = 0xFFFFFF;
  uint16_t step_ = 0;
  int dir_ = 1;
  uint8_t brightness_ = 0;
};

#endif // __cplusplus
