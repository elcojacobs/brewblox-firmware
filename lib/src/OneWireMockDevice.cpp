/*
 * Copyright 2020 BrewPi B.V.
 *
 * This file is part of the Brewblox Control Library.
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
 * along with Brewblox.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "OneWireMockDevice.h"
#include "OneWireMockDriver.h"

uint8_t
OneWireMockDevice::respond(OneWireMockDriver& mock)
{
    if (dropped) {
        return 0;
    }
    uint8_t cmd = 0;
    mock.peek_recv(&cmd, 1);
    switch (cmd) {
    case 0x33: // Read ROM
    {
        mock.write_bytes(address.asUint8ptr(), 8);
    }
        return 9;

    case 0x55: // Match ROM
    case 0x69: // Overdrive Match
    {
        OneWireAddress selectedAddress;
        mock.peek_recv(selectedAddress.asUint8ptr(), 8, 1);
        dropped = !match(selectedAddress);
    }
        return 9;
    case 0xF0: // Search ROM
        search_bitnr = 0;
        return 1;
    case 0xCC: // Skip ROM
    case 0x3C: // Overdrive skip
    case 0xA5: // Resume
        return 1;
    default:
        // not a generic OneWire command, forward to device implementation
        return respondImpl(mock);
    }
    return 1;
}

bool
OneWireMockDevice::getAddressBit()
{
    uint64_t mask = uint64_t{0x01} << search_bitnr;
    return (mask & uint64_t(address)) > 0;
}

void
OneWireMockDevice::search_triplet_read(bool* id_bit, bool* cmp_id_bit)
{
    bool my_bit = getAddressBit();
    if (!dropped && my_bit) {
        *id_bit = false; // pulls down bit if match
    }
    if (!dropped && !my_bit) {
        *cmp_id_bit = false; // pulls down bit if match with complement
    }
}

void
OneWireMockDevice::search_triplet_write(bool bit)
{
    bool my_bit = getAddressBit();
    if (my_bit != bit) {
        dropped = true;
    }
    if (search_bitnr < 63) {
        search_bitnr++;
    }
}
