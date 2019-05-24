/*
 * Copyright 2019 Elco Jacobs
 *
 * This file is part of BrewBlox.
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

#include <cstdint>

#include "IoArray.h"

/**
 * Bundles IO pins of the microcontroller into an array object.
 */
class IoPins : public IoArray {
public:
    IoPins(), IoArray(5)
    {
        m_regCache.pio = 0xFF;
        m_regCache.latch = 0xFF;
    }

    /**
     * Destructor is default.
     */
    ~IoPins() = default;

    void update() const;

    // generic OneWireIO interface
    virtual bool senseChannelImpl(uint8_t channel, ActuatorDigital::State& result) const override final
    {
        // TODO
        if (connected() && channel >= 1 && channel <= 8) {
            result = ActuatorDigital::State::Unknown;
            return true; // valid channel
        }
        return false;
    }

    virtual bool writeChannelImpl(uint8_t channel, const ChannelConfig& config) override final
    {
        if (channel >= 1 && channel <= 5) {
            bool latchEnabled = config != ChannelConfig::ACTIVE_HIGH;
            return writeLatchBit(channel - 1, latchEnabled);
        }
        return false;
    }
};
