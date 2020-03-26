/*
 * Copyright 2018 BrewPi
 *
 * This file is part of BrewPi.
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

#include "BrewBloxTestBox.h"
#include "MockTicks.h"
#include "blox/SetpointProfileBlock.h"
#include "blox/SetpointSensorPairBlock.h"
#include "blox/TempSensorMockBlock.h"
#include "blox/TicksBlock.h"
#include "cbox/DataStreamIo.h"
#include "proto/test/cpp/SetpointProfile_test.pb.h"
#include "proto/test/cpp/SetpointSensorPair_test.pb.h"
#include "proto/test/cpp/TempSensorMock_test.pb.h"
#include "proto/test/cpp/Ticks_test.pb.h"
#include <catch.hpp>
#include <sstream>

using namespace cbox;

SCENARIO("A SetpointProfile block")
{
    WHEN("a SetpointProfileBlock is created")
    {
        BrewBloxTestBox testBox;
        using commands = cbox::Box::CommandID;

        testBox.reset();

        {
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
            newPair.set_storedsetting(cnl::unwrap(temp_t(99)));
            newPair.set_settingenabled(true);
            testBox.put(newPair);

            testBox.processInput();
            CHECK(testBox.lastReplyHasStatusOk());
        }

        {
            testBox.put(uint16_t(0));
            testBox.put(commands::CREATE_OBJECT);
            testBox.put(cbox::obj_id_t(102));
            testBox.put(uint8_t(0xFF));
            testBox.put(SetpointProfileBlock::staticTypeId());

            // create setpoint profile
            auto message = blox::SetpointProfile();
            message.set_targetid(101);
            message.set_enabled(true);
            message.set_start(20'000);
            {
                auto newPoint = message.add_points();
                newPoint->set_time(10);
                newPoint->set_temperature(cnl::unwrap(temp_t(20)));
            }

            {
                auto newPoint = message.add_points();
                newPoint->set_time(20);
                newPoint->set_temperature(cnl::unwrap(temp_t(21)));
            }

            testBox.put(message);

            testBox.processInput();
            CHECK(testBox.lastReplyHasStatusOk());
        }

        testBox.update(10'000);

        auto pairLookup = brewbloxBox().makeCboxPtr<SetpointSensorPairBlock>(101);
        auto pairPtr = pairLookup.lock();
        auto profileLookup = brewbloxBox().makeCboxPtr<SetpointProfileBlock>(102);
        auto profilePtr = profileLookup.lock();

        REQUIRE(profilePtr);
        REQUIRE(pairPtr);

        WHEN("The box has not received the current time (in seconds since epoch")
        {
            THEN("It does not change the setpoint")
            {
                CHECK(pairPtr->get().setting() == temp_t(99));
                CHECK(pairPtr->get().settingValid() == true);
            }
        }

        WHEN("The box has received the current time (in seconds since epoch")
        {
            // set seconds since epoch
            testBox.put(uint16_t(0));
            testBox.put(commands::WRITE_OBJECT);
            testBox.put(cbox::obj_id_t(3)); // ticks block is at 3
            testBox.put(uint8_t(0xFF));
            testBox.put(TicksBlock<MockTicks>::staticTypeId());

            auto message = blox::Ticks();
            message.set_secondssinceepoch(20'000);
            testBox.put(message);

            auto reply = blox::Ticks();

            testBox.processInputToProto(reply);

            THEN("The system time is updated correctly")
            {
                CHECK(testBox.lastReplyHasStatusOk());
                CHECK(reply.millissinceboot() == 10'000);
                CHECK(reply.secondssinceepoch() == 20'000);
            }

            testBox.update(25000); // system is running for 25 seconds, so seconds since epoch should be 20.015 now

            THEN("The setpoint is valid")
            {
                CHECK(pairPtr->get().settingValid() == true);
            }
            AND_THEN("The setting is correctly interpolated")
            {
                CHECK(pairPtr->get().setting() == temp_t(20.5)); // halfway between points
            }
            AND_WHEN("The SetpointProfile block streams out protobuf settings, the data is as expected")
            {
                testBox.put(uint16_t(0));
                testBox.put(commands::READ_OBJECT);
                testBox.put(cbox::obj_id_t(102));

                auto decoded = blox::SetpointProfile();
                testBox.processInputToProto(decoded);
                CHECK(testBox.lastReplyHasStatusOk());
                // 20.5 * 4096 = 83968
                CHECK(decoded.ShortDebugString() == "points { time: 10 temperature: 81920 } "
                                                    "points { time: 20 temperature: 86016 } "
                                                    "enabled: true "
                                                    "targetId: 101 "
                                                    "drivenTargetId: 101 "
                                                    "start: 20000");
            }
        }

        WHEN("A point at 0s and temp 0.0 is written, it does not disappear")
        {
            testBox.put(uint16_t(0));
            testBox.put(commands::WRITE_OBJECT);
            testBox.put(cbox::obj_id_t(102));
            testBox.put(uint8_t(0xFF));
            testBox.put(SetpointProfileBlock::staticTypeId());

            auto message = blox::SetpointProfile();
            message.set_targetid(101);
            message.set_enabled(true);
            message.set_start(20'000);
            {
                auto newPoint = message.add_points();
                newPoint->set_time(0);
                newPoint->set_temperature(cnl::unwrap(temp_t(0)));
            }

            {
                auto newPoint = message.add_points();
                newPoint->set_time(20);
                newPoint->set_temperature(cnl::unwrap(temp_t(21)));
            }

            testBox.put(message);

            CHECK(testBox.lastReplyHasStatusOk());

            auto decoded = blox::SetpointProfile();
            testBox.processInputToProto(decoded);
            CHECK(testBox.lastReplyHasStatusOk());
            // 20.5 * 4096 = 83968

            CHECK(decoded.ShortDebugString() == "points { temperature: 0 } "
                                                "points { time: 20 temperature: 86016 } "
                                                "enabled: true "
                                                "targetId: 101 "
                                                "drivenTargetId: 101 "
                                                "start: 20000");
        }
    }
}