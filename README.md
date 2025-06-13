# HF-WS2812  
**ESP32 RMT based WS2812/NeoPixel Driver — used in the HardFOC-V1 controller**

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

---

## 📦 Overview

**HF-WS2812** is a lightweight ESP-IDF component that provides precise timing for WS2812-compatible LED strips via the RMT peripheral. While compile-time defaults exist, the `WS2812Strip` C++ class lets you supply GPIO, channel, LED type and timing parameters dynamically. A minimal C API is provided, with an optional C++ wrapper for easier integration in modern applications.

---

## 🚀 Features

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

## 🏗️ Building

1. Ensure the [ESP-IDF](https://github.com/espressif/esp-idf) is installed and `IDF_PATH` is set.
2. Install Doxygen if you wish to generate the reference documentation. On Ubuntu run:
   ```bash
   sudo apt-get install doxygen
   ```
3. From your application project run:
   ```bash
   idf.py set-target esp32  # adjust for your target
   idf.py build
   ```
4. Generate the API reference with:
   ```bash
   doxygen Doxyfile
   ```
   The HTML output can be found under `docs/html/index.html`.

---

## 🧠 Quick Start Example

`WS2812Strip` accepts the output pin, RMT channel, LED count and a `LedType` to
select either 24‑bit RGB or 32‑bit RGBW. Optional custom timings may be applied
after construction.

```cpp
#include "ws2812_cpp.hpp"
#include "ws2812_effects.hpp"

// GPIO, RMT channel, number of LEDs and more can be set here
WS2812Strip strip(GPIO_NUM_18, 0, 30, LedType::RGB); // runtime config
// strip.setTimings(custom_t0h, custom_t1h, custom_t0l, custom_t1l); // optional
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
    WS2812Strip strip(GPIO_NUM_18, 0, 30, LedType::RGB);
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
RmtChannel rmt(GPIO_NUM_18, 0);

void app_main(void)
{
    rmt.begin();
    // transmit your own rmt_symbol_word_t sequences
}
```

### Virtual Length Example

```cpp
WS2812Strip strip(GPIO_NUM_18, 0, 30, LedType::RGB);
WS2812Animator anim(strip, 60); // animate as if 60 LEDs are connected

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

### Custom Timings Example

By default the driver uses timings from Kconfig. You can tweak these at runtime
to fine‑tune compatibility with different LED variants. The values are specified
in **RMT ticks** (1 tick ≈ 50 ns when `clk_div` is 2).

```cpp
WS2812Strip strip(GPIO_NUM_18, 0, 30, LedType::RGB);
strip.setTimings(14, 52, 52, 52); // t0h, t1h, t0l, t1l
strip.begin();
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

### C Functions

| Function | Description |
|----------|-------------|
| `esp_err_t ws2812ControlInit(gpio, channel)` | Initialize RMT hardware for the selected pin |
| `esp_err_t ws2812WriteLeds(new_state)` | Send color values to the LED chain |
| `void ws2812SetBrightness(value)` | Set global brightness scaling |

### C++ Classes

#### RmtChannel
| Method | Description |
|--------|-------------|
| `begin()` | Configure an RMT channel for output |
| `transmit(items, len)` | Write raw RMT items and wait for finish |
| `end()` | Uninstall the driver |

#### WS2812Strip
| Method | Description |
|--------|-------------|
| `begin()` | Initialize the C++ driver wrapper |
| `setPixel(index, color)` | Set individual LED color |
| `show()` | Transmit buffered colors to the LED chain |
| `setBrightness(value)` | Set brightness from C++ wrapper |
| `setTimings(t0h, t1h, t0l, t1l)` | Adjust protocol timings |
| `length()` | Get the number of LEDs |
| `colorWheel(pos)` | Convert color wheel position to RGB |

#### WS2812Animator
| Method | Description |
|--------|-------------|
| `setEffect(type, color)` | Change the active animation effect |
| `setVirtualLength(value)` | Set a custom animation length |
| `tick()` | Advance the animation step |

#### WS2812MultiAnimator
| Method | Description |
|--------|-------------|
| `setEffect(eff, color)` | Apply an effect to all strips |
| `setEffect(idx, eff, color)` | Apply an effect to one strip |
| `tick()` | Advance all animations in sync |

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
