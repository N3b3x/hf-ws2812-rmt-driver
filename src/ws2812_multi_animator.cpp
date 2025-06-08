#include "ws2812_multi_animator.hpp"

#ifdef __cplusplus

WS2812MultiAnimator::WS2812MultiAnimator(const std::vector<WS2812Strip*>& strips,
                                         bool unified,
                                         bool sync)
    : m_sync(sync)
{
    uint32_t maxLen = 0;
    for (auto* s : strips) {
        if (s && s->length() > maxLen) {
            maxLen = s->length();
        }
    }
    for (auto* s : strips) {
        if (!s) continue;
        m_animators.emplace_back(*s, unified ? maxLen : 0);
    }
}

void WS2812MultiAnimator::setEffect(WS2812Animator::Effect eff, uint32_t color)
{
    for (auto& a : m_animators) {
        a.setEffect(eff, color);
    }
}

void WS2812MultiAnimator::setEffect(size_t idx, WS2812Animator::Effect eff, uint32_t color)
{
    if (idx < m_animators.size()) {
        m_animators[idx].setEffect(eff, color);
    }
}

void WS2812MultiAnimator::tick()
{
    if (m_sync) {
        for (auto& a : m_animators) {
            a.setStep(m_step);
        }
    }
    for (auto& a : m_animators) {
        a.tick();
    }
    if (m_sync && !m_animators.empty()) {
        m_step = m_animators.front().step();
    }
}

#endif // __cplusplus
