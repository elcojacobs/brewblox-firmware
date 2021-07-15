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

/**
 * An abstraction for an spi device. It will call the platform spi functions.
 *
 * All pointer passed to the spidevice functions will need to be deallocated by the user.
 */
struct SpiDevice {

    /**
    * Constructs a SpiDevice object..
    * 
    * @param host_idx The spi peripheral to use.
    * @param speed_hz The speed of the spi bus in Hz.
    * @param queue_size The size of the queue for queuing transactions for dma transfer.
    * @param ss_pin The slave select pin.
    * @param spi_mode The configuration of the spi device.
    * @param bit_order The bitorder of a spi transaction.
    * @param on_aquire Gets called when te bus is aquired.
    * @param on_release Gets called when te bus is released.
    */
    SpiDevice(spi::Settings&& settings)
        : settings{settings}
    {
    }

    ~SpiDevice()
    {
        deinit();
    }

    /// Initialises the spi device with the settings defined in the constructor.
    spi::error_t init()
    {
        return platform_spi::init(settings);
    }

    /// Deinitialises the spi device.
    void deinit()
    {
        platform_spi::deInit(settings);
    }

    /**
    * Writes a std::vector of bytes over the spi bus.
    * 
    * This function will block until the transfer is finished.
    * 
    * @param values A std::vector of bytes to be send over the bus.
    * @return If any error will occur a non zero result will indicate an error has happened.
    */
    spi::error_t write(const std::vector<uint8_t>& values)
    {
        return platform_spi::write(settings, values.data(), values.size());
    }

    /** 
    * Writes and reads from the spi bus.
    * 
    * @param toDevice An array containing the bytes to be send.
    * @param fromDevice An array where the received bytes can be stored.
    * @return If any error will occur a non zero result will indicate an error has happened.
    */
    template <size_t N>
    spi::error_t write_and_read(
        const std::array<uint8_t, N>& toDevice,
        std::array<uint8_t, N>& fromDevice)
    {
        return platform_spi::writeAndRead(settings, toDevice.data(), N, fromDevice.data(), N);
    }

    /**
    * Writes the data at the address at a given pointer over the spi bus.
    *
    * The caller will be responsible for deallocating the data pointer.
    * This function will block until the transfer is finished.
    * 
    * @param data A pointer pointing to the beginning of the data to be send.
    * @param size The amount of bytes to be send.
    * @return If any error has occurred a non zero result will indicate an error has happened.
    */
    spi::error_t write(const uint8_t* data, size_t size)
    {
        return platform_spi::write(settings, data, size);
    }

    /**
    * Writes a single byte to the SPI bus. 
    * 
    * This function will block until the transfer is finished.
    *
    * @param value The value to be written to the bus.
    * @return If any error has occurred a non zero result will indicate an error has happened.
    */
    spi::error_t write(uint8_t value)
    {
        return platform_spi::write(settings, &value, 1);
    }

    /**
    * Writes the data at the address at a given pointer over the spi bus asynchronously using dma.
    *
    * The caller will be responsible for deallocating the data pointer. One way to do this is to perform deallocation in the post function.
    * 
    * @param data A pointer pointing to the beginning of the data to be send.
    * @param size The amount of bytes to be send.
    * @param callbacks The callbacks to be called before and after the transaction. 
    * @return If any error has occurred a non zero result will indicate an error has happened.
    */
    template <typename Pre, typename Post>
    spi::error_t dmaWrite(const uint8_t* data, size_t size, const spi::StaticCallbacks<Pre, Post>& callbacks)
    {
        return platform_spi::dmaWrite(settings, data, size, static_cast<const spi::CallbacksBase*>(&callbacks));
    }

    /**
    * Writes the data at the address at a given pointer over the spi bus asynchronously using dma.
    *
    * The caller will be responsible for deallocating the data pointer. One way to do this is to perform deallocation in the post function.
    * 
    * @param data A pointer pointing to the beginning of the data to be send.
    * @param size The amount of bytes to be send.
    * @param callbacks The callbacks to be called before and after the transaction. 
    * @return If any error has occurred a non zero result will indicate an error has happened.
    */
    template <typename Pre, typename Post>
    spi::error_t dmaWrite(const uint8_t* data, size_t size, const spi::Callbacks<Pre, Post>& callbacks)
    {
        auto callbacksToSend = new (spi::callBackArgsBuffer.get<spi::Callbacks<Pre, Post>>()) spi::Callbacks<Pre, Post>(callbacks);
        return platform_spi::dmaWrite(settings, data, size, static_cast<spi::CallbacksBase*>(callbacksToSend));
    }

    /**
    * Writes the data at the address at a given pointer over the spi bus asynchronously using dma.
    *
    * The caller will be responsible for deallocating the data pointer. One way to do this is to perform deallocation in the post function.
    * 
    * @param data A pointer pointing to the beginning of the data to be send.
    * @param size The amount of bytes to be send.
    * @param callbacks The callbacks to be called before and after the transaction. 
    * @return If any error has occurred a non zero result will indicate an error has happened.
    */
    template <typename Pre, typename Post>
    spi::error_t dmaWrite(const uint8_t* data, size_t size, spi::Callbacks<Pre, Post>&& callbacks)
    {
        auto callbacksToSend = new (spi::callBackArgsBuffer.get<spi::Callbacks<Pre, Post>>()) spi::Callbacks<Pre, Post>(callbacks);
        return platform_spi::dmaWrite(settings, data, size, static_cast<spi::CallbacksBase*>(callbacksToSend));
    }

    void aquire_bus()
    {
        platform_spi::aquire_bus(this->settings);
        if (settings.on_Aquire) {
            settings.on_Aquire();
        }
    }

    void release_bus()
    {
        platform_spi::release_bus(this->settings);
        if (settings.on_Release) {
            settings.on_Release();
        }
    }

private:
    spi::Settings settings;
};
spi::error_t hal_spi_host_init(uint8_t idx);
