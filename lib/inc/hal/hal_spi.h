/*
 * Copyright 2020 BrewPi B.V./Elco Jacobs.
 *
 * This file is part of Brewblox.
 * 
 * Brewblox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Brewblox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Brewblox.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <functional>
#include <memory>
// #include <mutex>

using hal_spi_err_t = int32_t;

struct SpiTransaction {
    const uint8_t* tx_data = nullptr;
    uint8_t* rx_data = nullptr;
    size_t tx_len = 0;
    size_t rx_len = 0;
    void* user_cb_data = nullptr;
};

using hal_spi_transaction_cb_t = std::function<void(SpiTransaction& t)>&;
using hal_spi_device_handle_t = std::unique_ptr<void, std::function<void(void*)>>;

class SpiConfig {
public:
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

    SpiConfig(uint8_t host_idx, int speed_hz, int queue_size, int ss_pin,
              const hal_spi_transaction_cb_t pre, const hal_spi_transaction_cb_t post, Mode spi_mode = SPI_MODE0, BitOrder bit_order = MSBFIRST)
        : host(host_idx)
        , speed(speed_hz)
        , queueSize(queue_size)
        , ssPin(ss_pin)
        , pre_cb(pre)
        , post_cb(post)
        , mode(spi_mode)
        , bitOrder(bit_order)
    {
    }
    ~SpiConfig() = default;

    uint8_t host = 0; // index to select SPI master in case of multiple masters
    int speed;
    int queueSize;
    int ssPin;
    hal_spi_transaction_cb_t pre_cb;
    hal_spi_transaction_cb_t post_cb;
    void* user_cb_data;

    Mode mode = SPI_MODE0;
    BitOrder bitOrder = MSBFIRST;
};

struct SpiDevice {
    SpiDevice(const hal_spi_transaction_cb_t& pre, const hal_spi_transaction_cb_t& post)
        : hal_pre_cb(pre)
        , hal_post_cb(post)
    {
    }

    hal_spi_err_t init(const SpiConfig& cfg);
    hal_spi_err_t queue_transfer(const SpiTransaction& transaction, uint32_t timeout = 0);
    hal_spi_err_t transmit(const SpiTransaction& transaction, uint32_t timeout = 0);
    void aquire_bus();
    void release_bus();

    // platform handle type with custom deleter
    hal_spi_device_handle_t handle;
    const hal_spi_transaction_cb_t hal_pre_cb;
    const hal_spi_transaction_cb_t hal_post_cb;
};

// bool spi_device_transmit(spi_device_handle_t handle, spi_transaction_t* trans_desc);

hal_spi_err_t spi_queue_transfer(const SpiDevice& dev, const SpiTransaction& transaction);

std::unique_ptr<SpiDevice> spi_device_init(const SpiConfig& client);
