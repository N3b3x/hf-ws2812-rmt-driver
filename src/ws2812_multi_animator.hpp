#ifndef WS2812_MULTI_ANIMATOR_HPP
#define WS2812_MULTI_ANIMATOR_HPP

#include "ws2812_effects.hpp"
#include <vector>

#ifdef __cplusplus
class WS2812MultiAnimator {
public:
    explicit WS2812MultiAnimator(const std::vector<WS2812Strip*>& strips,
                                 bool unified = true, bool sync = true)
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

    void setEffect(WS2812Animator::Effect eff, uint32_t color = 0xFFFFFF) {
        for (auto& a : m_animators) {
            a.setEffect(eff, color);
        }
    }

    void setEffect(size_t idx, WS2812Animator::Effect eff, uint32_t color = 0xFFFFFF) {
        if (idx < m_animators.size()) {
            m_animators[idx].setEffect(eff, color);
        }
    }

    void tick() {
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

private:
    std::vector<WS2812Animator> m_animators;
    bool m_sync = true;
    uint16_t m_step = 0;
};
#endif // __cplusplus

#endif // WS2812_MULTI_ANIMATOR_HPP
