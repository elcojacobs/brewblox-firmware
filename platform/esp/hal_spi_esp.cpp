#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "hal/hal_spi_impl.hpp"
#include "hal/hal_spi_types.h"
#include "staticAllocator.hpp"
#include <stdio.h>
#include <string.h>
using namespace spi;

auto transactionBuffer = StaticAllocator<spi_transaction_t, 10>();
auto callBackArgsBuffer = StaticAllocator<CallbackArg, 10>();

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
             .flags = SPICOMMON_BUSFLAG_MASTER,
             .intr_flags = 0},
        }};

spi_device_t* get_platform_ptr(Settings& settings)
{
    return static_cast<spi_device_t*>(settings.platform_device_ptr);
}
void pre_callback(spi_transaction_t* t)
{
    if (t->user) {
        auto transactionData = TransactionData{
            .tx_data = reinterpret_cast<const uint8_t*>(t->tx_buffer),
            .rx_data = reinterpret_cast<uint8_t*>(t->rx_buffer),
            .tx_len = t->length / 8,
            .rx_len = t->rxlength / 8};

        auto callbacks = reinterpret_cast<CallbackArg*>(t->user);
        if (callbacks->pre)
            callbacks->pre(transactionData);

        t->tx_buffer = transactionData.tx_data;
        t->rx_buffer = transactionData.rx_data;
        t->length = transactionData.tx_len * 8;
        t->rxlength = transactionData.rx_len * 8;
    }
}

void post_callback(spi_transaction_t* t)
{
    if (t->user) {
        auto transactionData = TransactionData{
            .tx_data = reinterpret_cast<const uint8_t*>(t->tx_buffer),
            .rx_data = reinterpret_cast<uint8_t*>(t->rx_buffer),
            .tx_len = t->length,
            .rx_len = t->rxlength};

        auto callbacks = reinterpret_cast<CallbackArg*>(t->user);
        if (callbacks->post) {
            callbacks->post(transactionData);
        }

        callBackArgsBuffer.free(callbacks);
        transactionBuffer.free(t);
    }
}
error init(Settings& settings)
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

error write(Settings& settings, const uint8_t* data, size_t size)
{
    auto trans = spi_transaction_t{};
    if (size < 4) {
        trans = spi_transaction_t{
            .flags = uint32_t{SPI_TRANS_USE_TXDATA},
            .cmd = 0,
            .addr = 0,
            .length = size * 8, // esp platform wants size in bits
            .rxlength = 0,
            .user = nullptr,
            .tx_buffer = nullptr,
            .rx_buffer = nullptr,
        };
        memcpy(trans.tx_data, data, size);
    } else {
        trans = spi_transaction_t{
            .flags = uint32_t{0},
            .cmd = 0,
            .addr = 0,
            .length = size * 8, // esp platform wants size in bits
            .rxlength = 0,
            .user = nullptr,
            .tx_buffer = data,
            .rx_buffer = nullptr,
        };
    }

    return spi_device_transmit(get_platform_ptr(settings), &trans);
}

error dmaWrite(Settings& settings, const uint8_t* data, size_t size, std::function<void(TransactionData&)> pre, std::function<void(TransactionData&)> post)
{
    // Wait until there is space for the transaction in the static buffer.
    spi_transaction_t* trans;
    while (!(trans = new (transactionBuffer.get()) spi_transaction_t{})) {
    };

    // Wait until there is space for the callbackarg in the static buffer.
    CallbackArg* callBackArg;
    while (!(callBackArg = new (callBackArgsBuffer.get()) CallbackArg{})) {
    };

    *trans = spi_transaction_t{
        .flags = uint32_t{0},
        .cmd = 0,
        .addr = 0,
        .length = size * 8, // esp platform wants size in bits
        .rxlength = 0,
        .user = new (callBackArg) CallbackArg{
            pre,
            post},
        .tx_buffer = data,
        .rx_buffer = nullptr,
    };

    return spi_device_queue_trans(get_platform_ptr(settings), trans, portMAX_DELAY);
}

error writeAndRead(Settings& settings, const uint8_t* tx, size_t txSize, const uint8_t* rx, size_t rxSize)
{
    auto trans = spi_transaction_t{
        .flags = uint32_t{0},
        .cmd = 0,
        .addr = 0,
        .length = txSize * 8, // esp platform wants size in bits
        .rxlength = rxSize * 8,
        .user = nullptr,
        .tx_buffer = const_cast<uint8_t*>(tx),
        .rx_buffer = const_cast<uint8_t*>(rx),
    };

    return spi_device_transmit(get_platform_ptr(settings), &trans);
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

error hal_spi_host_init(uint8_t idx)
{
    auto spi_host = platform_spi::spiHosts[idx];
    auto err = spi_bus_initialize(spi_host.handle, &spi_host.config, SPI_DMA_CH_AUTO);
    if (err != 0) {
        ESP_LOGE("SPI", "spi init error %d", err);
    }
    return err;
}
