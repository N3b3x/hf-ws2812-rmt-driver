# HF-WS2812-RMT Documentation

Welcome! This directory contains documentation for the **HF-WS2812-RMT** driver library.

## Overview

**HF-WS2812** is a lightweight ESP-IDF component that provides precise timing for WS2812-compatible LED strips via the RMT peripheral. While compile-time defaults exist, the `WS2812Strip` C++ class lets you supply GPIO, channel, LED type and timing parameters dynamically. A minimal C API is provided, with an optional C++ wrapper for easier integration in modern applications.

## Features

- ✅ Accurate signal generation using RMT hardware
- ⚙️ Compile-time defaults with runtime override capability
- ✨ Optional `WS2812Strip` C++ class
- 🌈 Built-in effects with `WS2812Animator`
- 🎛️ Synchronised multi-strip animations with `WS2812MultiAnimator`
- 📏 Flexible strip lengths at runtime
- 🔧 All timings and LED type configurable at runtime using `WS2812Strip`
- 🧰 `RmtChannel` RAII helper for the RMT peripheral
- 👉 Simple API for updating entire LED chains
- 🔆 Global brightness control
- 📝 Doxygen documentation available

## Quick Start

```cpp
#include "ws2812_cpp.hpp"

// Create WS2812 strip instance
WS2812Strip strip(GPIO_NUM_8, 60);  // GPIO 8, 60 LEDs

// Initialize
if (strip.Initialize()) {
    // Set all LEDs to red
    for (int i = 0; i < 60; i++) {
        strip.SetPixel(i, 255, 0, 0);
    }
    strip.Update();
}
```

## Documentation Structure

- **API Reference**: See the main README.md for complete API documentation
- **Examples**: See `examples/esp32/` for ESP32 usage examples
- **Doxygen**: Full API documentation available via Doxygen

## Additional Resources

- [Main README](../README.md) - Complete driver documentation
- [Examples](../examples/) - Usage examples
- [Contributing](../CONTRIBUTING.md) - Contribution guidelines

