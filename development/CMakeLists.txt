# =============================================================================
# hf-ws2812-rmt-driver — Root CMakeLists.txt
# =============================================================================
# Dual-mode build file:
#   - ESP-IDF component (when ESP_PLATFORM is set, e.g. via idf_component.yml)
#   - Platform-agnostic static library (desktop / CI)
#
# Both paths import cmake/hf_ws2812_rmt_build_settings.cmake as the single
# source of truth for sources, includes, and version info.
# =============================================================================
cmake_minimum_required(VERSION 3.16)

set(HF_WS2812_RMT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH
    "Root of the hf-ws2812-rmt-driver repository")

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/hf_ws2812_rmt_build_settings.cmake")

# ── ESP-IDF path (used when installed via ESP Component Registry) ────────────
if(ESP_PLATFORM)
  idf_component_register(
      SRCS         ${HF_WS2812_RMT_SOURCES}
      INCLUDE_DIRS ${HF_WS2812_RMT_INCLUDE_DIRS}
      REQUIRES     ${HF_WS2812_RMT_IDF_REQUIRES}
  )
  target_compile_options(${COMPONENT_LIB} PRIVATE
      $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++20>)
  return()
endif()

# ── Platform-agnostic path ───────────────────────────────────────────────────
project(hf_ws2812_rmt
  VERSION   1.0.0
  LANGUAGES C CXX
)

add_library(hf_ws2812_rmt STATIC ${HF_WS2812_RMT_SOURCES})

target_include_directories(hf_ws2812_rmt PUBLIC ${HF_WS2812_RMT_INCLUDE_DIRS})

target_compile_features(hf_ws2812_rmt PUBLIC cxx_std_20)

# Optional warnings
option(HF_WS2812_RMT_ENABLE_WARNINGS "Enable compiler warnings" OFF)
if(HF_WS2812_RMT_ENABLE_WARNINGS)
  target_compile_options(hf_ws2812_rmt PRIVATE -Wall -Wextra -Wpedantic)
endif()

# CMake package export
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/hf_ws2812_rmtConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/hf_ws2812_rmtConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/hf_ws2812_rmtConfig.cmake"
  INSTALL_DESTINATION lib/cmake/hf_ws2812_rmt
)
