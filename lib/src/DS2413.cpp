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

#include "../inc/DS2413.h"
#include "../inc/Logger.h"
#include "../inc/OneWire.h"

bool
DS2413::update()
{

    bool success = false;
    if (!writeNeeded()) { // skip read if we need to write anyway, which also returns status
        selectRom();
        oneWire.write(ACCESS_READ);
        uint8_t data = oneWire.read();
        success = processStatus(data);
    }
    if (writeNeeded()) { // check again
        selectRom();
        oneWire.write(ACCESS_WRITE);
        uint8_t data = (desiredState & 0b1000) >> 2 | (desiredState & 0b0010) >> 1;
        oneWire.write(data);
        oneWire.write(~data); // write inverted for error checking
        data = oneWire.read();
        success = false;
        if (data == ACK_SUCCESS) {
            data = oneWire.read();
            success = processStatus(data);
        }
    }
    oneWire.reset();

    dirty = !success;

    if (success != m_connected) {
        if (success) {
            CL_LOG_INFO("DS2413 connected: ") << address.toString();
        } else {
            CL_LOG_WARN("DS2413 disconnected: ") << address.toString();
        }
        m_connected = success;
    }
    return success;
}

bool
DS2413::writeNeeded()
{
    return (desiredState & 0b1010) != (actualState & 0b1010);
}

bool
DS2413::writeChannelImpl(uint8_t channel, ChannelConfig config)
{
    bool latchEnabled = config == ChannelConfig::ACTIVE_HIGH;
    uint8_t bitmask;
    if (channel == 1) {
        bitmask = 0b0010;
    } else if (channel == 2) {
        bitmask = 0b1000;
    } else {
        return false;
    }

    if (latchEnabled) {
        desiredState &= ~bitmask;
    } else {
        desiredState |= bitmask;
    }
    if (writeNeeded()) {
        return update();
    }
    return true;
}

bool
DS2413::senseChannelImpl(uint8_t channel, State& result) const
{
    if (connected()) {
        // to reduce onewire communication, we assume the last read value in update() is correct
        // only in update(), actual onewire communication will take place to get the latest state
        if (channel == 1) {
            result = (actualState & 0b0001) == 0 ? State::Active : State::Inactive;
            return true;
        } else if (channel == 2) {
            result = (actualState & 0b0100) == 0 ? State::Active : State::Inactive;
            return true;
        }
    }
    result = State::Unknown;
    return false;
}

bool
DS2413::processStatus(uint8_t data)
{
    uint8_t newState = data & 0x0F;
    uint8_t verification = ((~data) >> 4) & 0xF;
    if (newState == verification) {
        actualState = newState;
        return true;
    }
    return false;
}