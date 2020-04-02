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
#include "blox/SetpointSensorPairBlock.h"
#include "blox/TempSensorMockBlock.h"
#include "cbox/Box.h"
#include "cbox/DataStream.h"
#include "cbox/DataStreamIo.h"
#include "cbox/Object.h"
#include "proto/test/cpp/SetpointSensorPair_test.pb.h"
#include "proto/test/cpp/TempSensorMock_test.pb.h"
#include "testHelpers.h"

SCENARIO("A Blox SetpointSensorPair object can be created from streamed protobuf data")
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

    // create pair
    testBox.put(uint16_t(0)); // msg id
    testBox.put(commands::CREATE_OBJECT);
    testBox.put(cbox::obj_id_t(101));
    testBox.put(uint8_t(0xFF));
    testBox.put(SetpointSensorPairBlock::staticTypeId());

    blox::SetpointSensorPair newPair;
    newPair.set_sensorid(100);
    newPair.set_settingenabled(true);
    newPair.set_storedsetting(cnl::unwrap(temp_t(21)));
    newPair.set_filter(blox::SetpointSensorPair::FilterChoice::SetpointSensorPair_FilterChoice_FILT_3m);
    newPair.set_filterthreshold(cnl::unwrap(temp_t(0.5)));
    testBox.put(newPair);

    testBox.processInput();
    CHECK(testBox.lastReplyHasStatusOk());

    // read pair
    testBox.put(uint16_t(0)); // msg id
    testBox.put(commands::READ_OBJECT);
    testBox.put(cbox::obj_id_t(101));

    auto decoded = blox::SetpointSensorPair();
    testBox.processInputToProto(decoded);
    CHECK(testBox.lastReplyHasStatusOk());
    CHECK(decoded.ShortDebugString() == "sensorId: 100 "
                                        "setting: 86016 "
                                        "value: 81920 "
                                        "settingEnabled: true "
                                        "storedSetting: 86016 "
                                        "filter: FILT_3m "
                                        "filterThreshold: 2048 "
                                        "valueUnfiltered: 81920");

    WHEN("The sensor is invalid for over 10 seconds")
    {
        auto cboxPtr = brewbloxBox().makeCboxPtr<TempSensorMockBlock>(100);
        auto ptr = cboxPtr.lock();
        REQUIRE(ptr);

        ptr->get().connected(false);

        for (cbox::update_t t = 1000; t <= 12000; t += 100) {
            testBox.update(t);
        }

        THEN("The input value is flagged as stripped field")
        {
            // read pair
            testBox.put(uint16_t(0)); // msg id
            testBox.put(commands::READ_OBJECT);
            testBox.put(cbox::obj_id_t(101));

            auto decoded = blox::SetpointSensorPair();
            testBox.processInputToProto(decoded);

            CHECK(testBox.lastReplyHasStatusOk());
            CHECK(decoded.ShortDebugString() == "sensorId: 100 "
                                                "setting: 86016 "
                                                "settingEnabled: true "
                                                "storedSetting: 86016 "
                                                "filter: FILT_3m "
                                                "filterThreshold: 2048 "
                                                "strippedFields: 6 "
                                                "strippedFields: 11");
        }
    }

    WHEN("The mock sensor value is changed to 25")
    {
        auto cboxPtr = brewbloxBox().makeCboxPtr<TempSensorMockBlock>(100);
        auto ptr = cboxPtr.lock();
        REQUIRE(ptr);

        ptr->get().setting(25);

        testBox.update(1000);

        THEN("The value that is read is still at the old value due to filtering immediately after")
        {
            // read pair
            testBox.put(uint16_t(0)); // msg id
            testBox.put(commands::READ_OBJECT);
            testBox.put(cbox::obj_id_t(101));

            auto decoded = blox::SetpointSensorPair();
            testBox.processInputToProto(decoded);

            CHECK(testBox.lastReplyHasStatusOk());
            CHECK(decoded.ShortDebugString() == "sensorId: 100 "
                                                "setting: 86016 "
                                                "value: 81920 "
                                                "settingEnabled: true "
                                                "storedSetting: 86016 "
                                                "filter: FILT_3m "
                                                "filterThreshold: 2048 "
                                                "valueUnfiltered: 102400");
        }

        AND_THEN("Sending a filter reset trigger will force the filter state to the current value")
        {
            testBox.put(uint16_t(0)); // msg id
            testBox.put(commands::WRITE_OBJECT);
            testBox.put(cbox::obj_id_t(101));
            testBox.put(uint8_t(0xFF));
            testBox.put(SetpointSensorPairBlock::staticTypeId());

            newPair.set_resetfilter(true);
            testBox.put(newPair);
            auto decoded = blox::SetpointSensorPair();
            testBox.processInputToProto(decoded);

            CHECK(testBox.lastReplyHasStatusOk());
            CHECK(decoded.ShortDebugString() == "sensorId: 100 "
                                                "setting: 86016 "
                                                "value: 102400 "
                                                "settingEnabled: true "
                                                "storedSetting: 86016 "
                                                "filter: FILT_3m "
                                                "filterThreshold: 2048 "
                                                "valueUnfiltered: 102400");
        }
    }
}
