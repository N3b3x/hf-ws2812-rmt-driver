# =============================================================================
# hf_ws2812_rmt — ESP-IDF component wrapper
# =============================================================================
# This thin wrapper imports the shared build-settings file and registers the
# component for ESP-IDF.  All sources, includes, and version info are defined
# in cmake/hf_ws2812_rmt_build_settings.cmake (single source of truth).
# =============================================================================

# ── Locate driver root ───────────────────────────────────────────────────────
set(HF_WS2812_RMT_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." CACHE PATH
    "Path to root of hf-ws2812-rmt-driver (auto-detected from component location)")

get_filename_component(HF_WS2812_RMT_ROOT "${HF_WS2812_RMT_ROOT}" ABSOLUTE)
if(NOT EXISTS "${HF_WS2812_RMT_ROOT}/cmake/hf_ws2812_rmt_build_settings.cmake")
    message(FATAL_ERROR
        "Cannot find WS2812 RMT driver root.\n"
        "  Resolved path: ${HF_WS2812_RMT_ROOT}\n"
        "  Expected file: ${HF_WS2812_RMT_ROOT}/cmake/hf_ws2812_rmt_build_settings.cmake\n"
        "  Fix: set HF_WS2812_RMT_ROOT to the correct driver checkout path.")
endif()

# ── Import build settings ────────────────────────────────────────────────────
include("${HF_WS2812_RMT_ROOT}/cmake/hf_ws2812_rmt_build_settings.cmake")

# ── Register ESP-IDF component ───────────────────────────────────────────────
idf_component_register(
    SRCS         ${HF_WS2812_RMT_SOURCE_FILES}
    INCLUDE_DIRS ${HF_WS2812_RMT_PUBLIC_INCLUDE_DIRS}
    REQUIRES     ${HF_WS2812_RMT_IDF_REQUIRES}
)

target_compile_options(${COMPONENT_LIB} PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++20>)
