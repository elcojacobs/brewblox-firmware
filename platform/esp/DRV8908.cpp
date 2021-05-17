#include "DRV8908.hpp"
#include <array>

DRV8908::DRV8908(uint8_t spi_idx, int ss,
                 std::function<void()> on_spi_aquire,
                 std::function<void()> on_spi_release)
    : spi(spi_idx, 100000, 1, ss,
          SpiDevice::Mode::SPI_MODE1, SpiDevice::BitOrder::MSBFIRST,
          on_spi_aquire, on_spi_release)
{
    spi.init();
}

hal_spi_err_t DRV8908::readRegister(RegAddr address, uint8_t& val)
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

hal_spi_err_t DRV8908::writeRegister(RegAddr address, uint8_t val)
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