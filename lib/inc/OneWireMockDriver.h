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
#include <memory>
#include <vector>

class OneWireMockDriver : public OneWireLowLevelInterface {

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
        // if multiple devices are answering, the result is a binary AND
        // This will only give valid responses with single bit replies, just like the real hardware
        uint8_t b = 0xFF;
        for (auto& device : devices) {
            b &= device->read();
        }
        return b;
    }

    virtual void write(uint8_t b, uint8_t power = 0) override final
    {
        for (auto& device : devices) {
            device->write(b);
        }
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

        bool devicePresent = false;
        for (auto& device : devices) {
            devicePresent |= device->reset();
        }
        return devicePresent;
    }

    void attach(std::shared_ptr<OneWireMockDevice> device)
    {
        devices.push_back(std::move(device));
    }

private:
    uint8_t countActiveDevices();

    std::vector<std::shared_ptr<OneWireMockDevice>> devices;
};
