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
#include <stdint.h>
#include <vector>

#include "hal_spi_impl.hpp"
#include "hal_spi_types.h"
using namespace spi;

struct SpiDevice {
    SpiDevice(uint8_t host_idx, int speed_hz, int queue_size, int ss_pin,
              Settings::Mode spi_mode, Settings::BitOrder bit_order,
              std::function<void()> on_aquire = {}, std::function<void()> on_release = {})
        : settings{.spi_idx = host_idx,
                   .speed = speed_hz,
                   .queueSize = queue_size,
                   .ssPin = ss_pin,
                   .mode = spi_mode,
                   .bitOrder = bit_order,
                   .on_Aquire = on_aquire,
                   .on_Release = on_release}

    {
    }

    ~SpiDevice()
    {
        deinit();
    }

    hal_spi_err_t init()
    {
        return platform_spi::init(settings);
    }
    void deinit()
    {
        platform_spi::deInit(settings);
    }

    hal_spi_err_t write(const std::vector<uint8_t>& values, bool dma = false, std::function<void(TransactionData&)> pre = nullptr, std::function<void(TransactionData&)> post = nullptr)
    {
        return platform_spi::write(settings, values.data(), values.size(), dma, pre, post, SpiDataType::POINTER);
    }

    template <size_t N, std::enable_if_t<N <= 4, int> = 0>
    hal_spi_err_t write(const std::array<uint8_t, N>& values, bool dma = false, std::function<void(TransactionData&)> pre = nullptr, std::function<void(TransactionData&)> post = nullptr)
    {
        return platform_spi::write(settings, values.data(), values.size(), dma, pre, post, SpiDataType::VALUE);
    }

    template <size_t N, std::enable_if_t<(N > 4), int> = 0>
    hal_spi_err_t write(const std::array<uint8_t, N>& values, bool dma = false, std::function<void(TransactionData&)> pre = nullptr, std::function<void(TransactionData&)> post = nullptr)
    {
        return platform_spi::write(settings, values.data(), values.size(), dma, pre, post, SpiDataType::POINTER);
    }

    template <size_t N>
    hal_spi_err_t write_and_read(
        const std::array<uint8_t, N>& toDevice,
        std::array<uint8_t, N>& fromDevice)
    {
        return platform_spi::writeAndRead(settings, toDevice.data(), N, fromDevice.data(), N, nullptr, nullptr, SpiDataType::POINTER);
    }

    // data is pointer to data that should not be destructed
    hal_spi_err_t write(const uint8_t* data, size_t size, bool dma = false, std::function<void(TransactionData&)> pre = nullptr, std::function<void(TransactionData&)> post = nullptr)
    {
        return platform_spi::write(settings, data, size, dma, pre, post, SpiDataType::POINTER);
    }

    // single byte transer, store in pointer location
    hal_spi_err_t write(uint8_t value, std::function<void(TransactionData&)> pre = nullptr, std::function<void(TransactionData&)> post = nullptr)
    {
        auto allocatedValue = uint8_t(value);
        return platform_spi::write(settings, &allocatedValue, 1, false, pre, post, SpiDataType::POINTER);
    }

    hal_spi_err_t write(const std::vector<uint8_t>& values)
    {
        return platform_spi::write(settings, values.data(), values.size(), false, nullptr, nullptr, SpiDataType::POINTER);
    }

    void aquire_bus()
    {
        // To implement
    }

    void release_bus()
    {
        // To implement
    }

private:
    Settings settings;
};
hal_spi_err_t hal_spi_host_init(uint8_t idx);