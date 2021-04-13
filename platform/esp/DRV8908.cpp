#include "DRV8908.hpp"

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
    uint8_t tx[2] = {uint8_t(static_cast<uint8_t>(address) + uint8_t(0x40)), 0};
    uint8_t rx[2] = {0};

    SpiTransaction t{
        .tx_data = tx,
        .rx_data = rx,
        .tx_len = 2,
        .rx_len = 2,
        .user_cb_data = nullptr,
    };
    auto err = spi.transfer(t);
    if (err == 0) {
        _status = rx[0];
        val = rx[1];
    }
    return err;
}

hal_spi_err_t DRV8908::writeRegister(RegAddr address, uint8_t val)
{
    uint8_t tx[2] = {static_cast<uint8_t>(address), val};
    uint8_t rx[2] = {0};

    SpiTransaction t{
        .tx_data = tx,
        .rx_data = rx,
        .tx_len = 2,
        .rx_len = 2,
        .user_cb_data = nullptr,
    };
    auto err = spi.transfer(t);
    if (err == 0) {
        _status = rx[0];
        // second byte returned is old value
    }
    return err;
}