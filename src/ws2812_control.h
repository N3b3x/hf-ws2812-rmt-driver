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

/** Number of LEDs that will be driven by the component. */
#define NUM_LEDS CONFIG_WS2812_NUM_LEDS

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
 * This function must be called once before any attempt to transmit LED data.
 * It configures the selected RMT channel and installs the driver.
 *
 * @return ESP_OK on success or an error code from the ESP-IDF drivers.
 */
esp_err_t ws2812ControlInit(void);

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

#endif

#ifdef __cplusplus
}
#endif
