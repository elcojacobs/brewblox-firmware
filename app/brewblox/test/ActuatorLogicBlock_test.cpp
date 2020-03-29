/*
 * Copyright 2020 BrewPi B.V.
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
#include "blox/ActuatorLogicBlock.h"
#include "cbox/DataStreamIo.h"
#include "proto/test/cpp/ActuatorLogic_test.pb.h"
#include "proto/test/cpp/DigitalActuator_test.pb.h"
#include <sstream>

SCENARIO("Test")
{
    BrewBloxTestBox testBox;
    using commands = cbox::Box::CommandID;

    testBox.reset();

    auto actId1 = cbox::obj_id_t(101);
    auto actId2 = cbox::obj_id_t(102);
    auto actId3 = cbox::obj_id_t(103);
    auto actId4 = cbox::obj_id_t(104);
    auto actId5 = cbox::obj_id_t(105);
    auto logicId = cbox::obj_id_t(106);

    auto setAct = [&testBox](cbox::obj_id_t id, blox::DigitalState state, bool firstCreate = false) {
        // configure digital actuator by writing to the object
        auto sparkPinsId = cbox::obj_id_t(19); // system object 19 is Spark IO pins

        testBox.put(uint16_t(0)); // msg id
        testBox.put(firstCreate ? commands::CREATE_OBJECT : commands::WRITE_OBJECT);
        testBox.put(id);
        testBox.put(uint8_t(0xFF));
        testBox.put(DigitalActuatorBlock::staticTypeId());

        auto message = blox::DigitalActuator();
        message.set_hwdevice(sparkPinsId);
        message.set_channel(uint8_t(id - 100));
        message.set_desiredstate(state);

        testBox.put(message);

        testBox.processInput();
        CHECK(testBox.lastReplyHasStatusOk());
    };

    // create 5 digital actuators
    for (cbox::obj_id_t i = actId1; i <= actId5; i++) {
        setAct(i, blox::DigitalState::Inactive, true);
    }

    // create logic block
    testBox.put(uint16_t(0)); // msg id
    testBox.put(commands::CREATE_OBJECT);
    testBox.put(logicId);
    testBox.put(uint8_t(0xFF));
    testBox.put(ActuatorLogicBlock::staticTypeId());

    auto message = blox::ActuatorLogic();
    message.set_targetid(actId5);
    message.set_enabled(true);

    testBox.put(message);

    auto decoded = blox::ActuatorLogic();
    testBox.processInputToProto(decoded);
    CHECK(testBox.lastReplyHasStatusOk());
    CHECK(decoded.ShortDebugString() == "targetId: 105 enabled: true");

    WHEN("Three mock actuators are combined using OR")
    {
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::WRITE_OBJECT);
        testBox.put(logicId);
        testBox.put(uint8_t(0xFF));
        testBox.put(ActuatorLogicBlock::staticTypeId());

        auto message = blox::ActuatorLogic();
        message.set_targetid(actId5);
        message.set_enabled(true);

        {
            auto newSection = message.add_sections();
            newSection->set_combineop(blox::ActuatorLogic_LogicOp_OR);
            newSection->set_sectionop(blox::ActuatorLogic_LogicOp_OR);
            newSection->add_inputs(101);
            newSection->add_inputs(102);
            newSection->add_inputs(103);
        }

        testBox.put(message);

        testBox.put(message);

        auto decoded = blox::ActuatorLogic();
        testBox.processInputToProto(decoded);
        CHECK(testBox.lastReplyHasStatusOk());
        CHECK(decoded.ShortDebugString() == "targetId: 105 sections { inputs: 101 inputs: 102 inputs: 103 } enabled: true");

        THEN("The target is active when one or more of the mocks is active")
        {
            {
                setAct(actId1, blox::DigitalState::Inactive);
                setAct(actId2, blox::DigitalState::Inactive);
                setAct(actId3, blox::DigitalState::Inactive);

                testBox.update(1000);

                testBox.put(uint16_t(0));
                testBox.put(commands::READ_OBJECT);
                testBox.put(logicId);

                auto decoded = blox::ActuatorLogic();
                testBox.processInputToProto(decoded);
                CHECK(testBox.lastReplyHasStatusOk());
                CHECK(decoded.ShortDebugString() == "targetId: 105 sections { inputs: 101 inputs: 102 inputs: 103 } enabled: true");
            }
            {
                setAct(actId1, blox::DigitalState::Active);
                setAct(actId2, blox::DigitalState::Inactive);
                setAct(actId3, blox::DigitalState::Inactive);

                testBox.update(2000);

                testBox.put(uint16_t(0));
                testBox.put(commands::READ_OBJECT);
                testBox.put(logicId);

                auto decoded = blox::ActuatorLogic();
                testBox.processInputToProto(decoded);
                CHECK(testBox.lastReplyHasStatusOk());
                CHECK(decoded.ShortDebugString() == "targetId: 105 sections { inputs: 101 inputs: 102 inputs: 103 } enabled: true result: Active");
            }
        }
    }

    WHEN("Four mock actuators are combined using two OR sections combined with AND")
    {
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::WRITE_OBJECT);
        testBox.put(logicId);
        testBox.put(uint8_t(0xFF));
        testBox.put(ActuatorLogicBlock::staticTypeId());

        auto message = blox::ActuatorLogic();
        message.set_targetid(actId5);
        message.set_enabled(true);

        {
            auto newSection = message.add_sections();
            newSection->set_combineop(blox::ActuatorLogic_LogicOp_OR);
            newSection->set_sectionop(blox::ActuatorLogic_LogicOp_OR);
            newSection->add_inputs(101);
            newSection->add_inputs(102);
        }
        {
            auto newSection = message.add_sections();
            newSection->set_combineop(blox::ActuatorLogic_LogicOp_AND);
            newSection->set_sectionop(blox::ActuatorLogic_LogicOp_OR);
            newSection->add_inputs(103);
            newSection->add_inputs(104);
        }

        testBox.put(message);

        auto decoded = blox::ActuatorLogic();
        testBox.processInputToProto(decoded);
        CHECK(testBox.lastReplyHasStatusOk());
        CHECK(decoded.ShortDebugString() == "targetId: 105 sections { inputs: 101 inputs: 102 } sections { combineOp: AND inputs: 103 inputs: 104 } enabled: true");

        THEN("The target is active when at least 1 mock is active on each section")
        {
            {
                setAct(actId1, blox::DigitalState::Inactive);
                setAct(actId2, blox::DigitalState::Active);
                setAct(actId3, blox::DigitalState::Inactive);
                setAct(actId4, blox::DigitalState::Inactive);

                testBox.update(1000);

                testBox.put(uint16_t(0));
                testBox.put(commands::READ_OBJECT);
                testBox.put(logicId);

                auto decoded = blox::ActuatorLogic();
                testBox.processInputToProto(decoded);
                CHECK(testBox.lastReplyHasStatusOk());
                CHECK(decoded.ShortDebugString() == "targetId: 105 sections { inputs: 101 inputs: 102 } sections { combineOp: AND inputs: 103 inputs: 104 } enabled: true");
            }
            {
                setAct(actId1, blox::DigitalState::Inactive);
                setAct(actId2, blox::DigitalState::Active);
                setAct(actId3, blox::DigitalState::Active);
                setAct(actId4, blox::DigitalState::Inactive);

                testBox.update(2000);

                testBox.put(uint16_t(0));
                testBox.put(commands::READ_OBJECT);
                testBox.put(logicId);

                auto decoded = blox::ActuatorLogic();
                testBox.processInputToProto(decoded);
                CHECK(testBox.lastReplyHasStatusOk());
                CHECK(decoded.ShortDebugString() == "targetId: 105 sections { inputs: 101 inputs: 102 } sections { combineOp: AND inputs: 103 inputs: 104 } enabled: true result: Active");
            }
        }
    }
}
