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
#include <cstddef>
#include <functional>
#include <stdint.h>

namespace spi {

/// An enum to designated if the data of a spi transaction will be used as value or pointer.
enum class SpiDataType {
    POINTER,
    VALUE,
};

using hal_spi_err_t = int32_t;

struct TransactionData {
    const uint8_t* tx_data = nullptr;
    uint8_t* rx_data = nullptr;
    size_t tx_len = 0;
    size_t rx_len = 0;
};

/// A helper struct to combine the pre and post condition into one object.
struct CallbackArg {
    std::function<void(TransactionData&)> pre;
    std::function<void(TransactionData&)> post;
};

/// A struct to transfer the settings of the spiDevice around.
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
    std::function<void()> on_Aquire;
    std::function<void()> on_Release;
    void* platform_device_ptr = nullptr;
};
}
