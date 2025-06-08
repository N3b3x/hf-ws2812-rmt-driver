#include "rmt_wrapper.hpp"

#ifdef __cplusplus

RmtChannel::RmtChannel(gpio_num_t gpio, rmt_channel_t channel)
    : m_gpio(gpio), m_channel(channel) {}

RmtChannel::~RmtChannel()
{
    end();
}

esp_err_t RmtChannel::begin()
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

esp_err_t RmtChannel::transmit(const rmt_item32_t *items, size_t length)
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

void RmtChannel::end()
{
    if (m_active) {
        rmt_driver_uninstall(m_channel);
        m_active = false;
    }
}

#endif // __cplusplus
