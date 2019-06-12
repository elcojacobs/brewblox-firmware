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
#include "blox/DS2413Block.h"
#include "blox/DigitalActuatorBlock.h"
#include "cbox/CboxPtr.h"
#include "cbox/DataStreamIo.h"
#include "proto/test/cpp/DS2413_test.pb.h"
#include "proto/test/cpp/DigitalActuator_test.pb.h"
#include <sstream>

SCENARIO("A DigitalActuator Block with a DS2413 target")
{
    WHEN("a DS2413 block is created")
    {
        BrewBloxTestBox testBox;
        using commands = cbox::Box::CommandID;
        auto ds2413Id = cbox::obj_id_t(100);
        auto actId = cbox::obj_id_t(101);

        testBox.reset();

        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::CREATE_OBJECT);
        testBox.put(cbox::obj_id_t(ds2413Id));
        testBox.put(uint8_t(0xFF));
        testBox.put(DS2413Block::staticTypeId());

        auto message = blox::DS2413();
        message.set_address(12345678);

        testBox.put(message);

        testBox.processInput();
        CHECK(testBox.lastReplyHasStatusOk());
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::READ_OBJECT);
        testBox.put(cbox::obj_id_t(ds2413Id));

        auto decoded = blox::DS2413();
        testBox.processInputToProto(decoded);

        THEN("The returned protobuf data is as expected")
        {
            CHECK(testBox.lastReplyHasStatusOk());

            // the channels are not in use yet, so the pins are empty
            CHECK(decoded.ShortDebugString() == "address: 12345678 pins { A { } } pins { B { } }");
        }

        THEN("The writable settings match what was sent")
        {
            auto lookup = brewbloxBox().makeCboxPtr<DS2413Block>(ds2413Id);
            auto devicePtr = lookup.lock();
            REQUIRE(devicePtr);
            CHECK(devicePtr->get().getDeviceAddress() == 12345678);
        }

        AND_WHEN("A DigitalActuator block is created that uses one of the channels")
        {
            testBox.put(uint16_t(0)); // msg id
            testBox.put(commands::CREATE_OBJECT);
            testBox.put(cbox::obj_id_t(actId));
            testBox.put(uint8_t(0xFF));
            testBox.put(DigitalActuatorBlock::staticTypeId());

            auto message = blox::DigitalActuator();
            message.set_hwdevice(ds2413Id);
            message.set_channel(1);
            message.set_state(blox::DigitalState::Active);

            testBox.put(message);

            testBox.processInput();
            CHECK(testBox.lastReplyHasStatusOk());

            THEN("A read of the actuator is as expected")
            {
                testBox.put(uint16_t(0)); // msg id
                testBox.put(commands::READ_OBJECT);
                testBox.put(cbox::obj_id_t(actId));

                auto decoded = blox::DigitalActuator();
                testBox.processInputToProto(decoded);

                // in simulation, the hw device will not work and therefore the state will be unknown
                CHECK(decoded.ShortDebugString() == "hwDevice: 100 channel: 1 constrainedBy { unconstrained: Active } strippedFields: 3");
            }
            THEN("A read of the DS2413 is as expected")
            {
                testBox.put(uint16_t(0)); // msg id
                testBox.put(commands::READ_OBJECT);
                testBox.put(cbox::obj_id_t(ds2413Id));

                auto decoded = blox::DS2413();
                testBox.processInputToProto(decoded);

                // the channels are not in use yet, so the pins are empty
                CHECK(decoded.ShortDebugString() == "address: 12345678 pins { A { config: ACTIVE_HIGH } } pins { B { } }");
            }
        }
    }
}
