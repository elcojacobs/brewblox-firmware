#pragma once

// #include "driver/gpio.h"
// #include "driver/spi_master.h"
#include "hal_spi_types.h"
using namespace spi;

using hal_spi_err_t = int32_t;

namespace platform_spi {

hal_spi_err_t init(Settings& settings);
void deInit(Settings& settings);
hal_spi_err_t write(Settings& settings, const uint8_t* data, size_t size, bool dma, std::function<void(TransactionData&)> pre, std::function<void(TransactionData&)> post, SpiDataType spiDataType);
// hal_spi_err_t write(Settings& settings, uint8_t data, size_t size, std::function<void(TransactionData&)> pre, std::function<void(TransactionData&)> post, SpiDataType spiDataType);

}
