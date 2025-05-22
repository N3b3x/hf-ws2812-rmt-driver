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

#ifdef __cplusplus

/**
 * @brief C++ convenience wrapper for a chain of WS2812 LEDs.
 */
class WS2812Strip {
public:
    /**
     * @brief Initialise the underlying driver.
     *
     * @return ESP_OK on success.
     */
    esp_err_t begin() { return ws2812ControlInit(); }

    /**
     * @brief Set the colour of a single LED.
     *
     * @param index Index of the LED starting from 0.
     * @param rgbw Packed 24/32-bit colour value.
     */
    void setPixel(uint32_t index, uint32_t rgbw) {
        if (index < NUM_LEDS) {
            state.leds[index] = rgbw;
        }
    }

    /**
     * @brief Send the currently stored colours to the LED strip.
     */
    esp_err_t show() { return ws2812WriteLeds(state); }

    /**
     * @brief Generate a colour from a 0-255 position on a colour wheel.
     *
     * @param pos Position on the colour wheel.
     * @return Packed RGB colour.
     */
    static uint32_t colorWheel(uint8_t pos);

private:
    struct led_state state = {};
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

#endif // __cplusplus

#endif // WS2812_CPP_HPP
