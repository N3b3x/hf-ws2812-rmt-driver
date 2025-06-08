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
    enum class Effect {
        Off,
        SolidColor,
        Rainbow,
        Chase,
        Blink,
        Breath,
        Larson
    };

    explicit WS2812Animator(WS2812Strip &strip, uint32_t virtualLength = 0)
        : m_strip(strip), m_virtualLength(virtualLength) {}

    /**
     * @brief Select the active effect.
     *
     * @param effect Desired pattern.
     * @param color  Base colour used by some effects.
     */
    void setEffect(Effect effect, uint32_t color = 0xFFFFFF) {
        m_effect = effect;
        m_color = color;
        m_step = 0;
        m_dir = 1;
        m_brightness = 0;
    }

    /** Adjust the virtual length used for effects. */
    void setVirtualLength(uint32_t length) { m_virtualLength = length; }

    /** Set the current step (used for external synchronization). */
    void setStep(uint16_t step) { m_step = step; }
    uint16_t step() const { return m_step; }

    /**
     * @brief Advance the animation by one step.
     */
    void tick();

private:
    WS2812Strip &m_strip;
    bool m_initialized = false;
    uint32_t m_virtualLength = 0;
    Effect m_effect = Effect::Off;
    uint32_t m_color = 0xFFFFFF;
    uint16_t m_step = 0;
    int m_dir = 1;
    uint8_t m_brightness = 0;
};

inline void WS2812Animator::tick()
{
    if (!m_initialized) {
        if (m_strip.begin() == ESP_OK) {
            m_initialized = true;
        } else {
            return;
        }
    }

    uint32_t stripLen = m_strip.length();
    uint32_t virtLen = m_virtualLength ? m_virtualLength : stripLen;

    switch (m_effect) {
    case Effect::SolidColor:
        for (uint32_t i = 0; i < stripLen; ++i) {
            m_strip.setPixel(i, m_color);
        }
        m_strip.show();
        break;

    case Effect::Rainbow:
        for (uint32_t i = 0; i < stripLen; ++i) {
            uint32_t vIndex = (virtLen == stripLen) ? i : (i * virtLen / stripLen);
            uint8_t pos = static_cast<uint8_t>((vIndex * 256 / virtLen + m_step) & 0xFF);
            m_strip.setPixel(i, WS2812Strip::colorWheel(pos));
        }
        m_strip.show();
        m_step = static_cast<uint16_t>((m_step + 1) & 0xFF);
        break;

    case Effect::Chase:
        for (uint32_t i = 0; i < stripLen; ++i) {
            if (((i + m_step) % 3) == 0) {
                m_strip.setPixel(i, m_color);
            } else {
                m_strip.setPixel(i, 0);
            }
        }
        m_strip.show();
        m_step = static_cast<uint16_t>((m_step + 1) % 3);
        break;

    case Effect::Blink:
        for (uint32_t i = 0; i < stripLen; ++i) {
            m_strip.setPixel(i, (m_step & 1) ? m_color : 0);
        }
        m_strip.show();
        m_step++;
        break;

    case Effect::Breath:
        m_brightness = (m_step < 256) ? m_step : 511 - m_step;
        m_strip.setBrightness(m_brightness);
        for (uint32_t i = 0; i < stripLen; ++i) {
            m_strip.setPixel(i, m_color);
        }
        m_strip.show();
        m_step = static_cast<uint16_t>((m_step + 4) % 512);
        break;

    case Effect::Larson:
        for (uint32_t i = 0; i < stripLen; ++i) {
            m_strip.setPixel(i, (i == m_step) ? m_color : 0);
        }
        m_strip.show();
        if ((m_step == 0 && m_dir < 0) || (m_step == stripLen - 1 && m_dir > 0)) {
            m_dir = -m_dir;
        }
        m_step = static_cast<uint16_t>(m_step + m_dir);
        break;

    default:
        for (uint32_t i = 0; i < stripLen; ++i) {
            m_strip.setPixel(i, 0);
        }
        m_strip.show();
        break;
    }
}

#endif // __cplusplus

#endif // WS2812_EFFECTS_HPP
