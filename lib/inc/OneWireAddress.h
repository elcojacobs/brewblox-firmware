/*
 * Copyright 2018 BrewPi B.V.
 *
 * This file is part of the BrewBlox Control Library.
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
#include <string>

class OneWireAddress {
public:
    OneWireAddress()
        : address(0)
    {
    }
    OneWireAddress(uint64_t addr)
        : address(addr)
    {
    }
    ~OneWireAddress() = default;

    const uint8_t* asUint8ptr() const
    { // for compatibility with OneWire classes that take a uint8_t *
        return reinterpret_cast<const uint8_t*>(&address);
    }

    uint8_t* asUint8ptr()
    { // for compatibility with OneWire classes that take a uint8_t *
        return reinterpret_cast<uint8_t*>(&address);
    }

    bool operator==(const OneWireAddress& other) const
    {
        return address == other.address;
    }

    const uint8_t& operator[](uint8_t i) const
    {
        return asUint8ptr()[i];
    }

    uint8_t& operator[](uint8_t i)
    {
        return asUint8ptr()[i];
    }

    bool getBit(uint8_t i)
    {
        uint64_t mask = uint64_t{0x01} << i;
        return (mask & uint64_t(address)) > 0;
    }

    void setBit(uint8_t i, bool val)
    {
        uint64_t mask = uint64_t{0x01} << i;
        if (val) {
            address |= mask;
        } else {
            address &= ~mask;
        }
    }

    bool valid() const;

    std::string toString() const;

private:
    uint64_t address;
};
