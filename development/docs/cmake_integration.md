# CMake Integration — WS2812 RMT Driver


## Quick start (ESP-IDF)

The ESP-IDF component wrapper lives at:

```
examples/esp32/components/hf_ws2812_rmt/CMakeLists.txt
```

It imports `cmake/hf_ws2812_rmt_build_settings.cmake`, which is the single
source of truth for sources, includes, and version information.

### Dual-mode root CMakeLists.txt

This driver also supports installation via the **ESP Component Registry**
(`idf_component.yml`). When `ESP_PLATFORM` is defined by ESP-IDF, the root
`CMakeLists.txt` registers the component directly. Otherwise it builds
a platform-agnostic static library.

## Layer overview

| Layer | File | Role |
|-------|------|------|
| 1 — Settings | `cmake/hf_ws2812_rmt_build_settings.cmake` | Sources, includes, version |
| 2 — Root | `CMakeLists.txt` | Dual-mode: IDF component *or* desktop library |
| 3 — ESP-IDF | `examples/esp32/components/…/CMakeLists.txt` | IDF wrapper for examples |

## Version

The version is defined once in `hf_ws2812_rmt_build_settings.cmake` and
stamped into `inc/ws2812_version.h.in` → `ws2812_version.h` via
`configure_file()`.

Access at runtime:

```cpp
WS2812Strip::GetDriverVersion();      // "1.0.0"
WS2812Strip::GetDriverVersionMajor(); // 1
GetWS2812DriverVersion();             // "1.0.0" (free function)
```
