#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "hal/hal_spi_impl.hpp"
#include "hal/hal_spi_types.h"
#include "ringBuffer.hpp"
#include <stdio.h>
#include <string.h>

// Making this global is not ideal but the current format of the hal functions is global.
// Maybe the hal functions should live inside a class.

auto transactionBuffer = RingBuffer<spi_transaction_t, 10>();
auto callBackArgsBuffer = RingBuffer<CallbackArg, 10>();

using namespace spi;

namespace platform_spi {
struct SpiHost {
    spi_host_device_t handle;
    spi_bus_config_t config;
};

SpiHost spiHosts[1]
    = {
        {
            SPI2_HOST,
            {.mosi_io_num = 13,
             .miso_io_num = 12,
             .sclk_io_num = 14,
             .quadwp_io_num = -1,
             .quadhd_io_num = -1,
             .max_transfer_sz = 0,
             .flags = SPICOMMON_BUSFLAG_MASTER, // investigate ESP_INTR_FLAG_IRAM flag
             .intr_flags = 0},
        }};

spi_device_t* get_platform_ptr(Settings& settings)
{
    return static_cast<spi_device_t*>(settings.platform_device_ptr);
}
void pre_callback(spi_transaction_t* t)
{
    auto transactionData = TransactionData{
        .tx_data = reinterpret_cast<const uint8_t*>(t->tx_buffer),
        .rx_data = reinterpret_cast<uint8_t*>(t->rx_buffer),
        .tx_len = t->length,
        .rx_len = t->rxlength};

    auto callbacks = reinterpret_cast<CallbackArg*>(t->user);
    if (callbacks->pre)
        callbacks->pre(transactionData);

    t->tx_buffer = transactionData.tx_data;
    t->rx_buffer = transactionData.rx_data;
    t->length = transactionData.tx_len;
    t->rxlength = transactionData.rx_len;
}

void post_callback(spi_transaction_t* t)
{
    auto transactionData = TransactionData{
        .tx_data = reinterpret_cast<const uint8_t*>(t->tx_buffer),
        .rx_data = reinterpret_cast<uint8_t*>(t->rx_buffer),
        .tx_len = t->length,
        .rx_len = t->rxlength};

    auto callbacks = reinterpret_cast<CallbackArg*>(t->user);
    if (callbacks->post) {
        callbacks->post(transactionData);
    }

    callBackArgsBuffer.giveBack(callbacks);
    transactionBuffer.giveBack(t);
}
hal_spi_err_t init(Settings& settings)
{
    auto spi_host = spiHosts[settings.spi_idx];

    spi_device_interface_config_t devcfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = settings.mode,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = settings.speed,
        .input_delay_ns = 0,
        .spics_io_num = settings.ssPin,
        .flags = 0,
        .queue_size = settings.queueSize,
        .pre_cb = pre_callback,
        .post_cb = post_callback};

    spi_device_t* dev_ptr = nullptr;
    auto err = spi_bus_add_device(spi_host.handle, &devcfg, &dev_ptr);
    if (err == ESP_OK) {
        settings.platform_device_ptr = dev_ptr;
    } else {
        ESP_LOGE("SPI", "spi device init error %d", err);
    }
    return err;
}

void deInit(Settings& settings)
{
    if (settings.platform_device_ptr) {
        spi_bus_remove_device(get_platform_ptr(settings));
    }
}

hal_spi_err_t write(Settings& settings, const uint8_t* data, size_t size, bool dma, std::function<void(TransactionData&)> pre, std::function<void(TransactionData&)> post, SpiDataType spiDataType)
{
    spi_transaction_t* trans = transactionBuffer.take();

    if (spiDataType == SpiDataType::VALUE) {
        *trans = spi_transaction_t{
            .flags = uint32_t{SPI_TRANS_USE_TXDATA},
            .cmd = 0,
            .addr = 0,
            .length = size * 8, // esp platform wants size in bits
            .rxlength = 0,
            .user = new (callBackArgsBuffer.take()) CallbackArg{
                pre,
                post},
            .rx_buffer = nullptr,
        };
        memcpy(trans->tx_data, data, size);
    } else {
        *trans = spi_transaction_t{
            .flags = uint32_t{0},
            .cmd = 0,
            .addr = 0,
            .length = size * 8, // esp platform wants size in bits
            .rxlength = 0,
            .user = new (callBackArgsBuffer.take()) CallbackArg{
                pre,
                post},
            .tx_buffer = data,
            .rx_buffer = nullptr,
        };
    }

    if (dma) {
        return spi_device_queue_trans(get_platform_ptr(settings), trans, portMAX_DELAY);
    }
    return spi_device_transmit(get_platform_ptr(settings), trans);
}

hal_spi_err_t writeAndRead(Settings& settings, const uint8_t* tx, size_t txSize, const uint8_t* rx, size_t rxSize, std::function<void(TransactionData&)> pre, std::function<void(TransactionData&)> post, SpiDataType spiDataType)
{
    spi_transaction_t* trans = transactionBuffer.take();

    *trans = spi_transaction_t{
        .flags = uint32_t{0},
        .cmd = 0,
        .addr = 0,
        .length = txSize * 8, // esp platform wants size in bits
        .rxlength = rxSize * 8,
        .user = new (callBackArgsBuffer.take()) CallbackArg{
            pre,
            post},
        .tx_buffer = const_cast<uint8_t*>(tx),
        .rx_buffer = const_cast<uint8_t*>(rx),
    };

    return spi_device_transmit(get_platform_ptr(settings), trans);
}

void aquire_bus(Settings& settings)
{
    spi_device_acquire_bus(get_platform_ptr(settings), portMAX_DELAY);
}
void release_bus(Settings& settings)
{
    spi_device_release_bus(get_platform_ptr(settings));
}
}

hal_spi_err_t hal_spi_host_init(uint8_t idx)
{
    auto spi_host = platform_spi::spiHosts[idx];
    auto err = spi_bus_initialize(spi_host.handle, &spi_host.config, SPI_DMA_CH_AUTO);
    if (err != 0) {
        ESP_LOGE("SPI", "spi init error %d", err);
    }
    return err;
}
