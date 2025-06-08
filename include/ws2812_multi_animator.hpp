#ifndef WS2812_MULTI_ANIMATOR_HPP
#define WS2812_MULTI_ANIMATOR_HPP

#include "ws2812_effects.hpp"
#include <vector>

#ifdef __cplusplus
/**
 * @file ws2812_multi_animator.hpp
 * @brief Helper coordinating animations across multiple LED strips.
 */
class WS2812MultiAnimator {
public:
    /**
     * @brief Construct from a list of strips.
     *
     * @param strips   Pointers to WS2812Strip objects.
     * @param unified  If true all strips share the longest length.
     * @param sync     If true keep animation steps in sync.
     */
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

    /**
     * @brief Apply an effect to all managed strips.
     */
    void setEffect(WS2812Animator::Effect eff, uint32_t color = 0xFFFFFF) {
        for (auto& a : m_animators) {
            a.setEffect(eff, color);
        }
    }

    /**
     * @brief Apply an effect to a single strip.
     *
     * @param idx   Index of the strip.
     * @param eff   Desired animation effect.
     * @param color Base colour for the effect.
     */
    void setEffect(size_t idx, WS2812Animator::Effect eff, uint32_t color = 0xFFFFFF) {
        if (idx < m_animators.size()) {
            m_animators[idx].setEffect(eff, color);
        }
    }

    /**
     * @brief Advance all animations by one step.
     */
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
    /** Animators for each strip. */
    std::vector<WS2812Animator> m_animators;
    /** Synchronise step counters across strips. */
    bool m_sync = true;
    /** Current animation step used for synchronisation. */
    uint16_t m_step = 0;
};
#endif // __cplusplus

#endif // WS2812_MULTI_ANIMATOR_HPP
