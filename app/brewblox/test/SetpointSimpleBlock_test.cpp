/*
 * Copyright 2017 BrewPi
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

#include "../BrewBlox.h"
#include "BrewBloxTestBox.h"
#include "SetpointSimple.test.pb.h"
#include "blox/SetpointSimpleBlock.h"
#include "cbox/CboxPtr.h"
#include "cbox/DataStreamIo.h"
#include <catch.hpp>
#include <sstream>

using namespace cbox;

SCENARIO("A Bloc SetpointSimpleBlock")
{
    BrewBloxTestBox testBox;
    using commands = cbox::Box::CommandID;

    testBox.reset();
    WHEN("a SetpointSimpleBlock is created")
    {

        // create setpoint
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::CREATE_OBJECT);
        testBox.put(cbox::obj_id_t(100));
        testBox.put(uint8_t(0xFF));
        testBox.put(SetpointSimpleBlock::staticTypeId());

        blox::SetpointSimple newSetpoint;
        newSetpoint.set_setpoint(cnl::unwrap(temp_t(21.0)));
        newSetpoint.set_enabled(true);
        testBox.put(newSetpoint);

        testBox.processInput();
        CHECK(testBox.lastReplyHasStatusOk());

        // read setpoint
        THEN("The setpoint settings match what was sent")
        {
            testBox.put(uint16_t(0)); // msg id
            testBox.put(commands::READ_OBJECT);
            testBox.put(cbox::obj_id_t(100));

            auto decoded = blox::SetpointSimple();
            testBox.processInputToProto(decoded);
            CHECK(testBox.lastReplyHasStatusOk());
            CHECK(decoded.ShortDebugString() == "setting: 86016 setpoint: 86016 enabled: true");
        }

        AND_WHEN("The setpoint is disabled, the settting is a stripped field")
        {
            // create setpoint
            testBox.put(uint16_t(0)); // msg id
            testBox.put(commands::WRITE_OBJECT);
            testBox.put(cbox::obj_id_t(100));
            testBox.put(uint8_t(0xFF));
            testBox.put(SetpointSimpleBlock::staticTypeId());

            blox::SetpointSimple newSetpoint;
            newSetpoint.set_setpoint(cnl::unwrap(temp_t(21.0)));
            newSetpoint.set_enabled(false);
            testBox.put(newSetpoint);

            auto decoded = blox::SetpointSimple();
            testBox.processInputToProto(decoded);
            CHECK(testBox.lastReplyHasStatusOk());
            CHECK(decoded.ShortDebugString() == "setpoint: 86016 strippedFields: 1");
        }
    }
}
