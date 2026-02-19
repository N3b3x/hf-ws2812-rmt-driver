# =============================================================================
# hf_ws2812_rmt — ESP-IDF component wrapper
# =============================================================================
# This thin wrapper imports the shared build-settings file and registers the
# component for ESP-IDF.  All sources, includes, and version info are defined
# in cmake/hf_ws2812_rmt_build_settings.cmake (single source of truth).
# =============================================================================

# ── Locate driver root ───────────────────────────────────────────────────────
set(HF_WS2812_RMT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." CACHE PATH
    "Root of the hf-ws2812-rmt-driver repository")

if(NOT EXISTS "${HF_WS2812_RMT_ROOT}/cmake/hf_ws2812_rmt_build_settings.cmake")
  message(FATAL_ERROR
    "Cannot find hf_ws2812_rmt_build_settings.cmake.\n"
    "  HF_WS2812_RMT_ROOT = ${HF_WS2812_RMT_ROOT}")
endif()

# ── Import build settings ────────────────────────────────────────────────────
include("${HF_WS2812_RMT_ROOT}/cmake/hf_ws2812_rmt_build_settings.cmake")

# ── Register ESP-IDF component ───────────────────────────────────────────────
idf_component_register(
    SRCS        ${HF_WS2812_RMT_SOURCES}
    INCLUDE_DIRS ${HF_WS2812_RMT_INCLUDE_DIRS}
    REQUIRES     ${HF_WS2812_RMT_IDF_REQUIRES}
)

target_compile_options(${COMPONENT_LIB} PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++20>)
