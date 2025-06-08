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
                         uint8_t brightness = WS2812_DEFAULT_BRIGHTNESS)
        : m_rmt(gpio, channel), m_gpio(gpio), m_channel(channel), m_numLeds(numLeds),
          m_type(type), m_t0h(t0h), m_t1h(t1h), m_t0l(t0l), m_t1l(t1l),
          m_brightness(brightness),
          m_pixels(numLeds, 0), m_buffer(numLeds * (type == LedType::RGBW ? 32 : 24))
    {}

    /**
     * @brief Initialise the underlying driver.
     *
     * @return ESP_OK on success.
     */
    esp_err_t begin() { return m_rmt.begin(); }

    /**
     * @brief Set the colour of a single LED.
     *
     * @param index Index of the LED starting from 0.
     * @param rgbw Packed 24/32-bit colour value.
     */
    void setPixel(uint32_t index, uint32_t rgbw) {
        if (index < m_numLeds) {
            m_pixels[index] = rgbw;
        }
    }

    /** Get the number of LEDs managed by this strip. */
    uint32_t length() const { return m_numLeds; }

    /**
     * @brief Send the currently stored colours to the LED strip.
     */
    esp_err_t show();

    /**
     * @brief Set global brightness for the strip.
     *
     * @param value Brightness 0-255.
     */
    void setBrightness(uint8_t value) { m_brightness = value; }

    /**
     * @brief Update the bit timing parameters.
     *
     * Allows adjustment of the WS2812 protocol timings after construction.
     */
    void setTimings(uint16_t t0h, uint16_t t1h, uint16_t t0l, uint16_t t1l)
    {
        m_t0h = t0h;
        m_t1h = t1h;
        m_t0l = t0l;
        m_t1l = t1l;
    }

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

inline uint32_t WS2812Strip::colorWheel(uint8_t pos)
{
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

inline esp_err_t WS2812Strip::show()
{
    uint32_t bitsPerLed = (m_type == LedType::RGBW) ? 32 : 24;
    for (uint32_t led = 0; led < m_numLeds; ++led) {
        uint32_t color = m_pixels[led];
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        uint32_t bits = 0;

        r = (r * m_brightness) / 255;
        g = (g * m_brightness) / 255;
        b = (b * m_brightness) / 255;

        if (m_type == LedType::RGBW) {
            uint8_t w = (color >> 24) & 0xFF;
            w = (w * m_brightness) / 255;
            bits = (w << 24) | (r << 16) | (g << 8) | b;
        } else {
            bits = (r << 16) | (g << 8) | b;
        }

        uint32_t mask = 1U << (bitsPerLed - 1);
        for (uint32_t bit = 0; bit < bitsPerLed; ++bit) {
            bool set = bits & mask;
            m_buffer[led * bitsPerLed + bit] =
                set ? (rmt_item32_t){{{m_t1h, 1, m_t1l, 0}}}
                    : (rmt_item32_t){{{m_t0h, 1, m_t0l, 0}}};
            mask >>= 1;
        }
    }

    return m_rmt.transmit(m_buffer.data(), m_numLeds * bitsPerLed);
}

#endif // __cplusplus

#endif // WS2812_CPP_HPP
