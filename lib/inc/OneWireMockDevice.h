/*
 * Copyright 2020 BrewPi B.V.
 *
 * This file is part of the BrewBlox Control Library.
 *
 * Brewblox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Brewblox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Brewblox.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "OneWireAddress.h"
class OneWireMockDriver;

class OneWireMockDevice {
public:
    OneWireMockDevice(const OneWireAddress& address_)
        : address(address_)
    {
    }

protected:
    ~OneWireMockDevice() = default;

public:
    uint8_t respond(OneWireMockDriver& mock);

    virtual uint8_t respondImpl(OneWireMockDriver& mock) = 0;

    bool match(const OneWireAddress& a)
    {
        return uint64_t(a) == uint64_t(address);
    }

    bool getAddressBit();

    void search_triplet_read(bool* id_bit, bool* cmp_id_bit);

    void search_triplet_write(bool bit);

    void reset()
    {
        dropped = false;
    }

    bool present()
    {
        return !dropped;
    }

protected:
    OneWireAddress address;
    mutable bool m_connected = true;
    bool dropped = true;
    bool parasite = false;
    uint8_t search_bitnr = 0;
};
