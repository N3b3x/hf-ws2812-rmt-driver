/**
 * @file ws2812_multi_animator.cpp
 * @brief Multi-strip animation coordinator implementation
 * @copyright Copyright (c) 2024-2025 HardFOC. All rights reserved.
 */

#include "ws2812_multi_animator.hpp"

#ifdef __cplusplus

WS2812MultiAnimator::WS2812MultiAnimator(const std::vector<WS2812Strip*>& strips, bool unified,
                                         bool sync)
    : m_sync(sync) {
  uint32_t maxLen = 0;
  for (auto* s : strips) {
    if (s && s->Length() > maxLen) {
      maxLen = s->Length();
    }
  }
  for (auto* s : strips) {
    if (!s)
      continue;
    m_animators.emplace_back(*s, unified ? maxLen : 0);
  }
}

void WS2812MultiAnimator::SetEffect(WS2812Animator::Effect eff, uint32_t color) {
  for (auto& a : m_animators) {
    a.SetEffect(eff, color);
  }
}

void WS2812MultiAnimator::SetEffect(size_t idx, WS2812Animator::Effect eff, uint32_t color) {
  if (idx < m_animators.size()) {
    m_animators[idx].SetEffect(eff, color);
  }
}

void WS2812MultiAnimator::Tick() {
  if (m_sync) {
    for (auto& a : m_animators) {
      a.SetStep(m_step);
    }
  }
  for (auto& a : m_animators) {
    a.Tick();
  }
  if (m_sync && !m_animators.empty()) {
    m_step = m_animators.front().Step();
  }
}

#endif // __cplusplus
