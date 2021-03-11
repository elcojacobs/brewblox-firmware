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
#include <hal/hal_delay.h>
#include <memory>

using hal_spi_err_t = int32_t;

class SpiDeviceHandle;

struct SpiTransaction {
    const uint8_t* tx_data = nullptr;
    uint8_t* rx_data = nullptr;
    size_t tx_len = 0;
    size_t rx_len = 0;
    void* user_cb_data = nullptr;
};

struct SpiDevice {
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

    SpiDevice(uint8_t host_idx, int speed_hz, int queue_size, int ss_pin,
              Mode spi_mode, BitOrder bit_order,
              std::function<void()> on_aquire = {}, std::function<void()> on_release = {},
              std::function<void(SpiTransaction& t)> pre = {}, std::function<void(SpiTransaction& t)> post = {})
        : spi_idx(host_idx)
        , speed(speed_hz)
        , queueSize(queue_size)
        , ssPin(ss_pin)
        , mode(spi_mode)
        , bitOrder(bit_order)
        , pre_cb(pre)
        , post_cb(post)
        , onAquire(on_aquire)
        , onRelease(on_release)
    {
    }

    ~SpiDevice()
    {
        deinit();
    }

    const uint8_t spi_idx = 0; // index to select SPI master in case of multiple masters
    const int speed;
    const int queueSize;
    const int ssPin;
    const Mode mode = SPI_MODE0;
    const BitOrder bitOrder = MSBFIRST;

    hal_spi_err_t init();
    void deinit();
    hal_spi_err_t queue_transfer(const SpiTransaction& transaction, uint32_t timeout = 0);
    hal_spi_err_t transfer(const SpiTransaction& transaction, uint32_t timeout = 0)
    {
        // if bus has not been already aquired, aquire and release
        bool inTransaction = hasBus;
        if (!inTransaction) {
            aquire_bus();
        }
        auto err = transfer_impl(transaction, timeout);
        if (!inTransaction) {
            release_bus();
        }
        return err;
    }

    void aquire_bus()
    {
        if (hasBus) {
            return;
        } else {
            aquire_bus_impl();
            hasBus = true;
            if (onAquire) {
                onAquire();
            }
            hal_delay_us(20);
        }
    }

    void release_bus()
    {
        release_bus_impl();
        hasBus = false;
        if (onRelease) {
            onRelease();
        }
        hal_delay_us(20);
    }

    inline bool has_bus()
    {
        return hasBus;
    }

    bool sense_miso();

    void* platform_device_ptr;

private:
    // callbacks
    std::function<void(SpiTransaction& t)> pre_cb;
    std::function<void(SpiTransaction& t)> post_cb;
    std::function<void()> onAquire;
    std::function<void()> onRelease;
    bool hasBus = false;
    void aquire_bus_impl();
    void release_bus_impl();
    hal_spi_err_t transfer_impl(const SpiTransaction& transaction, uint32_t timeout = 0);
};
