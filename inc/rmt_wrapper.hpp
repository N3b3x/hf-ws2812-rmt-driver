// rmt_wrapper.hpp — High‑level C++ RAII wrapper for the ESP‑IDF v5.3 RMT driver
// on ESP32‑C6 Copyright (c) 2025 Nebiyu Tadesse SPDX‑License‑Identifier:
// GPL‑3.0-or-later
/**
 * @file rmt_wrapper.hpp
 * @brief A modern C++17 header‑only wrapper around the ESP‑IDF v5.3 “new” RMT
 * @copyright Copyright (c) 2024-2025 HardFOC. All rights reserved.
 */
#pragma once

#include "esp_idf_version.h"

#if !defined(ESP_IDF_VERSION)
#error "ESP_IDF_VERSION macro is not defined. Is <esp_idf_version.h> included?"
#elif ESP_IDF_VERSION_MAJOR < 5
#error "This wrapper requires ESP‑IDF v5.0 or newer (uses the new RMT driver)."
#endif

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <utility>

#include "esp_check.h"
#include "esp_log.h"
#include "esp_private/esp_clk.h"

#include "driver/rmt_encoder.h"
#include "driver/rmt_rx.h"
#include "driver/rmt_tx.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

namespace ws2812 {

namespace detail {
[[nodiscard]] inline esp_err_t log_if_error(esp_err_t err, const char* tag, const char* msg) {
  if (err != ESP_OK) {
    ESP_LOGE(tag, "%s: %s", msg, esp_err_to_name(err));
  }
  return err;
}

} // namespace detail

//=============================================================================
// RmtTx — Transmit‑only wrapper
//=============================================================================
class RmtTx {
public:
  /**
   * @brief Create a Transmit channel.
   *
   * @param gpio           Output GPIO number.
   * @param resolution_hz  Clock resolution in Hz (max 40 MHz on ESP32‑C6).
   * @param mem_symbols    Symbol memory per channel (in 32”bit words).
   * @param with_dma       Enable DMA mode for huge transfers.
   * @param queue_depth    Depth of internal Transmit queue.
   * @param clk_src        Clock source (default: RMT_CLK_SRC_DEFAULT).
   * @param channel_id     [Optional] fixed channel (0‑3). Pass -1 for auto.
   */
  explicit RmtTx(gpio_num_t gpio, uint32_t resolution_hz = 10'000'000, size_t mem_symbols = 64,
                 bool with_dma = false, uint32_t queue_depth = 4,
                 rmt_clock_source_t clk_src = RMT_CLK_SRC_DEFAULT, int channel_id = -1) {
    constexpr char TAG[] = "RmtTx";

    if (channel_id < 0) {
      channel_id = 0;
    }

    rmt_tx_channel_config_t cfg = {};
    cfg.gpio_num = gpio;
    cfg.clk_src = clk_src;
    cfg.mem_block_symbols = mem_symbols;
    cfg.resolution_hz = resolution_hz;
    cfg.trans_queue_depth = queue_depth;
    cfg.flags.invert_out = false;
    cfg.flags.with_dma = with_dma;

    ESP_ERROR_CHECK(detail::log_if_error(rmt_new_tx_channel(&cfg, &handle_), TAG,
                                         "failed to create TX channel"));
    ESP_ERROR_CHECK(detail::log_if_error(rmt_enable(handle_), TAG, "enable TX"));

    // Prepare a generic copy encoder (raw symbol streaming)
    rmt_copy_encoder_config_t copy_cfg = {};
    ESP_ERROR_CHECK(rmt_new_copy_encoder(&copy_cfg, &copy_encoder_));
  }

  RmtTx(const RmtTx&) = delete;
  RmtTx& operator=(const RmtTx&) = delete;

  RmtTx(RmtTx&& other) noexcept {
    *this = std::move(other);
  }
  RmtTx& operator=(RmtTx&& other) noexcept {
    std::swap(handle_, other.handle_);
    std::swap(copy_encoder_, other.copy_encoder_);
    std::swap(ws_encoder_, other.ws_encoder_);
    return *this;
  }

