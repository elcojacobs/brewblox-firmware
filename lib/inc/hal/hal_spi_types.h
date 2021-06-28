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
#include "SlotMemPool.hpp"
#include <cstddef>
#include <functional>
#include <stdint.h>

namespace spi {

extern SlotMemPool<20, 10> callBackArgsBuffer;

using error_t = int32_t;

/// An struct containing the data of a transaction.
struct TransactionData {
    const uint8_t* tx_data = nullptr;
    uint8_t* rx_data = nullptr;
    size_t tx_len = 0;
    size_t rx_len = 0;
};

struct CallbacksBase {
    virtual void callPre(TransactionData&) = 0;
    virtual void callPost(TransactionData&) = 0;
};


/// A helper struct to combine the pre and post condition into one object.
template <typename Pre, typename Post>
struct Callbacks : public CallbacksBase {
    Callbacks(const Pre& pre, const Post& post)
        : pre(pre)
        , post(post)
    {
    }
    Callbacks(Pre&& pre, Post&& post)
        : pre(std::move(pre))
        , post(std::move(post))
    {
    }
    Pre pre;
    Post post;

    void callPre(TransactionData& t) override final
    {
        if constexpr (!std::is_same<Pre, nullptr_t>::value) {
            pre(t);
        }
    }
    void callPost(TransactionData& t) override final
    {
        if constexpr (!std::is_same<Post, nullptr_t>::value) {
            post(t);
        }
    }
};

template <typename Pre, typename Post>
struct StaticCallbacks : public CallbacksBase {

    StaticCallbacks(Pre pre, Post post)
        : pre(pre)
        , post(post)
    {
    }

    Pre& pre;
    Post& post;

    void callPre(TransactionData& t) override final
    {
        if constexpr (!std::is_same<Pre, nullptr_t>::value) {
            pre(t);
        }
    }
    void callPost(TransactionData& t) override final
    {
        if constexpr (!std::is_same<Post, nullptr_t>::value) {
            post(t);
        }
    }
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
