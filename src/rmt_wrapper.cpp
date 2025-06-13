#include "rmt_wrapper.hpp"
#include "driver/rmt_types.h"

#ifdef __cplusplus

RmtChannel::RmtChannel(gpio_num_t gpio, rmt_channel_t channel)
    : m_gpio(gpio)
{
    (void)channel;
}

RmtChannel::~RmtChannel()
{
    end();
}

esp_err_t RmtChannel::begin()
{
    if (m_active) {
        return ESP_OK;
    }
    rmt_tx_channel_config_t config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = m_gpio,
        .mem_block_symbols = 64,
        .resolution_hz = 10 * 1000 * 1000,
        .trans_queue_depth = 1,
    };
    esp_err_t err = rmt_new_tx_channel(&config, &m_handle);
    if (err != ESP_OK) {
        return err;
    }
    led_strip_encoder_config_t enc_config = {
        .resolution = config.resolution_hz,
    };
    err = rmt_new_led_strip_encoder(&enc_config, &m_encoder);
    if (err != ESP_OK) {
        rmt_del_channel(m_handle);
        return err;
    }
    err = rmt_enable(m_handle);
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
    rmt_transmit_config_t tx_config = {
        .loop_count = 0,
    };
    esp_err_t err = rmt_transmit(m_handle, m_encoder, (const void *)items,
                                 length * sizeof(rmt_item32_t), &tx_config);
    if (err != ESP_OK) {
        return err;
    }
    return rmt_tx_wait_all_done(m_handle, portMAX_DELAY);
}

void RmtChannel::end()
{
    if (m_active) {
        rmt_disable(m_handle);
        rmt_del_encoder(m_encoder);
        rmt_del_channel(m_handle);
        m_handle = nullptr;
        m_encoder = nullptr;
        m_active = false;
    }
}

#endif // __cplusplus
