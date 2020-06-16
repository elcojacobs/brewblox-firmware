/*
 * Copyright 2013 Matthew McGowan
 * Copyright 2013 BrewPi/Elco Jacobs.
 *
 * This file is part of BrewPi.
 *
 * BrewPi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BrewPi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BrewPi.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "IoArray.h"
#include "Logger.h"
#include "OneWireDevice.h"
#include <inttypes.h>

#define DS2413_FAMILY_ID 0x3A

/*
 * Provides access to a OneWire-addressable dual-channel I/O device.
 * The channel latch can be set to on (false) or off (true).
 * When a channel is off (PIOx=1), the channel state can be sensed. This is the power on-default.
 *
 * channelRead/channelWrite reads and writes the channel latch state to turn the output transistor on or off
 * channelSense senses if the channel is pulled high.
 */
class DS2413 : public OneWireDevice, public IoArray {
private:
    uint8_t actualState = 0b1111;
    uint8_t desiredState = 0b1111;

    static const uint8_t ACCESS_READ = 0xF5;
    static const uint8_t ACCESS_WRITE = 0x5A;
    static const uint8_t ACK_SUCCESS = 0xAA;
    static const uint8_t ACK_ERROR = 0xFF;

public:
    DS2413(OneWire& oneWire, OneWireAddress address = 0)
        : OneWireDevice(oneWire, address)
        , IoArray(2)
    {
    }

    DS2413(const DS2413&) = delete;
    DS2413&
    operator=(const DS2413&)
        = delete;

    /**
     * Destructor is default
     */
    virtual ~DS2413() = default;

    /**
     * Periodic update to make sure the cache is valid.
     * Performs a simultaneous read of both channels and saves value to the cache.
     * When read fails, prints a warning that the DS2413 is disconnected
     *
     * @return					true on successful communication
     */
    bool update();

    // generic ArrayIO interface
    virtual bool
    senseChannelImpl(uint8_t channel, State& result) const override final;

    virtual bool
    writeChannelImpl(uint8_t channel, const ChannelConfig& config) override final;

    virtual bool
    supportsFastIo() const override final
    {
        return false;
    }

private:
    bool writeNeeded();
    bool processStatus(uint8_t data);
};
