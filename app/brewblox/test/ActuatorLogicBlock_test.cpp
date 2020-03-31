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
#include "blox/DigitalActuatorBlock.h"
#include "blox/SetpointSensorPairBlock.h"
#include "blox/TempSensorMockBlock.h"
#include "cbox/DataStreamIo.h"
#include "proto/test/cpp/ActuatorLogic_test.pb.h"
#include "proto/test/cpp/DigitalActuator_test.pb.h"
#include "proto/test/cpp/SetpointSensorPair_test.pb.h"
#include "proto/test/cpp/TempSensorMock_test.pb.h"
#include <sstream>

SCENARIO("Test", "[maklogicblock]")
{
    CHECK(true);
    BrewBloxTestBox testBox;
    using commands = cbox::Box::CommandID;

    testBox.reset();

    auto actId1 = cbox::obj_id_t(101);
    auto actId2 = cbox::obj_id_t(102);
    auto actId3 = cbox::obj_id_t(103);
    auto actId4 = cbox::obj_id_t(104);
    auto actId5 = cbox::obj_id_t(105);
    auto logicId = cbox::obj_id_t(106);
    auto sensorId = cbox::obj_id_t(107);
    auto setpointId = cbox::obj_id_t(108);

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

    auto setLogic = [&testBox, &logicId, &actId5](blox::ActuatorLogic& message, bool firstCreate = false) {
        testBox.put(uint16_t(0)); // msg id
        testBox.put(firstCreate ? commands::CREATE_OBJECT : commands::WRITE_OBJECT);
        testBox.put(logicId);
        testBox.put(uint8_t(0xFF));
        testBox.put(ActuatorLogicBlock::staticTypeId());

        message.set_targetid(actId5);
        message.set_enabled(true);

        testBox.put(message);

        auto decoded = blox::ActuatorLogic();
        testBox.processInputToProto(decoded);
        REQUIRE(testBox.lastReplyHasStatusOk());
        return decoded;
    };

    // create logic block with emty logic
    auto message = blox::ActuatorLogic();
    auto result = setLogic(message, true);
    CHECK(result.ShortDebugString() == "targetId: 105 enabled: true result: EMPTY");

    WHEN("4 digital actuators are combined with various expressions")
    {
        auto message = blox::ActuatorLogic();
        {
            auto d = message.add_digital();
            d->set_id(101);
            d->set_rhs(blox::DigitalState::Active);
            d->set_op(blox::ActuatorLogic_DigitalCompareOp_DESIRED_IS);
        }
        {
            auto d = message.add_digital();
            d->set_id(102);
            d->set_rhs(blox::DigitalState::Active);
            d->set_op(blox::ActuatorLogic_DigitalCompareOp_DESIRED_IS);
        }
        {
            auto d = message.add_digital();
            d->set_id(103);
            d->set_rhs(blox::DigitalState::Active);
            d->set_op(blox::ActuatorLogic_DigitalCompareOp_DESIRED_IS);
        }
        {
            auto d = message.add_digital();
            d->set_id(104);
            d->set_rhs(blox::DigitalState::Active);
            d->set_op(blox::ActuatorLogic_DigitalCompareOp_DESIRED_IS);
        }

        message.set_expression("a|b|c");

        auto result = setLogic(message);
        CHECK(result.ShortDebugString() == "targetId: 105 enabled: true expression: \"a|b|c\" digital { op: DESIRED_IS id: 101 rhs: Active } digital { op: DESIRED_IS id: 102 rhs: Active } digital { op: DESIRED_IS id: 103 rhs: Active } digital { op: DESIRED_IS id: 104 rhs: Active }");

        THEN("The target is active when one or more of the mocks is active")
        {
            {
                setAct(101, blox::DigitalState::Inactive);
                setAct(102, blox::DigitalState::Active);
                setAct(103, blox::DigitalState::Inactive);

                testBox.update(1000);

                testBox.put(uint16_t(0));
                testBox.put(commands::READ_OBJECT);
                testBox.put(logicId);

                auto decoded = blox::ActuatorLogic();
                testBox.processInputToProto(decoded);
                CHECK(testBox.lastReplyHasStatusOk());
                CHECK(decoded.ShortDebugString() == "targetId: 105 enabled: true result: TRUE expression: \"a|b|c\" digital { op: DESIRED_IS id: 101 rhs: Active } digital { op: DESIRED_IS result: TRUE id: 102 rhs: Active } digital { op: DESIRED_IS id: 103 rhs: Active } digital { op: DESIRED_IS id: 104 rhs: Active }");
            }
            {
                setAct(101, blox::DigitalState::Inactive);
                setAct(102, blox::DigitalState::Inactive);
                setAct(103, blox::DigitalState::Active);

                testBox.update(2000);

                testBox.put(uint16_t(0));
                testBox.put(commands::READ_OBJECT);
                testBox.put(logicId);

                auto decoded = blox::ActuatorLogic();
                testBox.processInputToProto(decoded);
                CHECK(testBox.lastReplyHasStatusOk());
                CHECK(decoded.ShortDebugString() == "targetId: 105 enabled: true result: TRUE expression: \"a|b|c\" digital { op: DESIRED_IS id: 101 rhs: Active } digital { op: DESIRED_IS id: 102 rhs: Active } digital { op: DESIRED_IS result: TRUE id: 103 rhs: Active } digital { op: DESIRED_IS id: 104 rhs: Active }");
            }

            message.set_expression("a(|b&c)");
            auto result = setLogic(message);
            CHECK(result.result() == blox::ActuatorLogic_Result_UNEXPECTED_OPENING_BRACKET);
            CHECK(result.errorpos() == 1);

            message.set_expression("a|(b&c))");
            result = setLogic(message);
            CHECK(result.result() == blox::ActuatorLogic_Result_UNEXPECTED_CLOSING_BRACKET);
            CHECK(result.errorpos() == 7);

            message.set_expression("a|(b&)");
            result = setLogic(message);
            CHECK(result.result() == blox::ActuatorLogic_Result_EMPTY);
            CHECK(result.errorpos() == 5);

            message.set_expression("a|(b&c)");
            result = setLogic(message);
            CHECK(result.result() == blox::ActuatorLogic_Result_FALSE);
            CHECK(result.errorpos() == 0);

            message.set_expression("a|!(b&c)");
            result = setLogic(message);
            CHECK(result.result() == blox::ActuatorLogic_Result_TRUE);
            CHECK(result.errorpos() == 0);

            setAct(101, blox::DigitalState::Inactive);
            setAct(102, blox::DigitalState::Active);
            setAct(103, blox::DigitalState::Active);

            testBox.update(2000);

            result = setLogic(message);
            CHECK(result.result() == blox::ActuatorLogic_Result_TRUE);
            CHECK(result.errorpos() == 0);
        }
    }
    /*
    WHEN("Three mock actuators are combined using OR, with the second actuator inverted")
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
            auto newSection = message.add_section();
            auto acts = newSection->mutable_actuators();
            newSection->set_sectionop(blox::ActuatorLogic_SectionOp_OR);
            newSection->set_combineop(blox::ActuatorLogic_CombineOp_C_OR);
            auto act = acts->add_actuator();
            act->set_id(101);
            act->set_invert(false);
            act = acts->add_actuator();
            act->set_id(102);
            act->set_invert(true);
            act = acts->add_actuator();
            act->set_id(103);
            act->set_invert(false);
        }

        testBox.put(message);

        testBox.put(message);

        auto decoded = blox::ActuatorLogic();
        testBox.processInputToProto(decoded);
        CHECK(testBox.lastReplyHasStatusOk());
        CHECK(decoded.ShortDebugString() == "targetId: 105 enabled: true result: true section { actuators { actuator { id: 101 } actuator { id: 102 invert: true } actuator { id: 103 } } }");

        THEN("The target is active when one or more of the mocks is active")
        {
            {
                setAct(actId1, blox::DigitalState::Inactive);
                setAct(actId2, blox::DigitalState::Active);
                setAct(actId3, blox::DigitalState::Inactive);

                testBox.update(1000);

                testBox.put(uint16_t(0));
                testBox.put(commands::READ_OBJECT);
                testBox.put(logicId);

                auto decoded = blox::ActuatorLogic();
                testBox.processInputToProto(decoded);
                CHECK(testBox.lastReplyHasStatusOk());
                CHECK(decoded.ShortDebugString() == "targetId: 105 enabled: true section { actuators { actuator { id: 101 } actuator { id: 102 invert: true } actuator { id: 103 } } }");
            }
            {
                setAct(actId1, blox::DigitalState::Active);
                setAct(actId2, blox::DigitalState::Active);
                setAct(actId3, blox::DigitalState::Inactive);

                testBox.update(2000);

                testBox.put(uint16_t(0));
                testBox.put(commands::READ_OBJECT);
                testBox.put(logicId);

                auto decoded = blox::ActuatorLogic();
                testBox.processInputToProto(decoded);
                CHECK(testBox.lastReplyHasStatusOk());
                CHECK(decoded.ShortDebugString() == "targetId: 105 enabled: true result: true section { actuators { actuator { id: 101 } actuator { id: 102 invert: true } actuator { id: 103 } } }");
            }
        }
    }

    WHEN("Four mock actuators are combined using two OR section combined with AND")
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
            auto newSection = message.add_section();
            auto acts = newSection->mutable_actuators();
            newSection->set_sectionop(blox::ActuatorLogic_SectionOp_OR);
            newSection->set_combineop(blox::ActuatorLogic_CombineOp_C_OR);
            auto act = acts->add_actuator();
            act->set_id(101);
            act->set_invert(false);
            act = acts->add_actuator();
            act->set_id(102);
            act->set_invert(false);
        }
        {
            auto newSection = message.add_section();
            auto acts = newSection->mutable_actuators();
            newSection->set_sectionop(blox::ActuatorLogic_SectionOp_OR);
            newSection->set_combineop(blox::ActuatorLogic_CombineOp_C_OR);
            auto act = acts->add_actuator();
            act->set_id(103);
            act->set_invert(false);
            act = acts->add_actuator();
            act->set_id(104);
            act->set_invert(false);
        }

        testBox.put(message);

        auto decoded = blox::ActuatorLogic();
        testBox.processInputToProto(decoded);
        CHECK(testBox.lastReplyHasStatusOk());
        CHECK(decoded.ShortDebugString() == "targetId: 105 enabled: true section { actuators { actuator { id: 101 } actuator { id: 102 } } } section { actuators { actuator { id: 103 } actuator { id: 104 } } }");

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

                CHECK(decoded.ShortDebugString() == "targetId: 105 enabled: true result: true section { actuators { actuator { id: 101 } actuator { id: 102 } } } section { actuators { actuator { id: 103 } actuator { id: 104 } } }");
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
                CHECK(decoded.ShortDebugString() == "targetId: 105 enabled: true result: true section { actuators { actuator { id: 101 } actuator { id: 102 } } } section { actuators { actuator { id: 103 } actuator { id: 104 } } }");
            }
        }
    }

    WHEN("2 comparions for an out of range compare are added (LE || GE")
    {
        // create mock sensor
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::CREATE_OBJECT);
        testBox.put(cbox::obj_id_t(sensorId));
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
        testBox.put(cbox::obj_id_t(setpointId));
        testBox.put(uint8_t(0xFF));
        testBox.put(SetpointSensorPairBlock::staticTypeId());

        blox::SetpointSensorPair newPair;
        newPair.set_sensorid(sensorId);
        newPair.set_settingenabled(true);
        newPair.set_storedsetting(cnl::unwrap(temp_t(21)));
        newPair.set_filter(blox::SetpointSensorPair::FilterChoice::SetpointSensorPair_FilterChoice_FILT_NONE);
        newPair.set_filterthreshold(cnl::unwrap(temp_t(0.5)));
        testBox.put(newPair);

        testBox.processInput();
        CHECK(testBox.lastReplyHasStatusOk());

        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::WRITE_OBJECT);
        testBox.put(logicId);
        testBox.put(uint8_t(0xFF));
        testBox.put(ActuatorLogicBlock::staticTypeId());

        auto message = blox::ActuatorLogic();
        message.set_targetid(actId5);
        message.set_enabled(true);

        {
            auto newSection = message.add_section();
            auto comp = newSection->mutable_comparison();
            newSection->set_sectionop(blox::ActuatorLogic_SectionOp_GE);
            newSection->set_combineop(blox::ActuatorLogic_CombineOp_C_OR);
            comp->set_threshold(cnl::unwrap(temp_t{21.0}));
            comp->set_usesetting(false);
            comp->set_compared(setpointId);
        }
        {
            auto newSection = message.add_section();
            auto comp = newSection->mutable_comparison();
            newSection->set_sectionop(blox::ActuatorLogic_SectionOp_LE);
            newSection->set_combineop(blox::ActuatorLogic_CombineOp_C_OR);
            comp->set_threshold(cnl::unwrap(temp_t{19.0}));
            comp->set_usesetting(false);
            comp->set_compared(setpointId);
        }

        testBox.put(message);

        auto decoded = blox::ActuatorLogic();
        testBox.processInputToProto(decoded);
        CHECK(testBox.lastReplyHasStatusOk());
        CHECK(decoded.ShortDebugString() == "targetId: 105 enabled: true section { sectionOp: GE comparison { compared: 108 threshold: 86016 } } section { sectionOp: LE comparison { compared: 108 threshold: 77824 } }");

        auto sensorCboxPtr = brewbloxBox().makeCboxPtr<TempSensorMockBlock>(sensorId);
        auto sensorPtr = sensorCboxPtr.lock();
        REQUIRE(sensorPtr);

        auto setpointCboxPtr = brewbloxBox().makeCboxPtr<SetpointSensorPairBlock>(setpointId);
        auto setpointPtr = setpointCboxPtr.lock();
        REQUIRE(setpointPtr);

        THEN("The target is active when the sensor is out of the range")
        {
            ticks_millis_t now = 1000;
            {
                sensorPtr->get().setting(temp_t{20});
                while (now < 10000) {
                    // give filter time to update
                    testBox.update(now);
                    now += 1000;
                }

                testBox.put(uint16_t(0));
                testBox.put(commands::READ_OBJECT);
                testBox.put(logicId);

                auto decoded = blox::ActuatorLogic();
                testBox.processInputToProto(decoded);
                CHECK(testBox.lastReplyHasStatusOk());

                CHECK(decoded.ShortDebugString() == "targetId: 105 enabled: true section { sectionOp: GE comparison { compared: 108 threshold: 86016 } } section { sectionOp: LE comparison { compared: 108 threshold: 77824 } }");
            }
            {
                sensorPtr->get().setting(temp_t{15});
                while (now < 20000) {
                    // give filter time to update
                    testBox.update(now);
                    now += 1000;
                }

                testBox.put(uint16_t(0));
                testBox.put(commands::READ_OBJECT);
                testBox.put(logicId);

                auto decoded = blox::ActuatorLogic();
                testBox.processInputToProto(decoded);
                CHECK(testBox.lastReplyHasStatusOk());
                CHECK(decoded.ShortDebugString() == "targetId: 105 enabled: true result: true section { sectionOp: GE comparison { compared: 108 threshold: 86016 } } section { sectionOp: LE comparison { compared: 108 threshold: 77824 } }");
            }

            {
                sensorPtr->get().setting(temp_t{25});
                while (now < 30000) {
                    // give filter time to update
                    testBox.update(now);
                    now += 1000;
                }

                testBox.put(uint16_t(0));
                testBox.put(commands::READ_OBJECT);
                testBox.put(logicId);

                auto decoded = blox::ActuatorLogic();
                testBox.processInputToProto(decoded);
                CHECK(testBox.lastReplyHasStatusOk());
                CHECK(decoded.ShortDebugString() == "targetId: 105 enabled: true result: true section { sectionOp: GE comparison { compared: 108 threshold: 86016 } } section { sectionOp: LE comparison { compared: 108 threshold: 77824 } }");
            }
        }
    }
    */
}
