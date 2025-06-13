#ifndef RMT_WRAPPER_HPP
#define RMT_WRAPPER_HPP

#include "driver/rmt_tx.h"
#include "led_strip_encoder.h"
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
                        rmt_channel_t channel = RMT_CHANNEL_0);

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

    ~RmtChannel();

private:
    gpio_num_t m_gpio;
    rmt_channel_handle_t m_handle = nullptr;
    rmt_encoder_handle_t m_encoder = nullptr;
    bool m_active = false;
};


#endif // __cplusplus

#endif // RMT_WRAPPER_HPP
