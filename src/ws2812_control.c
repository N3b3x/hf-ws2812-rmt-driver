#include "ws2812_control.h"
#include "driver/rmt.h"
#include "esp_err.h"
#include "esp_check.h"

// Configure these based on your project needs using menuconfig ********
#define LED_RMT_TX_CHANNEL		CONFIG_WS2812_LED_RMT_TX_CHANNEL
#define LED_RMT_TX_GPIO			CONFIG_WS2812_LED_RMT_TX_GPIO
// ****************************************************


#if CONFIG_WS2812_LED_TYPE_RGB
#define BITS_PER_LED_CMD	24
#elif CONFIG_WS2812_LED_TYPE_RGBW
#define BITS_PER_LED_CMD	32
#endif

#define LED_BUFFER_ITEMS	(NUM_LEDS * BITS_PER_LED_CMD)

// These values are determined by measuring pulse timing with logic analyzer and adjusting to match datasheet. 
#define T0H	CONFIG_WS2812_T0H  // 0 bit high time
#define T1H	CONFIG_WS2812_T1H  // 1 bit high time
#define T0L	CONFIG_WS2812_T0L  // low time for either bit
#define T1L	CONFIG_WS2812_T1L

// Tag for log messages
static const char *TAG = "NeoPixel WS2812 Driver";

// This is the buffer which the hw peripheral will access while pulsing the output pin
static rmt_item32_t led_data_buffer[LED_BUFFER_ITEMS];
static rmt_channel_t s_channel = LED_RMT_TX_CHANNEL;
static gpio_num_t s_gpio = LED_RMT_TX_GPIO;

// Global brightness scaling value
static uint8_t s_brightness = CONFIG_WS2812_DEFAULT_BRIGHTNESS;

/**
 * @brief Prepare the RMT buffer from an array of LED colours.
 *
 * @param new_state Colours to transmit to the LEDs.
 */
static void setupRmtDataBuffer(struct led_state new_state);

/**
 * @brief Initialise the RMT driver for WS2812 output.
 *
 * Must be called once before any LED data is transmitted. The
 * pin and channel can be chosen at runtime.
 *
 * @param gpio_num GPIO connected to the LED strip.
 * @param channel  RMT channel used for transmission.
 * @return ESP_OK on success or an ESP-IDF error code.
 */
esp_err_t ws2812ControlInit(gpio_num_t gpio_num, rmt_channel_t channel)
{
  s_channel = channel;
  s_gpio = gpio_num;
  rmt_config_t config = {
    .rmt_mode = RMT_MODE_TX,
    .channel = channel,
    .gpio_num = gpio_num,
    .mem_block_num = 3,
    .tx_config.loop_en = false,
    .tx_config.carrier_en = false,
    .tx_config.idle_output_en = true,
    .tx_config.idle_level = 0,
    .clk_div = 2
  };

  ESP_RETURN_ON_ERROR(
    rmt_config(&config),
    TAG,
    "Failed to configure RMT");
  ESP_RETURN_ON_ERROR(
    rmt_driver_install(config.channel, 0, 0),
    TAG,
    "Failed to install RMT driver");

  return ESP_OK;
}

/**
 * @brief Transmit colour values to the LED chain.
 *
 * The function blocks until the RMT peripheral finishes sending
 * all LED data.
 *
 * @param new_state Desired LED colours.
 * @return ESP_OK on success or an ESP-IDF error code.
 */
esp_err_t ws2812WriteLeds(struct led_state new_state)
{
  setupRmtDataBuffer(new_state);
  ESP_RETURN_ON_ERROR(
    rmt_write_items(s_channel, led_data_buffer, LED_BUFFER_ITEMS, false),
    TAG,
    "Failed to write items");
  ESP_RETURN_ON_ERROR(
    rmt_wait_tx_done(s_channel, portMAX_DELAY),
    TAG,
    "Failed to wait for RMT transmission to finish");

  return ESP_OK;
}

void ws2812SetBrightness(uint8_t brightness)
{
  s_brightness = brightness;
}

/**
 * @brief Convert colour values into RMT items for transmission.
 *
 * @param new_state Colours to send.
 */
static void setupRmtDataBuffer(struct led_state new_state)
{
  for (uint32_t led = 0; led < NUM_LEDS; led++) {
    uint32_t color = new_state.leds[led];
#if CONFIG_WS2812_LED_TYPE_RGBW
    uint8_t w = (color >> 24) & 0xFF;
#endif
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    r = (r * s_brightness) / 255;
    g = (g * s_brightness) / 255;
    b = (b * s_brightness) / 255;
#if CONFIG_WS2812_LED_TYPE_RGBW
    w = (w * s_brightness) / 255;
    uint32_t bits_to_send = (w << 24) | (r << 16) | (g << 8) | b;
#else
    uint32_t bits_to_send = (r << 16) | (g << 8) | b;
#endif
    uint32_t mask = 1 << (BITS_PER_LED_CMD - 1);

    for (uint32_t bit = 0; bit < BITS_PER_LED_CMD; bit++) {
      uint32_t bit_is_set = bits_to_send & mask;
      led_data_buffer[(led * BITS_PER_LED_CMD) + bit] =
          bit_is_set ? (rmt_item32_t){{{T1H, 1, T1L, 0}}}
                     : (rmt_item32_t){{{T0H, 1, T0L, 0}}};
      mask >>= 1;
    }
  }
}
