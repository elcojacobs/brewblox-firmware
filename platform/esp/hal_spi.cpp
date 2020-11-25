#include "hal/hal_spi.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include <vector>

struct SpiHost {
    spi_host_device_t handle;
    spi_bus_config_t config;
};

class callback_glue_t {
public:
    callback_glue_t(const std::function<void(SpiTransaction& t)>& cb_pre,
                    const std::function<void(SpiTransaction& t)>& cb_post,
                    void* cb_user_data)
        : pre(cb_pre)
        , post(cb_post)
        , user(cb_user_data)
    {
    }
    const std::function<void(SpiTransaction& t)>& pre;
    const std::function<void(SpiTransaction& t)>& post;
    void* user;
};

void callback_glue_pre(spi_transaction_t* t)
{
    // unpack glue
    callback_glue_t* glue = static_cast<callback_glue_t*>(t->user);
    SpiTransaction hal_trans = {
        .tx_data = static_cast<const uint8_t*>(t->tx_buffer),
        .rx_data = static_cast<uint8_t*>(t->rx_buffer),
        .tx_len = t->length >> 3,
        .rx_len = t->rxlength >> 3,
        .user_cb_data = glue->user,
    };
    glue->pre(hal_trans);
}

void callback_glue_post(spi_transaction_t* t)
{
    callback_glue_t* glue = static_cast<callback_glue_t*>(t->user);
    SpiTransaction hal_trans = {
        .tx_data = static_cast<const uint8_t*>(t->tx_buffer),
        .rx_data = static_cast<uint8_t*>(t->rx_buffer),
        .tx_len = t->length >> 3,
        .rx_len = t->rxlength >> 3,
        .user_cb_data = glue->user,
    };
    glue->post(hal_trans);
    delete glue;
}

inline spi_transaction_t glue_transaction(SpiDevice* dev, const SpiTransaction& hal_trans)
{
    spi_transaction_t trans = {
        .flags = 0,
        .cmd = 0,
        .addr = 0,
        .length = hal_trans.tx_len << 3, // esp32 driver wants length in bits
        .rxlength = hal_trans.rx_len << 3,
        .user = nullptr, // new callback_glue_t{dev->pre_cb, dev->post_cb, hal_trans.user_cb_data},
        .tx_buffer = static_cast<const void*>(hal_trans.tx_data),
        .rx_buffer = static_cast<void*>(hal_trans.rx_data),
    };
    return trans;
}

SpiHost spiHosts[1]
    = {
        {
            SPI2_HOST,
            {.mosi_io_num = 4,
             .miso_io_num = 16,
             .sclk_io_num = 17,
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
        .pre_cb = nullptr,   //callback_glue_pre,
        .post_cb = nullptr}; // callback_glue_post};

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

hal_spi_err_t SpiDevice::transmit(const SpiTransaction& transaction, uint32_t timeout)
{
    spi_transaction_t trans = glue_transaction(this, transaction);
    return spi_device_transmit(get_platform_ptr(this), &trans);
}

void SpiDevice::aquire_bus()
{
    spi_device_acquire_bus(get_platform_ptr(this), portMAX_DELAY); // only port max delay is supported currently
    if (onAquire) {
        onAquire();
    }
}

void SpiDevice::release_bus()
{
    spi_device_release_bus(get_platform_ptr(this));
    if (onRelease) {
        onRelease();
    }
}
