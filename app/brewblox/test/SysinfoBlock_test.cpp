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
#include "blox/SysInfoBlock.h"
#include "blox/stringify.h"
#include "proto/test/cpp/SysInfo.test.pb.h"

using namespace cbox;

SCENARIO("SysInfo Block")
{
    WHEN("The SysInfo block is read")
    {
        BrewBloxTestBox testBox;
        using commands = cbox::Box::CommandID;

        testBox.reset();

        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::READ_OBJECT);
        testBox.put(cbox::obj_id_t(2));

        auto decoded = blox::SysInfo();
        testBox.processInputToProto(decoded);

        std::string version = stringify(GIT_VERSION);
        REQUIRE(version != "GIT_VERSION"); // check that it is properly expanded

        THEN("The VERSION and device ID are correct")
        {
            CHECK(testBox.lastReplyHasStatusOk());
            CHECK(decoded.ShortDebugString() == std::string("deviceId: \"999999999999\" version: \"") + version + std::string("\" platform: gcc hardware: Spark3"));
        }
    }
}
