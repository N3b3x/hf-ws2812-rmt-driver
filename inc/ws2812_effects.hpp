#ifndef WS2812_EFFECTS_HPP
#define WS2812_EFFECTS_HPP

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

  explicit WS2812Animator(WS2812Strip& strip, uint32_t virtualLength = 0);

  /**
   * @brief Select the active effect.
   *
   * @param effect Desired pattern.
   * @param color  Base colour used by some effects.
   */
  void setEffect(Effect effect, uint32_t color = 0xFFFFFF);

  /** Adjust the virtual length used for effects. */
  void setVirtualLength(uint32_t length);

  /** Set the current step (used for external synchronization). */
  void setStep(uint16_t step);
  uint16_t step() const;

  /**
   * @brief Advance the animation by one step.
   */
  void tick();

private:
  WS2812Strip& m_strip;
  bool m_initialized = false;
  uint32_t m_virtualLength = 0;
  Effect m_effect = Effect::Off;
  uint32_t m_color = 0xFFFFFF;
  uint16_t m_step = 0;
  int m_dir = 1;
  uint8_t m_brightness = 0;
};

#endif // __cplusplus

#endif // WS2812_EFFECTS_HPP
