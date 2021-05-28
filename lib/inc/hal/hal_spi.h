#pragma once

#include <functional>
#include <stdint.h>
#include <vector>

#include "hal_spi_impl.hpp"
#include "hal_spi_types.h"
// using namespace platform_spi;
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


    hal_spi_err_t write(const std::vector<uint8_t>& values,bool dma=false, std::function<void(TransactionData&)> pre = {}, std::function<void(TransactionData&)> post = {})
    {
        return platform_spi::write(values.data(), values.size(), dma, pre, post, SpiDataType::POINTER);
    }


    template <size_t N, std::enable_if_t<N <= 4, int> = 0>
    hal_spi_err_t write(const std::array<uint8_t, N>& values, bool dma=false, std::function<void(TransactionData&)> pre = {}, std::function<void(TransactionData&)> post = {})
    {
        return platform_spi::write(values.data(), values.size(), dma, pre, post, SpiDataType::VALUE);
    }

    template <size_t N, std::enable_if_t<(N > 4), int> = 0>
    hal_spi_err_t write(const std::array<uint8_t, N>& values, bool dma=false, std::function<void(TransactionData&)> pre = {}, std::function<void(TransactionData&)> post = {})
    {
        return platform_spi::write(values.data(), values.size(), dma, pre, post, SpiDataType::POINTER);
    }

    template <size_t N>
    hal_spi_err_t write_and_read(
        const std::array<uint8_t, N>& toDevice,
        std::array<uint8_t, N>& fromDevice)
    {
        // To implement
        return 0;
    }

    // data is pointer to data that should not be destructed
    hal_spi_err_t write(const uint8_t* data, size_t size, bool dma = false,std::function<void(TransactionData&)> pre = {}, std::function<void(TransactionData&)> post = {})
    {
        return platform_spi::write(data, size, dma, pre, post, SpiDataType::POINTER);
    }

    // single byte transer, store in pointer location
    hal_spi_err_t write(uint8_t value, bool dma = false, std::function<void(TransactionData&)> pre = {}, std::function<void(TransactionData&)> post = {})
    {
        return platform_spi::write(value, 1, dma, pre, post, SpiDataType::VALUE);
    }

    hal_spi_err_t write(const std::vector<uint8_t>& values)
    {
        return platform_spi::write(values.data(), values.size(),false, {},{}, SpiDataType::POINTER);
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
hal_spi_err_t hal_spi_host_init(uint8_t idx);