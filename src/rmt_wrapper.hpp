#ifndef RMT_WRAPPER_HPP
#define RMT_WRAPPER_HPP

#include "driver/rmt.h"
#include "driver/gpio.h"
#include "esp_err.h"

#ifdef __cplusplus
/**
 * @brief Lightweight RAII wrapper around an ESP32 RMT channel.
 *
 * This helper configures an RMT channel for transmission and provides
 * a simple transmit method. The driver is automatically uninstalled
 * when the object goes out of scope.
 */
class RmtChannel {
public:
    /**
     * @brief Construct with the desired GPIO and channel.
     */
    explicit RmtChannel(gpio_num_t gpio = GPIO_NUM_18,
                        rmt_channel_t channel = RMT_CHANNEL_0)
        : m_gpio(gpio), m_channel(channel) {}

    /** Initialise and install the RMT driver. */
    esp_err_t begin();

    /**
     * @brief Write a sequence of RMT items.
     *
     * The function blocks until transmission has completed.
     */
    esp_err_t transmit(const rmt_item32_t *items, size_t length);

    /** Uninstall the driver if active. */
    void end();

    ~RmtChannel() { end(); }

private:
    gpio_num_t m_gpio;
    rmt_channel_t m_channel;
    bool m_active = false;
};

inline esp_err_t RmtChannel::begin()
{
    if (m_active) {
        return ESP_OK;
    }
    rmt_config_t config = {};
    config.rmt_mode = RMT_MODE_TX;
    config.channel = m_channel;
    config.gpio_num = m_gpio;
    config.mem_block_num = 3;
    config.tx_config.loop_en = false;
    config.tx_config.carrier_en = false;
    config.tx_config.idle_output_en = true;
    config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
    config.clk_div = 2;
    esp_err_t err = rmt_config(&config);
    if (err != ESP_OK) {
        return err;
    }
    err = rmt_driver_install(m_channel, 0, 0);
    if (err == ESP_OK) {
        m_active = true;
    }
    return err;
}

inline esp_err_t RmtChannel::transmit(const rmt_item32_t *items, size_t length)
{
    if (!m_active) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t err = rmt_write_items(m_channel, items, length, false);
    if (err != ESP_OK) {
        return err;
    }
    return rmt_wait_tx_done(m_channel, portMAX_DELAY);
}

inline void RmtChannel::end()
{
    if (m_active) {
        rmt_driver_uninstall(m_channel);
        m_active = false;
    }
}

#endif // __cplusplus

#endif // RMT_WRAPPER_HPP
