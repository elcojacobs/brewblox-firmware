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

    virtual bool read(uint8_t& v) override final
    {
        // if multiple devices are answering, the result is a binary AND
        // This will only give valid responses with single bit replies, just like the real hardware
        v = 0xFF;
        for (auto& device : devices) {
            v &= device->read();
        }
        return true;
    }

    virtual bool write(uint8_t b) override final
    {
        for (auto& device : devices) {
            device->write(b);
        }
        return true;
    }

    virtual uint8_t search_triplet(bool search_direction) override final;

    virtual bool write_bit(bool bit) override final
    {
        write(bit ? 0x80 : 0x00);
        return true;
    }
    virtual bool read_bit(bool& bit) override final
    {
        uint8_t v;
        if (read(v)) {
            bit = v > 0;
            return true;
        }
        return false;
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
    std::vector<std::shared_ptr<OneWireMockDevice>> devices;
};
