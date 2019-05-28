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

#pragma once

#include "IoArray.h"
#include "proto/cpp/IoArray.pb.h"

inline void
writeIoConfig(IoArray& device, uint8_t chan, const blox_IoChannel_ChannelConfig& v)
{
    device.writeChannelConfig(1, IoArray::ChannelConfig(v));
}

inline void
readIoConfig(const IoArray& device, uint8_t chan, blox_IoChannel_ChannelConfig& result)
{
    auto res = IoArray::ChannelConfig::UNKNOWN;
    device.readChannelConfig(1, res);
    result = blox_IoChannel_ChannelConfig(res);
}
