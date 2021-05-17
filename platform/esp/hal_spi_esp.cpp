#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "hal/hal_spi.h"
#include <vector>

struct SpiHost {
    spi_host_device_t handle;
    spi_bus_config_t config;
};

struct CallBackArg {
    SpiDevice* dev;
    SpiTransaction t;
};

void IRAM_ATTR pre_callback(spi_transaction_t* trans)
{
    // Get the hal transaction and the spidevice from the user data.
    auto cbarg = reinterpret_cast<CallBackArg*>(trans->user);

    // Run the pre callback with the hal transaction.
    cbarg->dev->do_pre_cb(cbarg->t);
}
void IRAM_ATTR post_callback(spi_transaction_t* trans)
{
    auto cbarg = reinterpret_cast<CallBackArg*>(trans->user);
    cbarg->dev->do_post_cb(cbarg->t);
    free(trans->user);
    free(trans);
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
             .flags = SPICOMMON_BUSFLAG_MASTER, // investigate ESP_INTR_FLAG_IRAM flag
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
        .pre_cb = pre_callback,
        .post_cb = post_callback};

    spi_device_t* dev_ptr = nullptr;
    auto err = spi_bus_add_device(spi_host.handle, &devcfg, &dev_ptr);
    if (err == ESP_OK) {
        this->platform_device_ptr = dev_ptr;
    } else {
        ESP_LOGE("SPI", "spi device init error %d", err);
    }
    return err;
}

hal_spi_err_t hal_spi_host_init(uint8_t idx)
{
    auto spi_host = spiHosts[idx];
    auto err = spi_bus_initialize(spi_host.handle, &spi_host.config, SPI_DMA_CH_AUTO);
    if (err != 0) {
        ESP_LOGE("SPI", "spi init error %d", err);
    }
    return err;
}

void SpiDevice::deinit()
{
    if (platform_device_ptr) {
        spi_bus_remove_device(get_platform_ptr(this));
    }
}

hal_spi_err_t SpiDevice::transfer_impl(SpiTransaction transaction, bool dmaEnabled)
{

    spi_transaction_t* trans = static_cast<spi_transaction_t*>(heap_caps_malloc(sizeof(spi_transaction_t), MALLOC_CAP_DMA));

    *trans = spi_transaction_t{
        .flags = transaction.txDataType == SpiDataType::VALUE ? uint32_t{SPI_TRANS_USE_TXDATA} : uint32_t{0},
        .cmd = 0,
        .addr = 0,
        .length = transaction.tx_len * 8, // esp platform wants size in bits
        .rxlength = transaction.rx_len * 8,
        .user = heap_caps_malloc(sizeof(CallBackArg), MALLOC_CAP_DMA),
        .tx_buffer = const_cast<uint8_t*>(transaction.tx_data),
        .rx_buffer = transaction.rx_data,
    };

    *static_cast<CallBackArg*>(trans->user) = CallBackArg{
        .dev = this,
        .t = std::move(transaction),
    };

    if (dmaEnabled) {
        return spi_device_queue_trans(get_platform_ptr(this), trans, portMAX_DELAY);
    }
    return spi_device_transmit(get_platform_ptr(this), trans);
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
