/**
 * @file ws2812_cpp.hpp
 * @brief C++ wrapper for the HF-WS2812 RMT LED driver
 * @copyright Copyright (c) 2024-2025 HardFOC. All rights reserved.
 */

#pragma once
#include "rmt_wrapper.hpp"
#include "ws2812_control.h"
#include <cstdint>
#include <vector>

#ifdef __cplusplus

/** LED colour encoding. */
enum class LedType { RGB, RGBW };

/**
 * @brief C++ convenience wrapper for a chain of WS2812 LEDs.
 *
 * All configuration such as GPIO, channel, LED type and timing can be
 * supplied at runtime via the constructor, allowing multiple strips
 * with different settings.
 */
class WS2812Strip {
public:
  /**
   * @brief Construct a strip object.
   *
   * @param gpio    Output pin connected to the LEDs.
   * @param channel RMT channel for signal generation.
   */
  explicit WS2812Strip(gpio_num_t gpio = (gpio_num_t)CONFIG_WS2812_LED_RMT_TX_GPIO,
                       int channel = CONFIG_WS2812_LED_RMT_TX_CHANNEL, uint32_t num_leds = NUM_LEDS,
#if CONFIG_WS2812_LED_TYPE_RGBW
                       LedType type = LedType::RGBW,
#else
                       LedType type = LedType::RGB,
#endif
                       uint16_t t0h = WS2812_T0H, uint16_t t1h = WS2812_T1H,
                       uint16_t t0l = WS2812_T0L, uint16_t t1l = WS2812_T1L,
                       uint8_t brightness = WS2812_DEFAULT_BRIGHTNESS);

  /**
   * @brief Initialise the underlying driver.
   *
   * @return ESP_OK on success.
   */
  esp_err_t Begin();

  /**
   * @brief Set the colour of a single LED.
   *
   * @param index Index of the LED starting from 0.
   * @param rgbw Packed 24/32-bit colour value.
   */
  void SetPixel(uint32_t index, uint32_t rgbw);

  /** Get the number of LEDs managed by this strip. */
  uint32_t Length() const;

  /**
   * @brief Send the currently stored colours to the LED strip.
   */
  esp_err_t Show();

  /**
   * @brief Set global brightness for the strip.
   *
   * @param value Brightness 0-255.
   */
  void SetBrightness(uint8_t value);

  /**
   * @brief Resize the strip to a new LED count at runtime.
   *
   * Clears all pixel data to zero. The RMT symbol buffer is resized
   * to match the new LED count and current colour encoding.
   *
   * @param num_leds New number of LEDs in the strip.
   */
  void SetNumLeds(uint32_t num_leds);

  /**
   * @brief Update the bit timing parameters.
   *
   * Allows adjustment of the WS2812 protocol timings after construction.
   */
  void SetTimings(uint16_t t0h, uint16_t t1h, uint16_t t0l, uint16_t t1l);

  /**
   * @brief Generate a colour from a 0-255 position on a colour wheel.
   *
   * @param pos Position on the colour wheel.
   * @return Packed RGB colour.
   */
  static uint32_t ColorWheel(uint8_t pos);

private:
  std::vector<uint32_t> pixels_;
  std::vector<rmt_symbol_word_t> buffer_;
  ws2812::RmtTx rmt_;
  gpio_num_t gpio_;
  int channel_;
  LedType type_;
  uint16_t t0h_;
  uint16_t t1h_;
  uint16_t t0l_;
  uint16_t t1l_;
  uint8_t brightness_;
  uint32_t num_leds_;
};

#endif // __cplusplus
