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

#include "OneWire.h"
#include "OneWireAddress.h"
#include "OneWireDevice.h"
#include "blox/DS2408Block.h"
#include "blox/DS2413Block.h"
#include "blox/TempSensorOneWireBlock.h"
#include "cbox/Object.h"
#include "cbox/ObjectContainer.h"
#include "cbox/ScanningFactory.h"
#include <memory>

/**
 * Simple mock factory that emulates object discovery
 * Normally, a scanning factory will scan some type of communication bus
 * This factory just has a list of candidates. If a LongIntObject with that value doesn't exist, it creates it.
 */
class OneWireScanningFactory : public cbox::ScanningFactory {
private:
    OneWire& bus;

public:
    OneWireScanningFactory(OneWire& ow)
        : bus(ow)
    {
        reset();
    }

    virtual ~OneWireScanningFactory() = default;

    virtual void reset() override
    {
        bus.reset_search();
    }

    virtual OneWireAddress next()
    {
        auto newAddr = OneWireAddress();
        if (bus.search(newAddr)) {
            return newAddr;
        }
        return 0;
    }

    virtual std::shared_ptr<cbox::Object> scan(cbox::ObjectContainer& objects) override final
    {
        while (true) {
            if (auto newAddr = next()) {
                bool found = false;
                for (auto existing = objects.cbegin(); existing != objects.cend(); ++existing) {
                    OneWireDevice* ptrIfCorrectType = reinterpret_cast<OneWireDevice*>(existing->object()->implements(cbox::interfaceId<OneWireDevice>()));
                    if (ptrIfCorrectType == nullptr) {
                        continue; // not the right type, no match
                    }
                    if (ptrIfCorrectType->address() == newAddr) {
                        found = true; // object with value already exists
                        break;
                    }
                }
                if (!found) {
                    // create new object
                    uint8_t familyCode = newAddr[0];
                    switch (familyCode) {
                    case DS18B20::familyCode: {
                        auto newSensor = std::make_shared<TempSensorOneWireBlock>(bus);
                        newSensor->get().address(newAddr);
                        return newSensor;
                    }
                    case DS2413::familyCode: {
                        auto newDevice = std::make_shared<DS2413Block>(bus);
                        newDevice->get().address(newAddr);
                        return newDevice;
                    }
                    case DS2408::familyCode: {
                        auto newDevice = std::make_shared<DS2408Block>(bus);
                        newDevice->get().address(newAddr);
                        return newDevice;
                    }
                    default:
                        break;
                    }
                }
            } else {
                break;
            }
        };
        return nullptr;
    }
};