  ~RmtTx() {
    if (ws_encoder_)
      rmt_del_encoder(ws_encoder_);
    if (copy_encoder_)
      rmt_del_encoder(copy_encoder_);
    if (handle_)
      rmt_disable(handle_), rmt_del_channel(handle_);
  }

  //-------------------------------------------------------------------------
  /**
   * @brief Transmit raw RMT symbols (blocking until done).
   */
  esp_err_t Transmit(const rmt_symbol_word_t* symbols, size_t count,
                     TickType_t timeout = portMAX_DELAY) const {
    rmt_transmit_config_t tx_cfg = {};
    tx_cfg.loop_count = 0;
    esp_err_t err =
        rmt_transmit(handle_, copy_encoder_, symbols, count * sizeof(rmt_symbol_word_t), &tx_cfg);
    if (err != ESP_OK)
      return err;
    return rmt_tx_wait_all_done(handle_, timeout);
  }

  //-------------------------------------------------------------------------
  /**
   * @brief Transmit an arbitrary byte stream with custom bit timing.
   *
   * @param bit0  Symbol encoding logic‑0 (LSB first by default).
   * @param bit1  Symbol encoding logic‑1.
   */
  esp_err_t TransmitBytes(const uint8_t* data, size_t length, const rmt_symbol_word_t& bit0,
                           const rmt_symbol_word_t& bit1, TickType_t timeout = portMAX_DELAY) {
    // Create a throw‑away bytes encoder (kept only for this call)
    rmt_bytes_encoder_config_t be_cfg = {};
    be_cfg.bit0 = bit0;
    be_cfg.bit1 = bit1;
    be_cfg.flags.msb_first = true;
    rmt_encoder_handle_t bytes_enc = nullptr;
    ESP_RETURN_ON_ERROR(rmt_new_bytes_encoder(&be_cfg, &bytes_enc), "RmtTx", "new bytes enc");

    rmt_transmit_config_t tx_cfg = {};
    tx_cfg.loop_count = 0;
    esp_err_t err = rmt_transmit(handle_, bytes_enc, data, length, &tx_cfg);
    if (err == ESP_OK)
      err = rmt_tx_wait_all_done(handle_, timeout);
    rmt_del_encoder(bytes_enc);
    return err;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief Helper dedicated to WS2812/NeoPixel @ 800 kHz.
   * Uses 10 MHz or faster resolution to achieve accurate timings.
   */
  esp_err_t TransmitWs2812(const uint8_t* grb, size_t length, TickType_t timeout = portMAX_DELAY) {
    if (!ws_encoder_) {
      rmt_bytes_encoder_config_t ws_cfg = {};
      ws_cfg.bit0 = makeWs2812Bit0();
      ws_cfg.bit1 = makeWs2812Bit1();
      ws_cfg.flags.msb_first = true;
      ESP_RETURN_ON_ERROR(rmt_new_bytes_encoder(&ws_cfg, &ws_encoder_), "RmtTx", "new ws2812 enc");
    }
    rmt_transmit_config_t tx_cfg = {};
    tx_cfg.loop_count = 0;
    esp_err_t err = rmt_transmit(handle_, ws_encoder_, grb, length, &tx_cfg);
    if (err == ESP_OK)
      err = rmt_tx_wait_all_done(handle_, timeout);
    return err;
  }

  rmt_channel_handle_t Handle() const {
    return handle_;
  }

private:
  static constexpr rmt_symbol_word_t makeWs2812Bit0(uint32_t resolution_hz = 10'000'000) {
    // T0H=0.4 μs, T0L≈0.85 μs
    uint32_t ticks_h = (400'000'000ULL / resolution_hz + 9) / 10; // round‑nearest
    uint32_t ticks_l = (850'000'000ULL / resolution_hz + 9) / 10;
    return {
        .duration0 = (uint16_t)ticks_h, .level0 = 1, .duration1 = (uint16_t)ticks_l, .level1 = 0};
  }

  static constexpr rmt_symbol_word_t makeWs2812Bit1(uint32_t resolution_hz = 10'000'000) {
    // T1H=0.8 μs, T1L≈0.45 μs
    uint32_t ticks_h = (800'000'000ULL / resolution_hz + 9) / 10;
    uint32_t ticks_l = (450'000'000ULL / resolution_hz + 9) / 10;
    return {
        .duration0 = (uint16_t)ticks_h, .level0 = 1, .duration1 = (uint16_t)ticks_l, .level1 = 0};
  }

  rmt_channel_handle_t handle_ = nullptr;
  rmt_encoder_handle_t copy_encoder_ = nullptr;
  rmt_encoder_handle_t ws_encoder_ = nullptr;
};

//=============================================================================
// RmtRx — Receive‑only wrapper
//=============================================================================
class RmtRx {
public:
  explicit RmtRx(gpio_num_t gpio, uint32_t resolution_hz = 10'000'000, size_t mem_symbols = 64,
                 uint32_t idle_threshold_us = 1000, uint32_t filter_ns = 200,
                 rmt_clock_source_t clk_src = RMT_CLK_SRC_DEFAULT, int channel_id = -1) {
    constexpr char TAG[] = "RmtRx";

    if (channel_id < 0) {
      channel_id = 0;
    }

    rmt_rx_channel_config_t cfg = {};
    cfg.gpio_num = gpio;
    cfg.clk_src = clk_src;
    cfg.mem_block_symbols = mem_symbols;
    cfg.resolution_hz = resolution_hz;
    cfg.flags.invert_in = false;
    ESP_ERROR_CHECK(detail::log_if_error(rmt_new_rx_channel(&cfg, &_handle), TAG, "create RX"));

    rmt_rx_event_callbacks_t cbs = {};
    cbs.on_recv_done = &RmtRx::rx_done_cb_static;
    ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(_handle, &cbs, this));
    ESP_ERROR_CHECK(rmt_enable(_handle));

