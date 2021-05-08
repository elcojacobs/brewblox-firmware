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

#include "BrewBloxTestBox.h"
#include "Temperature.h"
#include "blox/ActuatorOffsetBlock.h"
#include "blox/SetpointSensorPairBlock.h"
#include "blox/TempSensorMockBlock.h"
#include "blox/proto/test/cpp/ActuatorOffset_test.pb.h"
#include "blox/proto/test/cpp/SetpointSensorPair_test.pb.h"
#include "blox/proto/test/cpp/TempSensorMock_test.pb.h"

SCENARIO("A Blox ActuatorOffset object can be created from streamed protobuf data")
{
    BrewBloxTestBox testBox;
    using commands = cbox::Box::CommandID;

    testBox.reset();

    // create mock sensor 1
    testBox.put(uint16_t(0)); // msg id
    testBox.put(commands::CREATE_OBJECT);
    testBox.put(cbox::obj_id_t(100));
    testBox.put(uint8_t(0xFF));
    testBox.put(TempSensorMockBlock::staticTypeId());

    auto newSensor1 = blox::TempSensorMock();
    newSensor1.set_setting(cnl::unwrap(temp_t(21.0)));
    newSensor1.set_connected(true);
    testBox.put(newSensor1);

    testBox.processInput();
    CHECK(testBox.lastReplyHasStatusOk());

    // create pair 1 (target)
    testBox.put(uint16_t(0)); // msg id
    testBox.put(commands::CREATE_OBJECT);
    testBox.put(cbox::obj_id_t(101));
    testBox.put(uint8_t(0xFF));
    testBox.put(SetpointSensorPairBlock::staticTypeId());

    auto newPair1 = blox::SetpointSensorPair();
    newPair1.set_sensorid(100);
    newPair1.set_storedsetting(cnl::unwrap(temp_t(20.0)));
    newPair1.set_settingenabled(true);
    testBox.put(newPair1);

    testBox.processInput();
    CHECK(testBox.lastReplyHasStatusOk());

    // create mock sensor 2
    testBox.put(uint16_t(0)); // msg id
    testBox.put(commands::CREATE_OBJECT);
    testBox.put(cbox::obj_id_t(102));
    testBox.put(uint8_t(0xFF));
    testBox.put(TempSensorMockBlock::staticTypeId());

    auto newSensor2 = blox::TempSensorMock();
    newSensor2.set_setting(cnl::unwrap(temp_t(27.0)));
    newSensor2.set_connected(true);
    testBox.put(newSensor2);

    testBox.processInput();
    CHECK(testBox.lastReplyHasStatusOk());

    // create pair 2 (reference)
    testBox.put(uint16_t(0)); // msg id
    testBox.put(commands::CREATE_OBJECT);
    testBox.put(cbox::obj_id_t(103));
    testBox.put(uint8_t(0xFF));
    testBox.put(SetpointSensorPairBlock::staticTypeId());

    auto newPair2 = blox::SetpointSensorPair();
    newPair2.set_sensorid(102);
    newPair2.set_storedsetting(cnl::unwrap(temp_t(20.0)));
    newPair2.set_settingenabled(true);
    testBox.put(newPair2);

    testBox.processInput();
    CHECK(testBox.lastReplyHasStatusOk());

    // create actuator
    testBox.put(uint16_t(0)); // msg id
    testBox.put(commands::CREATE_OBJECT);
    testBox.put(cbox::obj_id_t(104));
    testBox.put(uint8_t(0xFF));
    testBox.put(ActuatorOffsetBlock::staticTypeId());

    blox::ActuatorOffset newAct;
    newAct.set_targetid(101);
    newAct.set_referenceid(103);
    newAct.set_referencesettingorvalue(blox::ActuatorOffset_ReferenceKind(ActuatorOffset::ReferenceKind::SETTING));
    newAct.set_desiredsetting(cnl::unwrap(ActuatorAnalog::value_t(12)));
    newAct.set_enabled(true);

    testBox.put(newAct);

    testBox.processInput();
    CHECK(testBox.lastReplyHasStatusOk());

    testBox.update(0);

    // read actuator
    testBox.put(uint16_t(0)); // msg id
    testBox.put(commands::READ_OBJECT);
    testBox.put(cbox::obj_id_t(104));
    {
        auto decoded = blox::ActuatorOffset();
        testBox.processInputToProto(decoded);
        CHECK(testBox.lastReplyHasStatusOk());
        CHECK(decoded.ShortDebugString() ==
              "targetId: 101 referenceId: 103 "
              "setting: 49152 value: 4096 " // setting is 12 (setpoint difference), value is 1 (21 - 20)
              "drivenTargetId: 101 enabled: true "
              "desiredSetting: 49152");
    }

    // read reference pair
    testBox.put(uint16_t(0)); // msg id
    testBox.put(commands::READ_OBJECT);
    testBox.put(cbox::obj_id_t(101));

    {
        auto decoded = blox::SetpointSensorPair();
        testBox.processInputToProto(decoded);
        CHECK(testBox.lastReplyHasStatusOk());
        CHECK(decoded.ShortDebugString() ==
              "sensorId: 100 "
              "setting: 131072 "
              "value: 86016 "
              "settingEnabled: true "
              "storedSetting: 131072 "
              "filterThreshold: 20480 "
              "valueUnfiltered: 86016"); // setting 32, value 21 (setpoint adjusted to 20 + 12)
    }

    // read target pair
    testBox.put(uint16_t(0)); // msg id
    testBox.put(commands::READ_OBJECT);
    testBox.put(cbox::obj_id_t(103));

    {
        auto decoded = blox::SetpointSensorPair();
        testBox.processInputToProto(decoded);
        CHECK(testBox.lastReplyHasStatusOk());
        CHECK(decoded.ShortDebugString() ==
              "sensorId: 102 "
              "setting: 81920 "
              "value: 110592 "
              "settingEnabled: true "
              "storedSetting: 81920 "
              "filterThreshold: 20480 "
              "valueUnfiltered: 110592"); // 20, 27 (unaffected)
    }

    AND_WHEN("The reference setpoint is disabled")
    {
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::WRITE_OBJECT);
        testBox.put(cbox::obj_id_t(103));
        testBox.put(uint8_t(0xFF));
        testBox.put(SetpointSensorPairBlock::staticTypeId());

        auto newPair = blox::SetpointSensorPair();
        newPair.set_storedsetting(cnl::unwrap(temp_t(20.0)));
        newPair.set_settingenabled(false);
        testBox.put(newPair);

        testBox.processInput();
        CHECK(testBox.lastReplyHasStatusOk());

        testBox.update(1000);

        THEN("The actuator is not driving the target setpoint and setting and value are stripped")
        {
            // read actuator
            testBox.put(uint16_t(0)); // msg id
            testBox.put(commands::READ_OBJECT);
            testBox.put(cbox::obj_id_t(104));
            {
                auto decoded = blox::ActuatorOffset();
                testBox.processInputToProto(decoded);
                CHECK(testBox.lastReplyHasStatusOk());
                CHECK(decoded.ShortDebugString() ==
                      "targetId: 101 referenceId: 103 "
                      "enabled: true "
                      "desiredSetting: 49152 "
                      "strippedFields: 7 strippedFields: 6");
            }
        }
    }
}
