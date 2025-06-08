#include "ws2812_cpp.hpp"

#ifdef __cplusplus

WS2812Strip::WS2812Strip(gpio_num_t gpio,
                         rmt_channel_t channel,
                         uint32_t numLeds,
#if CONFIG_WS2812_LED_TYPE_RGBW
                         LedType type,
#else
                         LedType type,
#endif
                         uint16_t t0h,
                         uint16_t t1h,
                         uint16_t t0l,
                         uint16_t t1l,
                         uint8_t brightness)
    : m_rmt(gpio, channel),
      m_gpio(gpio),
      m_channel(channel),
      m_numLeds(numLeds),
      m_type(type),
      m_t0h(t0h),
      m_t1h(t1h),
      m_t0l(t0l),
      m_t1l(t1l),
      m_brightness(brightness),
      m_pixels(numLeds, 0),
      m_buffer(numLeds * (type == LedType::RGBW ? 32 : 24))
{}

esp_err_t WS2812Strip::begin()
{
    return m_rmt.begin();
}

void WS2812Strip::setPixel(uint32_t index, uint32_t rgbw)
{
    if (index < m_numLeds) {
        m_pixels[index] = rgbw;
    }
}

uint32_t WS2812Strip::length() const
{
    return m_numLeds;
}

esp_err_t WS2812Strip::show()
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

void WS2812Strip::setBrightness(uint8_t value)
{
    m_brightness = value;
}

void WS2812Strip::setTimings(uint16_t t0h, uint16_t t1h, uint16_t t0l, uint16_t t1l)
{
    m_t0h = t0h;
    m_t1h = t1h;
    m_t0l = t0l;
    m_t1l = t1l;
}

uint32_t WS2812Strip::colorWheel(uint8_t pos)
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
