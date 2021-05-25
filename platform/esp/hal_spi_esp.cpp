#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "hal/hal_spi_impl.hpp"
#include "hal/hal_spi_types.h"
using namespace spi;

hal_spi_err_t hal_spi_host_init(uint8_t idx)
{
    return 0;
}
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
}

void post_callback(spi_transaction_t* t)
{
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

hal_spi_err_t write(const uint8_t* data, size_t size, void* userData, bool dma, SpiDataType spiDataType)
{
    return 0;
}
hal_spi_err_t write(uint8_t data, size_t size, void* userData, bool dma, SpiDataType spiDataType)
{
    return 0;
}
}