// #pragma once

// #include "driver/gpio.h"
// #include "driver/spi_master.h"
// #include "esp_log.h"
// #include "hal/hal_spi.h"

// namespace platform {
// namespace spi {
//     struct SpiSettings {
//         enum Mode : uint8_t {
//             SPI_MODE0 = 0x00,
//             SPI_MODE1 = 0x01,
//             SPI_MODE2 = 0x02,
//             SPI_MODE3 = 0x03
//         };

//         enum BitOrder : uint8_t {
//             LSBFIRST = 0x00,
//             MSBFIRST = 0x01,
//         };
//         const uint8_t spi_idx = 0; // index to select SPI master in case of multiple masters
//         const int speed;
//         const int queueSize;
//         const int ssPin;
//         const Mode mode = SPI_MODE0;
//         const BitOrder bitOrder = MSBFIRST;
//         std::function<void(SpiTransaction&)> pre_cb;
//         std::function<void(SpiTransaction&)> post_cb;
//         std::function<void()> onAquire;
//         std::function<void()> onRelease;
//         void* platform_device_ptr = nullptr;
//     };
//     struct SpiHost {
//         spi_host_device_t handle;
//         spi_bus_config_t config;
//     };
//     struct CallBackArg {
//         SpiSettings* settings;
//         SpiTransaction t;
//     };

//     hal_spi_err_t init(SpiSettings& settings);
//     void deinit(SpiSettings& settings);
//     hal_spi_err_t transfer_impl(SpiTransaction transaction, bool dmaEnabled, SpiSettings& settings);
//     void aquire_bus_impl(SpiSettings& settings);
//     void release_bus_impl(SpiSettings& settings);
//     bool sense_miso(SpiSettings& settings);
// }
// }