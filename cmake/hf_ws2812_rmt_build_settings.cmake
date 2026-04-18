#===============================================================================
# WS2812 RMT Driver - Build Settings
# Shared variables for target name, includes, sources, and dependencies.
# This file is the SINGLE SOURCE OF TRUTH for the driver version.
#===============================================================================

include_guard(GLOBAL)

set(HF_WS2812_RMT_TARGET_NAME "hf_ws2812_rmt")

#===============================================================================
# Versioning (single source of truth)
#===============================================================================
set(HF_WS2812_RMT_VERSION_MAJOR 1)
set(HF_WS2812_RMT_VERSION_MINOR 0)
set(HF_WS2812_RMT_VERSION_PATCH 0)
set(HF_WS2812_RMT_VERSION "${HF_WS2812_RMT_VERSION_MAJOR}.${HF_WS2812_RMT_VERSION_MINOR}.${HF_WS2812_RMT_VERSION_PATCH}")

#===============================================================================
# Generate version header from template (into build directory)
#===============================================================================
set(HF_WS2812_RMT_VERSION_TEMPLATE "${CMAKE_CURRENT_LIST_DIR}/../inc/ws2812_version.h.in")
set(HF_WS2812_RMT_VERSION_HEADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/hf_ws2812_rmt_generated")
set(HF_WS2812_RMT_VERSION_HEADER     "${HF_WS2812_RMT_VERSION_HEADER_DIR}/ws2812_version.h")

file(MAKE_DIRECTORY "${HF_WS2812_RMT_VERSION_HEADER_DIR}")

if(EXISTS "${HF_WS2812_RMT_VERSION_TEMPLATE}")
    configure_file(
        "${HF_WS2812_RMT_VERSION_TEMPLATE}"
        "${HF_WS2812_RMT_VERSION_HEADER}"
        @ONLY
    )
    message(STATUS "WS2812 RMT driver v${HF_WS2812_RMT_VERSION} — generated ws2812_version.h in ${HF_WS2812_RMT_VERSION_HEADER_DIR}")
else()
    message(WARNING "ws2812_version.h.in not found at ${HF_WS2812_RMT_VERSION_TEMPLATE}")
endif()

#===============================================================================
# Public include directories
#===============================================================================
set(HF_WS2812_RMT_PUBLIC_INCLUDE_DIRS
    "${CMAKE_CURRENT_LIST_DIR}/../inc"
    "${HF_WS2812_RMT_VERSION_HEADER_DIR}"
)

#===============================================================================
# Source files (mixed C/C++)
#===============================================================================
set(HF_WS2812_RMT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/../src/ws2812_control.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ws2812_cpp.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ws2812_effects.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ws2812_multi_animator.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../src/led_strip_encoder.c"
)

#===============================================================================
# ESP-IDF component dependencies
#===============================================================================
set(HF_WS2812_RMT_IDF_REQUIRES driver freertos esp_driver_rmt)
