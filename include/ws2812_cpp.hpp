/*
 * ws2812_cpp.hpp
 *
 * Optional C++ wrapper for the HF-WS2812 driver.
 *
 * This header defines a WS2812Strip class which wraps the C API
 * provided in ws2812_control.h in a convenient C++ interface and
 * includes a helper for generating colour wheel values.
 */

#ifndef WS2812_CPP_HPP
#define WS2812_CPP_HPP

#include "ws2812_control.h"
#include "rmt_wrapper.hpp"
#include <vector>
#include <cstdint>

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
                         rmt_channel_t channel = (rmt_channel_t)CONFIG_WS2812_LED_RMT_TX_CHANNEL,
                         uint32_t numLeds = NUM_LEDS,
#if CONFIG_WS2812_LED_TYPE_RGBW
                         LedType type = LedType::RGBW,
#else
                         LedType type = LedType::RGB,
#endif
                         uint16_t t0h = WS2812_T0H,
                         uint16_t t1h = WS2812_T1H,
                         uint16_t t0l = WS2812_T0L,
                         uint16_t t1l = WS2812_T1L,
                         uint8_t brightness = WS2812_DEFAULT_BRIGHTNESS);

    /**
     * @brief Initialise the underlying driver.
     *
     * @return ESP_OK on success.
     */
    esp_err_t begin();

    /**
     * @brief Set the colour of a single LED.
     *
     * @param index Index of the LED starting from 0.
     * @param rgbw Packed 24/32-bit colour value.
     */
    void setPixel(uint32_t index, uint32_t rgbw);

    /** Get the number of LEDs managed by this strip. */
    uint32_t length() const;

    /**
     * @brief Send the currently stored colours to the LED strip.
     */
    esp_err_t show();

    /**
     * @brief Set global brightness for the strip.
     *
     * @param value Brightness 0-255.
     */
    void setBrightness(uint8_t value);

    /**
     * @brief Update the bit timing parameters.
     *
     * Allows adjustment of the WS2812 protocol timings after construction.
     */
    void setTimings(uint16_t t0h, uint16_t t1h, uint16_t t0l, uint16_t t1l);

    /**
     * @brief Generate a colour from a 0-255 position on a colour wheel.
     *
     * @param pos Position on the colour wheel.
     * @return Packed RGB colour.
     */
    static uint32_t colorWheel(uint8_t pos);

private:
    std::vector<uint32_t> m_pixels;
    std::vector<rmt_item32_t> m_buffer;
    RmtChannel m_rmt;
    gpio_num_t m_gpio;
    rmt_channel_t m_channel;
    LedType m_type;
    uint16_t m_t0h;
    uint16_t m_t1h;
    uint16_t m_t0l;
    uint16_t m_t1l;
    uint8_t m_brightness;
    uint32_t m_numLeds;
};


#endif // __cplusplus

#endif // WS2812_CPP_HPP
