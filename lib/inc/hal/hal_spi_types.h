#pragma once
#include <functional>
#include <stdint.h>

namespace spi {
enum class SpiDataType {
    POINTER,
    MALLOCED_POINTER,
    VALUE,
};
using hal_spi_err_t = int32_t;
struct Transaction {
    uint8_t* tx_data = nullptr;
    uint8_t* rx_data = nullptr;
    size_t tx_len = 0;
    size_t rx_len = 0;
    void* user_cb_data = nullptr;
    SpiDataType txDataType = SpiDataType::POINTER;
    SpiDataType userDataType = SpiDataType::POINTER;
};

struct Settings {
    enum Mode : uint8_t {
        SPI_MODE0 = 0x00,
        SPI_MODE1 = 0x01,
        SPI_MODE2 = 0x02,
        SPI_MODE3 = 0x03
    };
    enum BitOrder : uint8_t {
        LSBFIRST = 0x00,
        MSBFIRST = 0x01,
    };
    const uint8_t spi_idx = 0; // index to select SPI master in case of multiple masters
    const int speed;
    const int queueSize;
    const int ssPin;
    const Mode mode = SPI_MODE0;
    const BitOrder bitOrder = MSBFIRST;
    std::function<void(Transaction&)> pre_cb;
    std::function<void(Transaction&)> post_cb;
    std::function<void()> on_Aquire;
    std::function<void()> on_Release;
    void* platform_device_ptr = nullptr;
};

}