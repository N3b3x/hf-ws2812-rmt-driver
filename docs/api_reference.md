---
layout: default
title: "📖 API Reference"
description: "Complete API documentation for the WS2812 driver"
nav_order: 5
parent: "📚 Documentation"
permalink: /docs/api_reference/
---

# API Reference

Complete reference documentation for all public methods and types in the WS2812 driver.

## Source Code

- **C API Header**: [`inc/ws2812_control.h`](../inc/ws2812_control.h)
- **C API Implementation**: [`src/ws2812_control.c`](../src/ws2812_control.c)
- **C++ API Header**: [`inc/ws2812_cpp.hpp`](../inc/ws2812_cpp.hpp)
- **C++ API Implementation**: [`src/ws2812_cpp.cpp`](../src/ws2812_cpp.cpp)
- **Effects Header**: [`inc/ws2812_effects.hpp`](../inc/ws2812_effects.hpp)

## C API

### Functions

#### `ws2812ControlInit()`

Initialize the RMT peripheral for WS2812 output.

**Signature:**
```c
esp_err_t ws2812ControlInit(gpio_num_t gpio_num, int channel);
```

**Location**: [`src/ws2812_control.c#L59`](../src/ws2812_control.c#L59)

**Parameters:**
- `gpio_num`: GPIO pin connected to LED strip data line
- `channel`: RMT channel to use (0-7, or -1 for auto)

**Returns:**
- `ESP_OK` on success
- ESP-IDF error code on failure

**Example:**
```c
esp_err_t ret = ws2812ControlInit(GPIO_NUM_18, 0);
if (ret != ESP_OK) {
    // Handle error
}
```

---

#### `ws2812WriteLeds()`

Send LED color data to the strip.

**Signature:**
```c
esp_err_t ws2812WriteLeds(struct led_state new_state);
```

**Location**: [`src/ws2812_control.c#L91`](../src/ws2812_control.c#L91)

**Parameters:**
- `new_state`: `led_state` structure containing LED colors

**Returns:**
- `ESP_OK` on success
- ESP-IDF error code on failure

**Example:**
```c
struct led_state state = {0};
state.leds[0] = 0xFF0000; // Red
state.leds[1] = 0x00FF00; // Green
ws2812WriteLeds(state);
```

---

#### `ws2812SetBrightness()`

Set global brightness scaling.

**Signature:**
```c
void ws2812SetBrightness(uint8_t brightness);
```

**Location**: [`src/ws2812_control.c#L104`](../src/ws2812_control.c#L104)

**Parameters:**
- `brightness`: Brightness level (0-255, 255 = full brightness)

**Example:**
```c
ws2812SetBrightness(128); // 50% brightness
```

---

### Structures

#### `led_state`

LED color state structure.

**Location**: [`inc/ws2812_control.h#L62`](../inc/ws2812_control.h#L62)

```c
struct led_state {
    uint32_t leds[NUM_LEDS];  // Array of packed color values
};
```

Each `leds[i]` contains:
- **RGB**: Bytes 0-2 = B, G, R (little-endian)
- **RGBW**: Bytes 0-3 = B, G, R, W

---

## C++ API

### `WS2812Strip` Class

Main C++ class for controlling WS2812 LED strips.

**Location**: [`inc/ws2812_cpp.hpp#L31`](../inc/ws2812_cpp.hpp#L31)

#### Constructor

```cpp
WS2812Strip(gpio_num_t gpio, int channel, uint32_t numLeds, 
            LedType type, uint16_t t0h, uint16_t t1h, 
            uint16_t t0l, uint16_t t1l, uint8_t brightness);
```

**Location**: [`inc/ws2812_cpp.hpp#L39`](../inc/ws2812_cpp.hpp#L39)

#### Methods

##### `begin()`

Initialize the strip driver.

**Signature:**
```cpp
esp_err_t begin();
```

**Location**: [`inc/ws2812_cpp.hpp#L55`](../inc/ws2812_cpp.hpp#L55)

**Returns:**
- `ESP_OK` on success
- ESP-IDF error code on failure

---

##### `setPixel()`

Set the color of a single LED.

**Signature:**
```cpp
void setPixel(uint32_t index, uint32_t rgbw);
```

**Location**: [`inc/ws2812_cpp.hpp#L63`](../inc/ws2812_cpp.hpp#L63)

**Parameters:**
- `index`: LED index (0 to length-1)
- `rgbw`: Packed color value (24-bit RGB or 32-bit RGBW)

---

##### `show()`

Send buffered colors to the LED strip.

**Signature:**
```cpp
esp_err_t show();
```

**Location**: [`inc/ws2812_cpp.hpp#L71`](../inc/ws2812_cpp.hpp#L71)

**Returns:**
- `ESP_OK` on success
- ESP-IDF error code on failure

---

##### `setBrightness()`

Set global brightness.

**Signature:**
```cpp
void setBrightness(uint8_t value);
```

**Location**: [`inc/ws2812_cpp.hpp#L78`](../inc/ws2812_cpp.hpp#L78)

---

##### `setTimings()`

Update protocol timing parameters.

**Signature:**
```cpp
void setTimings(uint16_t t0h, uint16_t t1h, uint16_t t0l, uint16_t t1l);
```

**Location**: [`inc/ws2812_cpp.hpp#L85`](../inc/ws2812_cpp.hpp#L85)

---

##### `length()`

Get the number of LEDs.

**Signature:**
```cpp
uint32_t length() const;
```

**Location**: [`inc/ws2812_cpp.hpp#L66`](../inc/ws2812_cpp.hpp#L66)

---

##### `colorWheel()`

Generate color from color wheel position.

**Signature:**
```cpp
static uint32_t colorWheel(uint8_t pos);
```

**Location**: [`inc/ws2812_cpp.hpp#L93`](../inc/ws2812_cpp.hpp#L93)

**Parameters:**
- `pos`: Position on color wheel (0-255)

**Returns:**
- Packed RGB color value

---

### `WS2812Animator` Class

Animation effects helper class.

**Location**: [`inc/ws2812_effects.hpp#L14`](../inc/ws2812_effects.hpp#L14)

#### Effects

| Effect | Description |
|--------|-------------|
| `Off` | All LEDs off |
| `SolidColor` | Static color across strip |
| `Rainbow` | Moving rainbow pattern |
| `Chase` | Single-color chase effect |
| `Blink` | Blink entire strip |
| `Breath` | Smooth fade in/out |
| `Larson` | Scanning dot back and forth |

#### Methods

##### `setEffect()`

Set the active animation effect.

**Signature:**
```cpp
void setEffect(Effect effect, uint32_t color = 0xFFFFFF);
```

**Location**: [`inc/ws2812_effects.hpp#L26`](../inc/ws2812_effects.hpp#L26)

---

##### `tick()`

Advance the animation by one step.

**Signature:**
```cpp
void tick();
```

**Location**: [`inc/ws2812_effects.hpp#L38`](../inc/ws2812_effects.hpp#L38)

---

## Error Handling

The driver uses ESP-IDF's `esp_err_t` error codes:

- `ESP_OK`: Operation successful
- `ESP_ERR_INVALID_ARG`: Invalid parameter
- `ESP_ERR_NO_MEM`: Out of memory
- Other ESP-IDF error codes as applicable

## Thread Safety

The driver is **not thread-safe**. If used in a multi-threaded environment:
- Each `WS2812Strip` instance should be used by a single thread
- Use external synchronization (mutex, etc.) for shared access
- RMT peripheral access is handled internally but strip state is not protected

---

**Navigation**
⬅️ [Configuration](configuration.md) | [Next: Examples ➡️](examples.md) | [Back to Index](index.md)

