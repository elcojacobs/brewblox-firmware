#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "hal/hal_spi.h"
#include <vector>

struct SpiHost {
    spi_host_device_t handle;
    spi_bus_config_t config;
};

inline spi_transaction_t glue_transaction(SpiDevice* dev, const SpiTransaction& hal_trans)
{
    spi_transaction_t trans = {
        .flags = 0,
        .cmd = 0,
        .addr = 0,
        .length = hal_trans.tx_len *8, // esp32 driver wants length in bits
        .rxlength = hal_trans.rx_len *8,
        .user = nullptr,
        .tx_buffer = static_cast<const void*>(hal_trans.tx_data),
        .rx_buffer = static_cast<void*>(hal_trans.rx_data),
    };
    return trans;
}

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

spi_device_t* get_platform_ptr(SpiDevice* dev)
{
    return static_cast<spi_device_t*>(dev->platform_device_ptr);
}

// platform dependent implementation of transfer functions
hal_spi_err_t SpiDevice::init()
{
    auto spi_host = spiHosts[this->spi_idx];
    auto err = spi_bus_initialize(spi_host.handle, &spi_host.config, 1);
    if (err != 0) {
        ESP_LOGE("SPI", "spi init error %d", err);
    }

    spi_device_interface_config_t devcfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = this->mode,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = this->speed,
        .input_delay_ns = 0,
        .spics_io_num = this->ssPin,
        .flags = 0,
        .queue_size = this->queueSize,
        .pre_cb = nullptr,
        .post_cb = nullptr};

    spi_device_t* dev_ptr = nullptr;
    err = spi_bus_add_device(spi_host.handle, &devcfg, &dev_ptr);
    if (err == ESP_OK) {
        this->platform_device_ptr = dev_ptr;
    } else {
        ESP_LOGE("SPI", "spi device init error %d", err);
    }
    return err;
}

void SpiDevice::deinit()
{
    if (platform_device_ptr) {
        spi_bus_remove_device(get_platform_ptr(this));
    }
}

hal_spi_err_t SpiDevice::queue_transfer(const SpiTransaction& transaction, uint32_t timeout)
{
    spi_transaction_t trans = glue_transaction(this, transaction);
    return spi_device_queue_trans(get_platform_ptr(this), &trans, timeout ? timeout : portMAX_DELAY);
}

// hal_spi_err_t spi_get_trans_result(const SpiDevice& dev, SpiTransaction* transaction, uint32_t timeout)
// {
//     spi_transaction_t *trans = glue_transaction(dev, transaction);
//     return spi_device_get_trans_result(get_platform_ptr(dev), &trans, timeout, timeout ? timeout : portMAX_DELAY);
// }

hal_spi_err_t SpiDevice::transfer_impl(const SpiTransaction& transaction, uint32_t timeout)
{
    spi_transaction_t trans = glue_transaction(this, transaction);
    return spi_device_transmit(get_platform_ptr(this), &trans);
}

void SpiDevice::aquire_bus_impl()
{
    spi_device_acquire_bus(get_platform_ptr(this), portMAX_DELAY); // only port max delay is supported currently
}

void SpiDevice::release_bus_impl()
{
    spi_device_release_bus(get_platform_ptr(this));
}

bool SpiDevice::sense_miso()
{
    auto pin = gpio_num_t(spiHosts[this->spi_idx].config.miso_io_num);
    return gpio_get_level(pin) != 0;
}
