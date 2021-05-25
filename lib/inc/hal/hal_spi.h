#pragma once

#include <functional>
#include <stdint.h>
#include <vector>

#include "hal_spi_impl.hpp"
#include "hal_spi_types.h"
// using namespace platform_spi;
using namespace spi;
template <typename UserType = bool>
struct SpiDevice {

    SpiDevice(uint8_t host_idx, int speed_hz, int queue_size, int ss_pin,
              Settings::Mode spi_mode, Settings::BitOrder bit_order,
              std::function<void()> on_aquire = {}, std::function<void()> on_release = {},
              std::function<void(Transaction&)> pre = {}, std::function<void(Transaction&)> post = {})
        : settings{.spi_idx = host_idx,
                   .speed = speed_hz,
                   .queueSize = queue_size,
                   .ssPin = ss_pin,
                   .mode = spi_mode,
                   .bitOrder = bit_order,
                   .pre_cb = pre,
                   .post_cb = post,
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

    void set_user(Transaction& t, nullptr_t)
    {
        // To implement
    }

    void set_user(Transaction& t, UserType&& userData)
    {
        // To implement
    }

    // without DMA, vector will not be destructed during transfer
    hal_spi_err_t write(const std::vector<uint8_t>& values, UserType userData)
    {
        return platform_spi::write(values.data(), values.size(), reinterpret_cast<void*>(userData), false, SpiDataType::POINTER);
    }

    // with of without DMA, optimized for small transfer under 4 bytes
    // copied to point address location, so no distruction needed
    template <size_t N, std::enable_if_t<N <= 4, int> = 0>
    hal_spi_err_t write(const std::array<uint8_t, N>& values, UserType userData, bool dma)
    {
        return platform_spi::write(values.data(), values.size(), reinterpret_cast<void*>(userData), dma, SpiDataType::VALUE);
    }

    template <size_t N, std::enable_if_t<N <= 4, int> = 0>
    hal_spi_err_t write(const std::array<uint8_t, N>& values)
    {
        return platform_spi::write(values.data(), values.size(), nullptr, false, SpiDataType::POINTER);
    }

    // without DMA, inputs are valid during transfer
    template <size_t N>
    hal_spi_err_t write_and_read(
        const std::array<uint8_t, N>& toDevice,
        std::array<uint8_t, N>& fromDevice)
    {
        // To implement
        return 0;
    }

    // data is pointer to data that should not be destructed
    hal_spi_err_t write(const uint8_t* data, size_t size, UserType userData, bool dma = false)
    {
        return platform_spi::write(data, size, reinterpret_cast<void*>(userData), dma, SpiDataType::POINTER);
    }

    // single byte transer, store in pointer location
    hal_spi_err_t write(uint8_t value, UserType userData, bool dma = false)
    {
        return platform_spi::write(value, 1, reinterpret_cast<void*>(userData), dma, SpiDataType::VALUE);
    }

    hal_spi_err_t write(const std::vector<uint8_t>& values)
    {
        return platform_spi::write(values.data(), values.size(), nullptr, false, SpiDataType::POINTER);
    }

    void aquire_bus()
    {
        // To implement
    }

    void release_bus()
    {
        // To implement
    }

    void do_pre_cb(Transaction& t)
    {
        // To implement
    }
    void do_post_cb(Transaction& t)
    {
        // To implement
    }

private:
    Settings settings;
};
template struct SpiDevice<>;
hal_spi_err_t hal_spi_host_init(uint8_t idx);