#ifndef WS2812_CONTROL_H
#define WS2812_CONTROL_H
/**
 * @file ws2812_control.h
 * @brief ESP-IDF RMT based driver for WS2812 / NeoPixel LED chains.
 *
 * This component provides a small API for driving a string of
 * addressable RGB(W) LEDs using the ESP32's RMT peripheral.
 * All configuration options are exposed via Kconfig and the
 * functions declared here form the public interface for the driver.
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_types.h"
#include "led_strip_encoder.h"

/**
 * @name Driver constants
 * @{*/

/** Number of LEDs that will be driven by the component. */
#define NUM_LEDS CONFIG_WS2812_NUM_LEDS

#if CONFIG_WS2812_LED_TYPE_RGB
#define WS2812_BITS_PER_LED 24
#elif CONFIG_WS2812_LED_TYPE_RGBW
#define WS2812_BITS_PER_LED 32
#endif

/** Number of RMT items required to represent the LED chain. */
#define WS2812_LED_BUFFER_ITEMS (NUM_LEDS * WS2812_BITS_PER_LED)

/** Timing for a '0' bit high pulse in RMT ticks */
#define WS2812_T0H CONFIG_WS2812_T0H
/** Timing for a '1' bit high pulse in RMT ticks */
#define WS2812_T1H CONFIG_WS2812_T1H
/** Low time for any bit in RMT ticks */
#define WS2812_T0L CONFIG_WS2812_T0L
#define WS2812_T1L CONFIG_WS2812_T1L

/** Default global brightness scaling */
#define WS2812_DEFAULT_BRIGHTNESS CONFIG_WS2812_DEFAULT_BRIGHTNESS

/**@}*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Desired colors for a chain of LEDs.
 *
 * Each element of @ref leds describes one LED. The lower three bytes contain
 * the RGB value in the order R (byte 2), G (byte 1) and B (byte 0). When the
 * driver is configured for RGBW LEDs the most significant byte contains the
 * white channel value.
 */
struct led_state {
    /** Array of packed color values for each LED. */
    uint32_t leds[NUM_LEDS];
};

/**
 * @brief Initialise the RMT peripheral for WS2812 output.
 *
 * This function must be called once before any LED data is transmitted.
 * The GPIO and channel can be specified at runtime.
 *
 * @param gpio_num GPIO connected to the LED strip.
 * @param channel  RMT channel to use.
 * @return ESP_OK on success or an error code from the ESP-IDF drivers.
 */
esp_err_t ws2812ControlInit(gpio_num_t gpio_num, rmt_channel_t channel);

/**
 * @brief Send LED colour data.
 *
 * The function blocks the calling task until the complete buffer has been
 * transmitted by the RMT peripheral.
 *
 * @param new_state Desired LED colours.
 * @return ESP_OK on success or an ESP-IDF error code.
 */
esp_err_t ws2812WriteLeds(struct led_state new_state);

/**
 * @brief Set global brightness for subsequent transmissions.
 *
 * All LED values are scaled by this factor before being sent. 255 means
 * full brightness.
 *
 * @param brightness Brightness level 0-255.
 */
void ws2812SetBrightness(uint8_t brightness);

#ifdef __cplusplus
}
#endif

#endif // WS2812_CONTROL_H
