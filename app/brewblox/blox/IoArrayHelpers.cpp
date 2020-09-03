/*
 * Copyright 2018 BrewPi B.V.
 *
 * This file is part of BrewBlox
 *
 * BrewBlox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BrewBlox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "IoArrayHelpers.h"

void
writeIoConfig(IoArray& device, uint8_t chan, const blox_ChannelConfig& v)
{
    device.writeChannelConfig(chan, IoArray::ChannelConfig(v));
}

void
readIoConfig(const IoArray& device, uint8_t chan, blox_ChannelConfig& result)
{
    auto res = IoArray::ChannelConfig::UNKNOWN;
    device.readChannelConfig(chan, res);
    result = blox_ChannelConfig(res);
}

void
readIo(const IoArray& device, uint8_t chan, blox_IoChannel& result)
{
    auto config = IoArray::ChannelConfig::UNKNOWN;
    device.readChannelConfig(chan, config);
    result.config = blox_ChannelConfig(config);

    auto state = IoArray::State::Unknown;
    device.senseChannel(chan, state);
    result.state = _blox_DigitalState(state);
}