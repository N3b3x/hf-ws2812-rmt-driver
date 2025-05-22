# HF-WS2812  
**ESP32 RMT based WS2812/NeoPixel Driver — used in the HardFOC-V1 controller**

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

---

## 📦 Overview

**HF-WS2812** is a lightweight ESP-IDF component that provides precise timing for WS2812-compatible LED strips via the RMT peripheral. All parameters—including the number of LEDs, timings, and GPIO—are configurable through Kconfig. A minimal C API is provided, with an optional C++ wrapper for easier integration in modern applications.

---

## 🚀 Features

- ✅ Accurate signal generation using RMT hardware
- ⚙️ Fully configurable (timings, RGB/RGBW format, GPIO)
- ✨ Optional `WS2812Strip` C++ class
- 👉 Simple API for updating entire LED chains
- 📝 Doxygen documentation available

---

## 📂 Project Structure

```
├── src/
│   ├── ws2812_control.c        # Core driver implementation
│   ├── ws2812_control.h        # C API header
│   └── ws2812_cpp.hpp          # Optional C++ wrapper
├── CMakeLists.txt              # Component build configuration
├── component.mk                # Legacy build support
├── Kconfig                     # Configuration options
├── Doxyfile                    # Doxygen config
└── README.md                   # Project documentation
```

---

## 🔧 Installation

1. Copy this component into your project's `components/` directory.
2. Run `idf.py menuconfig` and configure **HF-ESP32-WS2812-RMT** under Component Config.
3. In your code, include the driver header:
   ```c
   #include "ws2812_control.h"
   ```
4. For C++ projects, include:
   ```cpp
   #include "ws2812_cpp.hpp"
   ```

---

## 🧠 Quick Start Example

```cpp
#include "ws2812_cpp.hpp"

WS2812Strip strip;  // Uses NUM_LEDS from Kconfig

void app_main(void)
{
    strip.begin();
    for (uint8_t i = 0; i < NUM_LEDS; ++i) {
        strip.setPixel(i, WS2812Strip::colorWheel(i * (256 / NUM_LEDS)));
    }
    strip.show();
}
```

---

## 📰 API Summary

| Function                                  | Description                                 |
|-------------------------------------------|---------------------------------------------|
| `esp_err_t ws2812ControlInit()`         | Initialize RMT hardware and driver          |
| `esp_err_t ws2812WriteLeds(...)`        | Send color values to the LED chain          |
| `WS2812Strip::begin()`                    | Initialize the C++ driver wrapper           |
| `WS2812Strip::setPixel(index, color)`     | Set individual LED color                    |
| `WS2812Strip::show()`                     | Transmit buffered colors to the LED chain   |
| `WS2812Strip::colorWheel(pos)`            | Generate a rainbow-style color              |

---

## ⏱️ Timing Details

This driver is tuned according to timing specifications from [SparkFun’s datasheet](https://cdn.sparkfun.com/datasheets/Components/LED/COM-12877.pdf).

### Typical LED Timings (per datasheet):

#### WS2811 (400 Kbps, 2.5 μs bit time)
```
T0H: 0.5 μs  ← 0 bit
T0L: 2.0 μs
T1H: 1.2 μs  ← 1 bit
T1L: 1.3 μs
RES: 50 μs
```

#### WS2812 (800 Kbps, 1.25 μs bit time)
```
T0H: 0.35 μs ← 0 bit
T0L: 0.8 μs
T1H: 0.7 μs  ← 1 bit
T1L: 0.6 μs
RES: 50 μs
```

#### WS2812B (800 Kbps, 1.25 μs bit time)
```
T0H: 0.4 μs  ← 0 bit
T0L: 0.85 μs
T1H: 0.8 μs  ← 1 bit
T1L: 0.45 μs
RES: 50 μs
```

⚠️ **If Wi-Fi interferes with timing**, pin the task to Core 1 using `xTaskCreatePinnedToCore()`.

---

## 📜 License

This project is licensed under the **GNU General Public License v3.0**.  
See the [LICENSE](./LICENSE) file for more information.

---

## 🤝 Contributing

Pull requests and suggestions are welcome!  
To contribute:

1. Fork the repository  
2. Create a new branch for your feature/fix  
3. Commit your changes  
4. Open a pull request and describe your contribution  
