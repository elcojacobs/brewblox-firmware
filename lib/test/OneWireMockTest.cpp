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

#include <catch.hpp>

#include "../inc/OneWireMockDevice.h"
#include "../inc/OneWireMockDriver.h"
#include "DS18B20mock.h"
#include "TempSensorOneWire.h"

#include <math.h>

SCENARIO("A mocked OneWire bus", "[onewiremock]")
{
    OneWireMockDriver owMock;
    OneWire ow(owMock);

    WHEN("No devices are on the bus, reset returns false")
    {
        CHECK(ow.reset() == false);
    }

    WHEN("A mock DS18B20 is attached")
    {
        auto mockSensor = std::make_shared<DS18B20Mock>(0x0011223344556628);
        owMock.attach(mockSensor);
        THEN("Reset returns a presence")
        {
            CHECK(ow.reset() == true);
        }

        THEN("It can be found with a bus search")
        {
            OneWireAddress addr(0);
            ow.reset();
            bool found = ow.search(addr.asUint8ptr());
            CHECK(found == true);
            CHECK(uint64_t(addr) == 0x0011223344556628);
        }

        THEN("It can be found by family code")
        {
            OneWireAddress addr(0);
            ow.reset();
            ow.target_search(0x28);
            bool found = ow.search(addr.asUint8ptr());
            CHECK(found == true);
            CHECK(uint64_t(addr) == 0x0011223344556628);
        }

        THEN("A OneWire sensor can use it on the fake bus")
        {
            TempSensorOneWire sensor(ow, 0x0011223344556628);
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
    }
}
