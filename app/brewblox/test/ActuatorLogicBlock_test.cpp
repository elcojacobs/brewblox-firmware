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

    auto setAct = [&testBox](uint8_t nr, blox::DigitalState state, bool firstCreate = false) {
        // configure digital actuator by writing to the object
        auto sparkPinsId = cbox::obj_id_t(19); // system object 19 is Spark IO pins

        testBox.put(uint16_t(0)); // msg id
        testBox.put(firstCreate ? commands::CREATE_OBJECT : commands::WRITE_OBJECT);
        testBox.put(cbox::obj_id_t(100 + nr));
        testBox.put(uint8_t(0xFF));
        testBox.put(DigitalActuatorBlock::staticTypeId());

        auto message = blox::DigitalActuator();
        message.set_hwdevice(sparkPinsId);
        message.set_channel(nr);
        message.set_state(state);

        testBox.put(message);

        testBox.processInput();
        CHECK(testBox.lastReplyHasStatusOk());
    };

    // create 5 digital actuators
    for (int i = 1; i <= 5; i++) {
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
}
