/*
 * Copyright 2018 BrewPi B.V.
 *
 * This file is part of BrewBlox.
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
#include <cstdio>
#include <iomanip>
#include <iostream>

#include "../BrewBlox.h"
#include "BrewBloxTestBox.h"
#include "Temperature.h"
#include "blox/TempSensorMockBlock.h"
#include "cbox/Box.h"
#include "cbox/DataStream.h"
#include "cbox/DataStreamIo.h"
#include "cbox/Object.h"
#include "proto/test/cpp/TempSensorMock_test.pb.h"
#include "testHelpers.h"

SCENARIO("A TempSensorMock block")
{
    BrewBloxTestBox testBox;
    using commands = cbox::Box::CommandID;

    testBox.reset();

    // create mock sensor
    testBox.put(uint16_t(0)); // msg id
    testBox.put(commands::CREATE_OBJECT);
    testBox.put(cbox::obj_id_t(100));
    testBox.put(uint8_t(0xFF));
    testBox.put(TempSensorMockBlock::staticTypeId());

    auto newSensor = blox::TempSensorMock();
    newSensor.set_setting(cnl::unwrap(temp_t(20.0)));
    newSensor.set_connected(true);
    testBox.put(newSensor);

    testBox.processInput();
    CHECK(testBox.lastReplyHasStatusOk());

    WHEN("The mock sensor value is changed to 25")
    {
        auto cboxPtr = brewbloxBox().makeCboxPtr<TempSensorMockBlock>(100);
        auto ptr = cboxPtr.lock();
        REQUIRE(ptr);

        ptr->get().setting(25);

        testBox.update(1000);

        THEN("The value that is read back is correct")
        {
            // read pair
            testBox.put(uint16_t(0)); // msg id
            testBox.put(commands::READ_OBJECT);
            testBox.put(cbox::obj_id_t(100));

            auto decoded = blox::TempSensorMock();
            testBox.processInputToProto(decoded);

            CHECK(testBox.lastReplyHasStatusOk());
            CHECK(decoded.ShortDebugString() == "value: 102400 connected: true setting: 102400");
        }
    }

    WHEN("Fluctuations are set")
    {

        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::WRITE_OBJECT);
        testBox.put(cbox::obj_id_t(100));
        testBox.put(uint8_t(0xFF));
        testBox.put(TempSensorMockBlock::staticTypeId());

        auto newSensor = blox::TempSensorMock();
        newSensor.set_setting(cnl::unwrap(temp_t(20.0)));
        newSensor.set_connected(true);

        {
            auto newFluct = newSensor.add_fluctuations();
            newFluct->set_amplitude(cnl::unwrap(temp_t{2}));
            newFluct->set_period(2000);
        }

        {
            auto newFluct = newSensor.add_fluctuations();
            newFluct->set_amplitude(cnl::unwrap(temp_t{3}));
            newFluct->set_period(3000);
        }

        testBox.put(newSensor);

        testBox.processInput();
        CHECK(testBox.lastReplyHasStatusOk());

        testBox.update(1000);

        THEN("The value that is read back is correct")
        {
            // read pair
            testBox.put(uint16_t(0)); // msg id
            testBox.put(commands::READ_OBJECT);
            testBox.put(cbox::obj_id_t(100));

            auto decoded = blox::TempSensorMock();
            testBox.processInputToProto(decoded);

            CHECK(testBox.lastReplyHasStatusOk());
            CHECK(decoded.ShortDebugString() == "value: 90856 connected: true setting: 81920 fluctuations { amplitude: 8192 period: 2000 } fluctuations { amplitude: 12288 period: 3000 }");
        }
    }
}
