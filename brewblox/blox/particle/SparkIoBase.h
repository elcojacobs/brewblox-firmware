/*
 * Copyright 2019 BrewPi B.V.
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
#include "pinmap_hal.h" // pin_t defined here

class SparkIoBase : public IoArray {
protected:
    virtual pin_t channelToPin(uint8_t channel) const = 0;

public:
    SparkIoBase(uint8_t numPins)
        : IoArray(numPins)
    {
    }

    // generic ArrayIO interface
    virtual bool
    senseChannelImpl(uint8_t channel, State& result) const override final;
    virtual bool
    writeChannelImpl(uint8_t channel, ChannelConfig config) override final;
    virtual bool
    supportsFastIo() const override final
    {
        return true;
    }
};
