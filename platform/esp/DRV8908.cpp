#include "DRV8908.hpp"
#include "esp_err.h"
#include <array>

DRV8908::DRV8908(uint8_t spi_idx, int ss,
                 std::function<void()> on_spi_aquire,
                 std::function<void()> on_spi_release)
    : spi(
        spi::Settings{
            .spi_idx = spi_idx,
            .speed = 100000,
            .queueSize = 1,
            .ssPin = ss,
            .mode = spi::Settings::Mode::SPI_MODE1,
            .bitOrder = spi::Settings::BitOrder::MSBFIRST,
            .on_Aquire = []() {},
            .on_Release = []() {}})
{
    spi.init();
}

spi::error DRV8908::readRegister(RegAddr address, uint8_t& val)
{
    spi.aquire_bus();
    std::array<uint8_t, 2> tx{uint8_t(static_cast<uint8_t>(address) | uint8_t(0x40)), 0};
    std::array<uint8_t, 2> rx{0, 0};
    auto ec = spi.write_and_read(tx, rx);
    ESP_ERROR_CHECK_WITHOUT_ABORT(ec);
    if (ec == 0) {
        _status = rx[0];
        val = rx[1];
    }
    spi.release_bus();

    return ec;
}

spi::error DRV8908::writeRegister(RegAddr address, uint8_t val)
{
    std::array<uint8_t, 2> tx{static_cast<uint8_t>(address), val};
    std::array<uint8_t, 2> rx{0, 0};
    spi.aquire_bus();
    auto ec = spi.write_and_read(tx, rx);
    ESP_ERROR_CHECK_WITHOUT_ABORT(ec);
    spi.release_bus();
    if (ec == 0) {
        _status = rx[0];
        // second byte returned is old value
    }
    return ec;
}