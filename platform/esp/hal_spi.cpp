#include "hal/hal_spi.h"
#include "driver/spi_master.h"
#include <vector>

struct SpiHost {
    spi_host_device_t host;
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
    callback_glue_t* glue = reinterpret_cast<callback_glue_t*>(t->user);
    SpiTransaction hal_trans = {
        .tx_data = reinterpret_cast<const uint8_t*>(t->tx_buffer),
        .rx_data = reinterpret_cast<uint8_t*>(t->rx_buffer),
        .tx_len = t->length,
        .rx_len = t->rxlength,
        .user_cb_data = glue->user,
    };
    glue->pre(hal_trans);
}

void callback_glue_post(spi_transaction_t* t)
{
    callback_glue_t* glue = reinterpret_cast<callback_glue_t*>(t->user);
    SpiTransaction hal_trans = {
        .tx_data = reinterpret_cast<const uint8_t*>(t->tx_buffer),
        .rx_data = reinterpret_cast<uint8_t*>(t->rx_buffer),
        .tx_len = t->length,
        .rx_len = t->rxlength,
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
        .length = hal_trans.tx_len,
        .rxlength = hal_trans.rx_len,
        .user = new callback_glue_t{dev->hal_pre_cb, dev->hal_post_cb, hal_trans.user_cb_data},
        .tx_buffer = reinterpret_cast<const void*>(hal_trans.tx_data),
        .rx_buffer = reinterpret_cast<void*>(hal_trans.rx_data),
    };
    return trans;
}

spi_device_interface_config_t convert_config(const SpiConfig& cfg)
{
    spi_device_interface_config_t devcfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = cfg.mode,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = cfg.speed,
        .input_delay_ns = 0,
        .spics_io_num = cfg.ssPin,
        .flags = 0,
        .queue_size = cfg.queueSize,
        .pre_cb = callback_glue_pre,
        .post_cb = callback_glue_post};
    return devcfg;
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

spi_device_handle_t get_handle(SpiDevice* dev)
{
    return reinterpret_cast<spi_device_handle_t>(dev->handle.get());
}

// platform dependent implementation of transfer functions
hal_spi_err_t SpiDevice::init(const SpiConfig& cfg)
{
    auto host = spiHosts[cfg.host];
    auto config = convert_config(cfg);
    spi_bus_initialize(host.host, &host.config, 1);
    spi_device_handle_t handle;
    esp_err_t err = spi_bus_add_device(host.host, &config, &handle);
    if (err == ESP_OK) {
        this->handle = hal_spi_device_handle_t(
            reinterpret_cast<void*>(handle),
            [](void* h) {
                spi_bus_remove_device(reinterpret_cast<spi_device_handle_t>(h));
            });
    }
    return err;
}

hal_spi_err_t SpiDevice::queue_transfer(const SpiTransaction& transaction, uint32_t timeout)
{
    spi_transaction_t trans = glue_transaction(this, transaction);
    return spi_device_queue_trans(get_handle(this), &trans, timeout ? timeout : portMAX_DELAY);
}

// hal_spi_err_t spi_get_trans_result(const SpiDevice& dev, SpiTransaction* transaction, uint32_t timeout)
// {
//     spi_transaction_t *trans = glue_transaction(dev, transaction);
//     return spi_device_get_trans_result(get_handle(dev), &trans, timeout, timeout ? timeout : portMAX_DELAY);
// }

hal_spi_err_t SpiDevice::transmit(const SpiTransaction& transaction, uint32_t timeout)
{
    spi_transaction_t trans = glue_transaction(this, transaction);
    return spi_device_transmit(get_handle(this), &trans);
}

void SpiDevice::aquire_bus()
{
    spi_device_acquire_bus(get_handle(this), portMAX_DELAY); // only port max delay is supported currently
}

void SpiDevice::release_bus()
{
    spi_device_release_bus(get_handle(this));
}
