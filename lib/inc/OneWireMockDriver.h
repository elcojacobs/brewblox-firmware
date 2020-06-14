/*
 * Copyright 2018 BrewPi B.V.
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
#include "OneWireLowLevelInterface.h"
#include "OneWireMockDevice.h"
#include <deque>
#include <memory>
#include <vector>

class OneWireMockDriver : public OneWireLowLevelInterface {
private:
    std::deque<uint8_t> masterToSlave;
    std::deque<uint8_t> slaveToMaster;

public:
    OneWireMockDriver()
    {
    }
    virtual ~OneWireMockDriver() = default;

public:
    virtual bool init() override final
    {
        return true;
    }

    virtual uint8_t read() override final
    {
        if (!masterToSlave.empty()) {
            // first process all outgoing bytes
            communicate();
        }
        uint8_t retv = 0;
        if (!slaveToMaster.empty()) {
            retv = slaveToMaster.front();
            slaveToMaster.pop_front();
        }
        return retv;
    }

    virtual void write(uint8_t b, uint8_t power = 0) override final
    {
        masterToSlave.push_back(b);
    }

    void write_bytes(const uint8_t* buf, uint16_t count)
    {
        for (uint16_t i = 0; i < count; i++) {
            write(buf[i]);
        }
    }

    void read_bytes(uint8_t* buf, uint16_t count)
    {
        for (uint16_t i = 0; i < count; i++) {
            buf[i] = read();
        }
    }

    void peek_recv(uint8_t* buf, uint16_t count, uint16_t start = 0)
    {
        for (uint16_t i = 0; i < count; i++) {
            buf[i] = masterToSlave.at(start + i);
        }
    }

    void send(uint8_t* buf, uint16_t count)
    {
        for (uint16_t i = 0; i < count; i++) {
            slaveToMaster.push_back(buf[i]);
        }
    }

    virtual uint8_t search_triplet(uint8_t* search_direction, uint8_t* id_bit, uint8_t* cmp_id_bit) override final;

    virtual void write_bit(uint8_t bit) override final
    {
        write(bit ? 0x80 : 0x00);
    }
    virtual uint8_t read_bit() override final
    {
        return read() ? 0x01 : 0x00;
    }

    virtual bool reset() override final
    {

        for (auto& device : devices) {
            device->reset();
        }
        return devices.size() > 0;
    }

    void attach(std::shared_ptr<OneWireMockDevice> device)
    {
        devices.push_back(std::move(device));
    }

private:
    void communicate();
    uint8_t countActiveDevices();

    std::vector<std::shared_ptr<OneWireMockDevice>> devices;
};
