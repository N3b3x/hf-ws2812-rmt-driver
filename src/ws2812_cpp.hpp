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

#ifdef __cplusplus

/**
 * @brief C++ convenience wrapper for a chain of WS2812 LEDs.
 *
 * The constructor optionally accepts a GPIO number and RMT channel so
 * the strip can be initialised at runtime without changing Kconfig.
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
                         uint32_t numLeds = NUM_LEDS)
        : m_rmt(gpio, channel), m_gpio(gpio), m_channel(channel), m_numLeds(numLeds),
          m_pixels(numLeds, 0), m_buffer(numLeds * WS2812_BITS_PER_LED)
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
    uint8_t m_brightness = WS2812_DEFAULT_BRIGHTNESS;
    uint32_t m_numLeds = NUM_LEDS;
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
    for (uint32_t led = 0; led < m_numLeds; ++led) {
        uint32_t color = m_pixels[led];
#if CONFIG_WS2812_LED_TYPE_RGBW
        uint8_t w = (color >> 24) & 0xFF;
#endif
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;

        r = (r * m_brightness) / 255;
        g = (g * m_brightness) / 255;
        b = (b * m_brightness) / 255;
#if CONFIG_WS2812_LED_TYPE_RGBW
        w = (w * m_brightness) / 255;
        uint32_t bits = (w << 24) | (r << 16) | (g << 8) | b;
#else
        uint32_t bits = (r << 16) | (g << 8) | b;
#endif
        uint32_t mask = 1U << (WS2812_BITS_PER_LED - 1);
        for (uint32_t bit = 0; bit < WS2812_BITS_PER_LED; ++bit) {
            bool set = bits & mask;
            m_buffer[led * WS2812_BITS_PER_LED + bit] =
                set ? (rmt_item32_t){{{WS2812_T1H, 1, WS2812_T1L, 0}}}
                    : (rmt_item32_t){{{WS2812_T0H, 1, WS2812_T0L, 0}}};
            mask >>= 1;
        }
    }

    return m_rmt.transmit(m_buffer.data(), m_numLeds * WS2812_BITS_PER_LED);
}

#endif // __cplusplus

#endif // WS2812_CPP_HPP
