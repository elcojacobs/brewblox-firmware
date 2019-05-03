/*
 * Copyright 2019 BrewPi/Elco Jacobs.
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

#include "ActuatorDigital.h"
#include <inttypes.h>
#include <vector>

/*
 * Abstract interface to an array of digital inputs and/or outputs
 */
class IoArray {
public:
    IoArray(size_t size)
        : channelConfigs(size)
        , channelStates(size)
    {
    }

    virtual ~IoArray() = default;

    enum class ChannelConfig {
        DISABLED = 0,
        ACTIVE_LOW = 1,
        ACTIVE_HIGH = 2,
        INPUT = 10,
        INPUT_PULLUP = 11,
        INPUT_PULLDOWN = 12,
        UNKNOWN = 255,
    };

    // returns the chached value for the pin state. The
    bool senseChannel(uint8_t channel, ActuatorDigital::State& result) const
    {
        // first channel is 1, because 0 is used as 'unconfigured'
        if (channel > 0 && channel < channelStates.size()) {
            if (senseChannelImpl(channel, channelStates[channel - 1])) {
                result = channelStates[channel - 1];
                return true;
            }
        }
        return false;
    };

    // returns cached value for channel config, which is assumed to be equal to the last write
    bool readChannel(uint8_t channel, ChannelConfig& result) const
    {
        // first channel on external interface is 1, because 0 is unconfigured
        if (channel > 0 && channel < channelConfigs.size()) {
            result = channelConfigs[channel - 1];
            return true;
        }
        return false;
    }
    virtual bool writeChannel(uint8_t channel, const ChannelConfig& config)
    {
        // first channel on external interface is 1, because 0 is unconfigured
        if (channel > 0 && channel < channelConfigs.size()) {
            if (writeChannelImpl(channel, config)) {
                channelConfigs[channel - 1] = config;
                return true;
            }
        }
        return false;
    }

    std::vector<ChannelConfig> readChannels()
    {
        return channelConfigs;
    }

    std::vector<ActuatorDigital::State> senseChannels()
    {
        return channelStates;
    }

protected:
    // virtual functions to be implemented by super class that perform actual hardware IO.
    // values are cached in this class.

    // the super class can choose to apply/read immediately or to implement no-op functions and sync in an update function
    virtual bool senseChannelImpl(uint8_t channel, ActuatorDigital::State& result) const = 0;
    virtual bool writeChannelImpl(uint8_t channel, const ChannelConfig& config) = 0;

    mutable std::vector<ChannelConfig> channelConfigs;
    mutable std::vector<ActuatorDigital::State> channelStates;
};