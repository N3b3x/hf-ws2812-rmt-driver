#ifndef WS2812_MULTI_ANIMATOR_HPP
#define WS2812_MULTI_ANIMATOR_HPP

#include "ws2812_effects.hpp"
#include <vector>

#ifdef __cplusplus
/**
 * @file ws2812_multi_animator.hpp
 * @brief Helper coordinating animations across multiple LED strips.
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
  explicit WS2812MultiAnimator(const std::vector<WS2812Strip *> &strips,
                               bool unified = true, bool sync = true);

  /**
   * @brief Apply an effect to all managed strips.
   */
  void setEffect(WS2812Animator::Effect eff, uint32_t color = 0xFFFFFF);

  /**
   * @brief Apply an effect to a single strip.
   *
   * @param idx   Index of the strip.
   * @param eff   Desired animation effect.
   * @param color Base colour for the effect.
   */
  void setEffect(size_t idx, WS2812Animator::Effect eff,
                 uint32_t color = 0xFFFFFF);

  /**
   * @brief Advance all animations by one step.
   */
  void tick();

private:
  /** Animators for each strip. */
  std::vector<WS2812Animator> m_animators;
  /** Synchronise step counters across strips. */
  bool m_sync = true;
  /** Current animation step used for synchronisation. */
  uint16_t m_step = 0;
};
#endif // __cplusplus

#endif // WS2812_MULTI_ANIMATOR_HPP
