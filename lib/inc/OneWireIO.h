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
class OneWireIO : public OneWireDevice {
public:
    OneWireIO(OneWire& oneWire, OneWireAddress address = 0)
        : OneWireDevice(oneWire, address)
    {
    }
    virtual ~OneWireIO() = default;

    virtual bool sensePin(uint8_t channel, bool& isHigh) const = 0;
    virtual bool readLatch(uint8_t channel, bool& isEnabled) const = 0;
    virtual bool writeLatch(uint8_t channel, bool enabled) = 0;

    /**
     * @return true if connected (hardware DS2408 is found)
     */
    bool
    connected() const
    {
        return m_connected;
    }

    uint8_t
    claimed() const
    {
        return m_claimed;
    }

    bool
    claim(const uint8_t& v)
    {
        if ((m_claimed & v) == 0) {
            m_claimed |= v;
            return true;
        }
        return false;
    }

    void
    unclaim(const uint8_t& v)
    {
        m_claimed &= ~v;
    }

protected:
    mutable bool m_connected = false;

private:
    uint8_t m_claimed = 0x00;
};