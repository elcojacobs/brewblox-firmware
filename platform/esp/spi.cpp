#include "driver/spi_master.h"
#include "hal/hal_spi.h"
#include <vector>

struct SpiHost {
    spi_host_device_t host;
    spi_bus_config_t config;
    std::vector<spi_device_handle_t> devices;
};

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
            {},
        }};

// platform dependent implementation of transfer functions
uint8_t
spi_transfer(const SpiConfig& client, uint8_t data) { return 0; }
void spi_transfer_dma(const SpiConfig& client, void* tx_buffer, void* rx_buffer, size_t length, spi_dma_transfercomplete_callback_t user_callback) {}
void spi_transfer_cancel(const SpiConfig& client) {}

spi_device_interface_config_t convertConfig(const SpiConfig& client)
{
    spi_device_interface_config_t devcfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = client.mode,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = client.speed,
        .input_delay_ns = 0,
        .spics_io_num = client.ssPin,
        .flags = 0,
        .queue_size = client.queueSize,
        .pre_cb = nullptr,
        .post_cb = nullptr,
    };
    return devcfg;
}

bool spi_device_init(const SpiConfig& client)
{
    auto host = spiHosts[client.host];
    if (host.devices.empty()) {
        spi_bus_initialize(host.host, &host.config, 1);
    }
    auto config = convertConfig(client);
    spi_device_handle_t handle;
    esp_err_t err = spi_bus_add_device(host.host, &config, &handle);
    if (err == ESP_OK) {
        host.devices.push_back(std::move(handle));
        return true;
    }
    return false;
}