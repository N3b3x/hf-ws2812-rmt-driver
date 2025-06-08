#include "ws2812_effects.hpp"

#ifdef __cplusplus

WS2812Animator::WS2812Animator(WS2812Strip &strip, uint32_t virtualLength)
    : m_strip(strip), m_virtualLength(virtualLength) {}

void WS2812Animator::setEffect(Effect effect, uint32_t color)
{
    m_effect = effect;
    m_color = color;
    m_step = 0;
    m_dir = 1;
    m_brightness = 0;
}

void WS2812Animator::setVirtualLength(uint32_t length)
{
    m_virtualLength = length;
}

void WS2812Animator::setStep(uint16_t step)
{
    m_step = step;
}

uint16_t WS2812Animator::step() const
{
    return m_step;
}

void WS2812Animator::tick()
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
