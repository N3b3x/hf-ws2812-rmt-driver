# =============================================================================
# hf_ws2812_rmt_build_settings.cmake — Single source of truth
# =============================================================================
# This file defines ALL build settings for the WS2812 RMT driver.
# It is consumed by:
#   1. The root CMakeLists.txt   (ESP Component Registry & desktop builds)
#   2. The ESP-IDF component wrapper (examples/esp32/components/…/CMakeLists.txt)
#
# Prerequisites:
#   HF_WS2812_RMT_ROOT must be set to the driver's repository root before
#   including this file.
# =============================================================================
cmake_minimum_required(VERSION 3.16)

# ── Guard ────────────────────────────────────────────────────────────────────
if(_HF_WS2812_RMT_BUILD_SETTINGS_INCLUDED)
  return()
endif()
set(_HF_WS2812_RMT_BUILD_SETTINGS_INCLUDED TRUE)

# ── Root validation ──────────────────────────────────────────────────────────
if(NOT DEFINED HF_WS2812_RMT_ROOT)
  message(FATAL_ERROR "HF_WS2812_RMT_ROOT must be set before including "
                      "hf_ws2812_rmt_build_settings.cmake")
endif()

# ── Version ──────────────────────────────────────────────────────────────────
set(HF_WS2812_RMT_VERSION_MAJOR 1)
set(HF_WS2812_RMT_VERSION_MINOR 0)
set(HF_WS2812_RMT_VERSION_PATCH 0)
set(HF_WS2812_RMT_VERSION_STRING
    "${HF_WS2812_RMT_VERSION_MAJOR}.${HF_WS2812_RMT_VERSION_MINOR}.${HF_WS2812_RMT_VERSION_PATCH}")

# ── Generate version header ─────────────────────────────────────────────────
configure_file(
  "${HF_WS2812_RMT_ROOT}/inc/ws2812_version.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/generated/ws2812_version.h"
  @ONLY
)

# ── Source files (compiled — mixed C / C++) ──────────────────────────────────
set(HF_WS2812_RMT_SOURCES
    "${HF_WS2812_RMT_ROOT}/src/ws2812_control.c"
    "${HF_WS2812_RMT_ROOT}/src/ws2812_cpp.cpp"
    "${HF_WS2812_RMT_ROOT}/src/ws2812_effects.cpp"
    "${HF_WS2812_RMT_ROOT}/src/ws2812_multi_animator.cpp"
    "${HF_WS2812_RMT_ROOT}/src/led_strip_encoder.c"
)

# ── Public include directories ───────────────────────────────────────────────
set(HF_WS2812_RMT_INCLUDE_DIRS
    "${HF_WS2812_RMT_ROOT}/inc"
    "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

# ── ESP-IDF component dependencies ──────────────────────────────────────────
set(HF_WS2812_RMT_IDF_REQUIRES
    driver
    freertos
    esp_driver_rmt
)

message(STATUS "[hf_ws2812_rmt] v${HF_WS2812_RMT_VERSION_STRING} — "
               "${HF_WS2812_RMT_ROOT}")
