/*
 * Copyright 2020 BrewPi B.V.
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

#include <catch.hpp>

#include "../inc/OneWireMockDevice.h"
#include "../inc/OneWireMockDriver.h"
#include "DS18B20mock.h"
#include "DS2413.h"
#include "DS2413mock.h"
#include "TempSensorOneWire.h"

#include <math.h>

namespace Catch {
template <>
struct StringMaker<OneWireAddress> {
    static std::string convert(OneWireAddress const& value)
    {
        return value.toString();
    }
};
}

OneWireAddress
makeValidAddress(OneWireAddress addr)
{
    addr[7] = OneWire::crc8(&addr[0], 7);
    return addr;
}

SCENARIO("A mocked OneWire bus and DS18B20 sensor", "[onewire]")
{
    OneWireMockDriver owMock;
    OneWire ow(owMock);

    WHEN("No devices are on the bus, reset returns false")
    {
        CHECK(ow.reset() == false);
    }

    WHEN("A mock DS18B20 is attached")
    {
        auto addr1 = makeValidAddress(0x0011223344556628);
        auto mockSensor = std::make_shared<DS18B20Mock>(addr1);
        owMock.attach(mockSensor);
        THEN("Reset returns a presence")
        {
            CHECK(ow.reset() == true);
        }

        THEN("It can be found with a bus search")
        {
            OneWireAddress addr(0);
            ow.reset();
            bool found = ow.search(addr);
            CHECK(found == true);
            CHECK(addr == addr1);
        }

        THEN("A OneWire sensor can use it on the fake bus")
        {
            TempSensorOneWire sensor(ow, addr1);
            sensor.update();
            CHECK(sensor.valid() == false); // a reset will be detected, triggering a re-init
            sensor.update();
            CHECK(sensor.valid() == true);
            CHECK(sensor.value() == 10.0);

            mockSensor->setTemperature(temp_t{21.0});
            CHECK(mockSensor->getTemperature() == 21.0);
            sensor.update();
            CHECK(sensor.value() == 21.0);

            mockSensor->setTemperature(temp_t{-10.0});
            CHECK(mockSensor->getTemperature() == -10.0);
            sensor.update();
            CHECK(sensor.value() == -10.0);
        }

        AND_WHEN("Another sensor is connected")
        {

            auto addr2 = makeValidAddress(0x0099223344556628);
            auto mockSensor2 = std::make_shared<DS18B20Mock>(addr2);
            owMock.attach(mockSensor2);

            TempSensorOneWire sensor1(ow, addr1);
            TempSensorOneWire sensor2(ow, addr2);

            THEN("A bus search finds two sensors")
            {
                OneWireAddress addr(0);
                ow.reset();
                ow.reset_search();
                bool found = ow.search(addr);
                CHECK(found == true);
                CHECK(addr == OneWireAddress(addr1));

                found = ow.search(addr);
                CHECK(found == true);
                CHECK(addr == OneWireAddress(addr2));

                found = ow.search(addr);
                CHECK(found == false);
            }

            sensor1.update();
            sensor2.update();
            sensor1.update();
            sensor2.update();
            CHECK(sensor1.valid() == true);
            CHECK(sensor2.valid() == true);
            CHECK(sensor1.value() == 10.0);
            CHECK(sensor2.value() == 10.0);

            mockSensor->setTemperature(temp_t{21.0});
            mockSensor2->setTemperature(temp_t{22.0});
            CHECK(mockSensor->getTemperature() == 21.0);
            CHECK(mockSensor2->getTemperature() == 22.0);
            sensor1.update();
            sensor2.update();
            CHECK(sensor1.value() == 21.0);
            CHECK(sensor2.value() == 22.0);
        }
    }

    WHEN("A mock DS2413 is attached")
    {
        auto addr3 = makeValidAddress(0x002222334455663A);
        auto ds1mock = std::make_shared<DS2413Mock>(addr3);
        owMock.attach(ds1mock);
        THEN("Reset returns a presence")
        {
            CHECK(ow.reset() == true);
        }

        THEN("It can be found with a bus search")
        {
            OneWireAddress addr(0);
            ow.reset();
            bool found = ow.search(addr);
            CHECK(found == true);
            CHECK(addr == addr3);
        }

        THEN("A DS2413 class can use it on the fake bus")
        {
            DS2413 ds1(ow, addr3);

            ActuatorDigitalBase::State result;
            ds1.update();
            CHECK(ds1.connected() == true);
            CHECK(ds1.senseChannel(1, result));
            CHECK(result == ActuatorDigitalBase::State::Inactive);
            CHECK(ds1.senseChannel(2, result));
            CHECK(result == ActuatorDigitalBase::State::Inactive);

            // note that for IoArray ACTIVE_HIGH means OUTPUT and ACTIVE
            // It is actually an open drain pull down on in the DS2413
            // Should we rename this?
            CHECK(ds1.writeChannelConfig(1, IoArray::ChannelConfig::ACTIVE_HIGH));
            CHECK(ds1.senseChannel(1, result));
            CHECK(result == ActuatorDigitalBase::State::Active);

            CHECK(ds1.senseChannel(2, result));
            CHECK(result == ActuatorDigitalBase::State::Inactive);
        }
    }

    WHEN("Both a DS18B20 and DS2413 are connected")
    {
        auto addr1 = makeValidAddress(0x001122334455663A);
        auto ds1mock = std::make_shared<DS2413Mock>(addr1);
        owMock.attach(ds1mock);
        auto addr2 = makeValidAddress(0x0011223344556628);
        auto mockSensor = std::make_shared<DS18B20Mock>(addr2);
        owMock.attach(mockSensor);

        THEN("Then target search can determine which is found first")
        {
            OneWireAddress addr(0);
            ow.reset();
            ow.reset_search();
            ow.target_search(DS18B20Mock::family_code);
            bool found = ow.search(addr);
            CHECK(found == true);
            CHECK(addr == addr2);
            addr = 0;
            found = ow.search(addr);
            CHECK(found == false);
            CHECK(addr == 0);

            ow.reset();
            ow.reset_search();
            addr = 0;
            ow.target_search(DS2413Mock::family_code);
            found = ow.search(addr);
            CHECK(found == true);
            CHECK(addr == addr1);
            addr = 0;
            found = ow.search(addr);
            CHECK(found == false);
            CHECK(addr == 0);
        }
    }
}
