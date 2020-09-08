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

#include "SparkIoBase.h"
#include "ActuatorDigital.h"
#include "Board.h"

// generic ArrayIO interface
bool
SparkIoBase::senseChannelImpl(uint8_t channel, State& result) const
{
    auto pin = channelToPin(channel);
    if (pin != static_cast<decltype(pin)>(-1)) {
        result = pinReadFast(pin) != 0 ? State::Active : State::Inactive;
        return true;
    }
    return false;
}

bool
SparkIoBase::writeChannelImpl(uint8_t channel, ChannelConfig config)
{
    auto pin = channelToPin(channel);
    if (pin != static_cast<decltype(pin)>(-1)) {
#ifdef PIN_V3_TOP1_DIR
        if (pin == PIN_V3_TOP1) {
            bool isOutput = (config == ChannelConfig::ACTIVE_HIGH || config == ChannelConfig::ACTIVE_LOW);
            HAL_Pin_Mode(PIN_V3_TOP1_DIR, OUTPUT);
            digitalWriteFast(PIN_V3_TOP1_DIR, isOutput);
        }
#endif
#ifdef PIN_V3_TOP2_DIR
        if (pin == PIN_V3_TOP2) {
            bool isOutput = (config == ChannelConfig::ACTIVE_HIGH || config == ChannelConfig::ACTIVE_LOW);
            HAL_Pin_Mode(PIN_V3_TOP2_DIR, OUTPUT);
            digitalWriteFast(PIN_V3_TOP2_DIR, isOutput);
        }
#endif
        switch (config) {
        case ChannelConfig::ACTIVE_HIGH:
            HAL_Pin_Mode(pin, OUTPUT);
            digitalWriteFast(pin, true);
            break;
        case ChannelConfig::ACTIVE_LOW:
            HAL_Pin_Mode(pin, OUTPUT);
            digitalWriteFast(pin, false);
            break;
        case ChannelConfig::INPUT:
        case ChannelConfig::UNUSED:
        case ChannelConfig::UNKNOWN:
            HAL_Pin_Mode(pin, INPUT_PULLDOWN);
            break;
        }
        return true;
    }
    return false;
}