    // Create queue for ISR -> task communication
    _queue = xQueueCreate(4, sizeof(size_t));

    // Configure RX receive behavior (can be changed later)
    _rcv_cfg.signal_range_max_ns = idle_threshold_us * 1000ULL;
    _rcv_cfg.signal_range_min_ns = filter_ns;
  }

  RmtRx(const RmtRx&) = delete;
  RmtRx& operator=(const RmtRx&) = delete;
  RmtRx(RmtRx&& o) noexcept {
    *this = std::move(o);
  }
  RmtRx& operator=(RmtRx&& o) noexcept {
    std::swap(_handle, o._handle);
    std::swap(_queue, o._queue);
    std::swap(_rcv_cfg, o._rcv_cfg);
    return *this;
  }

  ~RmtRx() {
    if (_handle)
      rmt_disable(_handle), rmt_del_channel(_handle);
    if (_queue)
      vQueueDelete(_queue);
  }

  /**
   * @brief Start a single receive transaction and block until it completes.
   *
   * The caller must provide a buffer sized to hold `buffer_symbols` symbols.
   */
  esp_err_t receive(rmt_symbol_word_t* buffer, size_t buffer_symbols, size_t* out_symbols,
                    TickType_t timeout = portMAX_DELAY) {
    if (!buffer || !out_symbols)
      return ESP_ERR_INVALID_ARG;

    esp_err_t err =
        rmt_receive(_handle, buffer, buffer_symbols * sizeof(rmt_symbol_word_t), &_rcv_cfg);
    if (err != ESP_OK)
      return err;

    // Wait until ISR posts the symbol count
    if (xQueueReceive(_queue, out_symbols, timeout) != pdPASS) {
      return ESP_ERR_TIMEOUT;
    }
    return ESP_OK;
  }

  rmt_channel_handle_t handle() const {
    return _handle;
  }

private:
  static bool rx_done_cb_static(rmt_channel_handle_t /*chan*/,
                                const rmt_rx_done_event_data_t* edata, void* user_ctx) {
    auto* self = static_cast<RmtRx*>(user_ctx);
    BaseType_t HPW = pdFALSE;
    xQueueSendFromISR(self->_queue, &edata->num_symbols, &HPW);
    return HPW == pdTRUE; // context switch request
  }

  rmt_channel_handle_t _handle = nullptr;
  QueueHandle_t _queue = nullptr;
  rmt_receive_config_t _rcv_cfg = {};
};

} // namespace ws2812
