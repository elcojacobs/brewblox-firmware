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
#include <deque>
#include <vector>
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
    uint8_t read();
    void write(uint8_t b);
    bool reset();
    virtual void resetImpl()
    {
        // devices can override this for extra behavior at onewire reset
    }

    uint8_t recv();
    void recv(uint8_t* buf, uint16_t count);
    void send(uint8_t b);
    void send(uint8_t* buf, uint16_t count);

    void process();
    virtual void processImpl(uint8_t cmd) = 0;

    bool match(const OneWireAddress& a)
    {
        return a == address;
    }

    bool getAddressBit();

    void search_triplet_read(bool* id_bit, bool* cmp_id_bit);

    bool search_triplet_write(bool bit);

    bool present()
    {
        return !dropped;
    }

    // flip an upcoming bit in to test communication errors
    void flipWrittenBits(const std::vector<uint32_t>& positions)
    {
        positionsToMasks(positions, flippedWriteBits);
    }

    void flipReadBits(const std::vector<uint32_t>& positions)
    {
        positionsToMasks(positions, flippedReadBits);
    }

    void setConnected(bool v)
    {
        connected = v;
    }

private:
    void positionsToMasks(const std::vector<uint32_t>& positions, std::deque<uint8_t>& queue);

protected:
    OneWireAddress address;
    bool connected = true;
    bool dropped = true;

    bool selected = false;
    bool parasite = false;
    uint8_t search_bitnr = 0;
    std::deque<uint8_t> flippedWriteBits;
    std::deque<uint8_t> flippedReadBits;

private:
    std::deque<uint8_t> masterToSlave;
    std::deque<uint8_t> slaveToMaster;
};
