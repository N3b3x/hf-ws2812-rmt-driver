# HF-WS2812
Hardware Agnostic WS2812/NeoPixel driver - as used in the HardFOC-V1 controller

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

## \ud83d\udcc6 Overview
**HF-WS2812** is a lightweight ESP-IDF component that generates the precise timing required by WS2812 compatible LED strips via the RMT peripheral. All parameters\u2014number of LEDs, timings and GPIO\u2014are configurable through Kconfig. A small C API is provided along with an optional C++ wrapper for easier use in modern applications.

---

## \ud83d\ude80 Features
- \u2705 Accurate signal generation with the RMT hardware
- \u2699\ufe0f Configurable timings, LED type (RGB or RGBW) and GPIO
- \u2728 Optional `WS2812Strip` C++ class
- \ud83d\udd1c Simple API to update the entire LED chain
- \ud83d\udcdd Doxygen documentation

---

## \ud83d\udcc2 Project Structure
```text
├── src/
│   ├── ws2812_control.c        # Core driver implementation
│   ├── ws2812_control.h        # C API
│   └── ws2812_cpp.hpp          # Optional C++ wrapper
├── CMakeLists.txt              # Component build file
├── component.mk                # Legacy build support
├── Kconfig                     # Configuration options
├── Doxyfile                    # Doxygen configuration
└── README.md                   # Project documentation
```

---

## \ud83d\udd27 Installation
1. Copy this component into your project's `components` directory.
2. Run `idf.py menuconfig` and configure **HF-ESP32-WS2812-RMT**.
3. Include the header in your application:
   ```c
   #include "ws2812_control.h"
   ```
4. For C++ projects you may include `ws2812_cpp.hpp` to use the convenience class.

---

## \ud83e\udde0 Quick Start
```c
#include "ws2812_cpp.hpp"

WS2812Strip strip;                // Uses NUM_LEDS from Kconfig

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

## \ud83d\udcf0 API Summary
| Function | Description |
|----------|-------------|
| `esp_err_t ws2812_control_init()` | Initialise RMT hardware and driver |
| `esp_err_t ws2812_write_leds(struct led_state)` | Send colour values to the LED chain |
| `WS2812Strip::begin()` | Initialise driver (C++ wrapper) |
| `WS2812Strip::setPixel(index, color)` | Set individual LED colour |
| `WS2812Strip::show()` | Transmit buffered colours |
| `WS2812Strip::colorWheel(pos)` | Generate rainbow colour |

---

## \u23f1\ufe0f Timing
This driver is tuned according to the timing information from [SparkFun's datasheet](https://cdn.sparkfun.com/datasheets/Components/LED/COM-12877.pdf).

#### LED TIMINGS, per their datasheets:

##### WS2811: (2.5us bit time, 400Kbps)
```
T0H: 0.5us  \u2190 0 bit
T0L: 2.0us
T1H: 1.2us  \u2190 1 bit
T1L: 1.3us
RES: 50us
```
##### WS2812: (1.25us bit time, 800Kbps)
```
T0H: 0.35us \u2190 0 bit
T0L: 0.8us
T1H: 0.7us  \u2190 1 bit
T1L: 0.6us
RES: 50us
```
##### WS2812b: (1.25us bit time, 800Kbps)
```
T0H: 0.4us  \u2190 0 bit
T0L: 0.85us
T1H: 0.8us  \u2190 1 bit
T1L: 0.45us
RES: 50us
```

If WiFi interrupts timing, create the driver task on core 1 with `xTaskCreatePinnedToCore`.

---

## \ud83d\udcdd License
This project is licensed under the **GNU General Public License v3.0**. See [LICENSE](./LICENSE) for details.

---

## \ud83e\udd1d Contributing
Pull requests and suggestions are welcome. Fork the repo, create your feature branch, commit your changes and open a PR.

