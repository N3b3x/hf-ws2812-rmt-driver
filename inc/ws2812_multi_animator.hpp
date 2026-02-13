#pragma once
#include "ws2812_effects.hpp"
#include <vector>

#ifdef __cplusplus
/**
 * @file ws2812_multi_animator.hpp
 * @brief Helper coordinating animations across multiple LED strips
 * @copyright Copyright (c) 2024-2025 HardFOC. All rights reserved.
 */
class WS2812MultiAnimator {
public:
  /**
   * @brief Construct from a list of strips.
   *
   * @param strips   Pointers to WS2812Strip objects.
   * @param unified  If true all strips share the longest length.
   * @param sync     If true keep animation steps in sync.
   */
  explicit WS2812MultiAnimator(const std::vector<WS2812Strip*>& strips, bool unified = true,
                               bool sync = true);

  /**
   * @brief Apply an effect to all managed strips.
   */
  void SetEffect(WS2812Animator::Effect effect, uint32_t color = 0xFFFFFF);

  /**
   * @brief Apply an effect to a single strip.
   *
   * @param index   Index of the strip.
   * @param effect   Desired animation effect.
   * @param color Base colour for the effect.
   */
  void SetEffect(size_t index, WS2812Animator::Effect effect, uint32_t color = 0xFFFFFF);

  /**
   * @brief Advance all animations by one step.
   */
  void Tick();

private:
  /** Animators for each strip. */
  std::vector<WS2812Animator> animators_;
  /** Synchronise step counters across strips. */
  bool sync_ = true;
  /** Current animation step used for synchronisation. */
  uint16_t step_ = 0;
};
#endif // __cplusplus
