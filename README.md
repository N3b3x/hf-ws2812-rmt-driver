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
- 🌈 Built-in effects with `WS2812Animator`
- 🎛️ Synchronised multi-strip animations with `WS2812MultiAnimator`
- 📏 Flexible strip lengths at runtime
- 🧰 `RmtChannel` RAII helper for the RMT peripheral
- 👉 Simple API for updating entire LED chains
- 🔆 Global brightness control
- 📝 Doxygen documentation available

---

## 📂 Project Structure

```
├── src/
│   ├── ws2812_control.c        # Core driver implementation
│   ├── ws2812_control.h        # C API header
│   ├── ws2812_cpp.hpp          # Optional C++ wrapper
│   ├── ws2812_effects.hpp      # Basic animation helper
│   └── ws2812_multi_animator.hpp # Multi-strip animator
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
5. To use built-in animations, also include:
   ```cpp
   #include "ws2812_effects.hpp"
   ```
6. For synchronized multi-strip control, include:
   ```cpp
   #include "ws2812_multi_animator.hpp"
   ```

---

## 🧠 Quick Start Example

```cpp
#include "ws2812_cpp.hpp"
#include "ws2812_effects.hpp"

WS2812Strip strip(GPIO_NUM_18); // runtime pin selection
WS2812Animator anim(strip);

void app_main(void)
{
    strip.begin();
    anim.setEffect(WS2812Animator::Effect::Rainbow);
    while (true) {
        anim.tick();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
```

### FreeRTOS Task Example

```cpp
static void ledTask(void *)
{
    WS2812Strip strip(GPIO_NUM_18);
    WS2812Animator anim(strip);
    strip.begin();
    anim.setEffect(WS2812Animator::Effect::Breath, 0x00FF00);
    for (;;) {
        anim.tick();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void app_main(void)
{
    xTaskCreate(ledTask, "led", 2048, nullptr, 5, nullptr);
} 
```

### Low-level RMT Example

```cpp
RmtChannel rmt(GPIO_NUM_18, RMT_CHANNEL_0);

void app_main(void)
{
    rmt.begin();
    // transmit your own rmt_item32_t sequences
}
```

## 🎨 Animation Effects

| Effect | Description |
|--------------|---------------------------------------|
| `Off` | All LEDs off |
| `SolidColor` | Static color across the strip |
| `Rainbow` | Moving rainbow pattern |
| `Chase` | Single-color chase effect |
| `Blink` | Blink entire strip on and off |
| `Breath` | Smooth fade in/out using brightness |
| `Larson` | Scanning dot back and forth |

---

## 📰 API Summary

| Function                                  | Description                                 |
|-------------------------------------------|---------------------------------------------|
| `esp_err_t ws2812ControlInit(gpio, channel)` | Initialize RMT hardware with selected pin |
| `esp_err_t ws2812WriteLeds(...)`        | Send color values to the LED chain          |
| `RmtChannel::begin()`                   | Configure an RMT channel for output |
| `RmtChannel::transmit(items, len)`      | Write raw RMT items and wait for finish |
| `WS2812Strip::begin()`                    | Initialize the C++ driver wrapper           |
| `WS2812Strip::setPixel(index, color)`     | Set individual LED color                    |
| `WS2812Strip::show()`                     | Transmit buffered colors to the LED chain   |
| `ws2812SetBrightness(value)`              | Set global brightness (0-255)     |
| `WS2812Strip::setBrightness(value)`       | Set brightness from C++ wrapper   |
| `WS2812Animator`                          | Helper class for simple animations      |
| `WS2812Animator::setEffect(type, color)`  | Change the active animation effect      |

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
