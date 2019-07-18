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
#include "blox/DS2408Block.h"
#include "blox/DS2413Block.h"
#include "blox/TempSensorOneWireBlock.h"
#include "cbox/CboxPtr.h"
#include "cbox/DataStreamIo.h"
#include "proto/test/cpp/TempSensorOneWire_test.pb.h"
#include <sstream>

SCENARIO("Auto discovery of OneWire devices")
{
    BrewBloxTestBox testBox;
    using commands = cbox::Box::CommandID;

    testBox.reset();

    WHEN("An object discovery command is received")
    {
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::DISCOVER_NEW_OBJECTS);
        auto reply = testBox.processInput();
        THEN("3 new objects are discovered")
        {
            CHECK(reply == cbox::addCrc("00000C") + "|0000" + "," + cbox::addCrc("64002E01") + "," + cbox::addCrc("65002E01") + "," + cbox::addCrc("66002E01") + "," + cbox::addCrc("67003B01") + "," + cbox::addCrc("68003D01") + "\n");
            AND_THEN("These objects can be used as temp sensor")
            {
                auto d1 = brewbloxBox().makeCboxPtr<TempSensor>(100);
                auto d2 = brewbloxBox().makeCboxPtr<TempSensor>(101);
                auto d3 = brewbloxBox().makeCboxPtr<TempSensor>(102);
                auto d4 = brewbloxBox().makeCboxPtr<DS2413Block>(103);
                auto d5 = brewbloxBox().makeCboxPtr<DS2408Block>(104);

                CHECK(d1.lock());
                CHECK(d2.lock());
                CHECK(d3.lock());
                CHECK(d4.lock());
                CHECK(d5.lock());
            }
        }

        AND_WHEN("The command is given for the second time")
        {
            testBox.put(uint16_t(0)); // msg id
            testBox.put(commands::DISCOVER_NEW_OBJECTS);
            auto reply = testBox.processInput();
            THEN("No new objects are discovered")
            {
                CHECK(reply == cbox::addCrc("00000C") + "|0000\n");
            }
        }

        AND_WHEN("One of the sensors is removed")
        {
            testBox.put(uint16_t(0)); // msg id
            testBox.put(commands::DELETE_OBJECT);
            testBox.put(cbox::obj_id_t(101));
            testBox.processInput();
            CHECK(testBox.lastReplyHasStatusOk());

            THEN("It will be discovered again")
            {
                testBox.put(uint16_t(0)); // msg id
                testBox.put(commands::DISCOVER_NEW_OBJECTS);
                auto reply = testBox.processInput();
                CHECK(reply == cbox::addCrc("00000C") + "|0000" + "," + cbox::addCrc("69002E01") + "\n");
            }
        }
    }
}
