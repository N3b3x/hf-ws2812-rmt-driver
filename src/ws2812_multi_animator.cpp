/**
 * @file ws2812_multi_animator.cpp
 * @brief Multi-strip animation coordinator implementation
 * @copyright Copyright (c) 2024-2025 HardFOC. All rights reserved.
 */

#include "ws2812_multi_animator.hpp"

#ifdef __cplusplus

WS2812MultiAnimator::WS2812MultiAnimator(const std::vector<WS2812Strip*>& strips, bool unified,
                                         bool sync)
    : sync_(sync) {
  uint32_t max_len = 0;
  for (auto* s : strips) {
    if (s && s->Length() > max_len) {
      max_len = s->Length();
    }
  }
  for (auto* s : strips) {
    if (!s)
      continue;
    animators_.emplace_back(*s, unified ? max_len : 0);
  }
}

void WS2812MultiAnimator::SetEffect(WS2812Animator::Effect effect, uint32_t color) {
  for (auto& a : animators_) {
    a.SetEffect(effect, color);
  }
}

void WS2812MultiAnimator::SetEffect(size_t index, WS2812Animator::Effect effect, uint32_t color) {
  if (index < animators_.size()) {
    animators_[index].SetEffect(effect, color);
  }
}

void WS2812MultiAnimator::Tick() {
  if (sync_) {
    for (auto& a : animators_) {
      a.SetStep(step_);
    }
  }
  for (auto& a : animators_) {
    a.Tick();
  }
  if (sync_ && !animators_.empty()) {
    step_ = animators_.front().Step();
  }
}

#endif // __cplusplus
