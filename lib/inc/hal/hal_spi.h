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
#include <mutex>

class SpiConfig {
public:
    SpiConfig()
    {
    }
    ~SpiConfig() = default;

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

    uint8_t host = 0; // index to select SPI master in case of multiple masters
    int speed = 0;
    int queueSize = 1;
    int ssPin = -1;

    Mode mode = SPI_MODE0;
    BitOrder bitOrder = MSBFIRST;
};

// platform dependent implementation of transfer functions
typedef void (*spi_dma_transfercomplete_callback_t)(void);
uint8_t transferImpl(const SpiConfig& client, uint8_t data);
void transferDmaImpl(const SpiConfig& client, void* tx_buffer, void* rx_buffer, size_t length, spi_dma_transfercomplete_callback_t user_callback);
void transferDmaCancelImpl(const SpiConfig& client);
void apply(const SpiConfig& client);
void unapply(const SpiConfig& client);
bool spi_device_init(const SpiConfig& client);

#if 0
class SpiBus {
    SpiConfig activeClient;

    std::mutex mutex;

    void unapply();
    void apply(SpiConfig& client);

public:
    SpiBus()
        : current_(nullptr)
    {
    }

    ~SpiBus()
    {
    }

    inline bool try_begin(SpiConfig& client)
    {
        if (isClient(client) || mutex.try_lock()) {
            activeClient = client;
            apply(activeClient);
        }
        return isClient(client);
    }

    inline void begin(SpiConfig& client)
    {
        mutex.lock();
        activeClient = client;
        apply(activeClient);
    }

    inline void end(SpiConfig& client)
    {
        if (isClient(client)) {
            unapply();
            mutex.unlock();
        }
    }

    inline bool isClient(SpiConfig& client)
    {
        return client.ssPin == activeClient.ssPin;
    }

    /*
     * transfer assumes the SPI has already been acquired by calling begin()
     */
    inline uint8_t transfer(const SpiConfig& client, uint8_t data)
    {
        if (isClient(client)) {
            return transferImpl(client, data);
        }
        return 0;
    }

    inline void transfer(const SpiConfig& client, void* tx_buffer, void* rx_buffer, size_t length, spi_dma_transfercomplete_callback_t user_callback)
    {
        if (isClient(client)) {
            transferDmaImpl(client, tx_buffer, rx_buffer, length, user_callback);
        }
    }

    inline void transferCancel(const SpiConfig& client)
    {
        if (isClient(client)) {
            transferDmaCancelImpl(client);
        }
    }
};

class SPIUser : public SpiConfig {
    SpiBus& spi_;

public:
    SPIUser(SpiBus& spi)
        : spi_(spi)
    {
    }

    inline void begin()
    {
        spi_.begin(*this);
    }

    inline void begin(uint16_t ss_pin)
    {
        this.ssPin = ss_pin;
        spi_.begin(*this);
    }

    inline void end()
    {
        spi_.end(*this);
    }

    inline void setBitOrder(BitOrder v)
    {
        this.bitOrder = v;
    }

    inline void setDataMode(Mode v)
    {
        this.mode = v;
    }

    /**
     * Sets the clock speed as a divider relative to the clock divider reference.
     * @param divider SPI_CLOCK_DIVx where x is a power of 2 from 2 to 256.
     */
    inline void setClockDivider(ClockDivider divider)
    {
        this.clockDivider = divider;
    }

    inline uint8_t transfer(uint8_t data)
    {
        return spi_.transfer(*this, data);
    }

    inline void transfer(void* tx_buffer, void* rx_buffer, size_t length, wiring_spi_dma_transfercomplete_callback_t user_callback)
    {
        spi_.transfer(*this, tx_buffer, rx_buffer, length, user_callback);
    }

    inline void transferCancel()
    {
        spi_.transferCancel(*this);
    }
};

SpiBus& SpiBus1();
#endif