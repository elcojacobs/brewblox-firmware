/*
 * Copyright 2020 BrewPi B.V.
 *
 * This file is part of the BrewBlox Control Library.
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

#include "../inc/OneWireMockDevice.h"
#include "../inc/OneWireMockDriver.h"
#include "ActuatorDigital.h"
#include "DS2413.h"
#include "DS2413Mock.h"
#include "OneWireCrc.h"

namespace Catch {
template <>
struct StringMaker<OneWireAddress> {
    static std::string convert(OneWireAddress const& value)
    {
        return value.toString();
    }
};
}

SCENARIO("A mocked OneWire bus and mocked DS2413", "[ds2413]")
{
    OneWireMockDriver owMock;
    OneWire ow(owMock);
    auto addr = makeValidAddress(0x002222334455663A);
    auto ds1mock = std::make_shared<DS2413Mock>(addr);

    owMock.attach(ds1mock);

    auto ds1 = std::make_shared<DS2413>(ow, addr);

    SECTION("The DS2413 hardware (mock) state doesn't match the controller state")
    {
        auto result = ActuatorDigitalBase::State::Unknown;
        CHECK(!ds1->senseChannel(1, result)); // should return false before the update
        CHECK(result == ActuatorDigitalBase::State::Unknown);

        WHEN("The latch pull down is turned on before the block is updated")
        {
            ds1mock->setLatchA(false); // low state is latch enabled

            THEN("Updating the block turns it off")
            {
                ds1->update();

                CHECK(ds1->senseChannel(1, result));
                CHECK(result == ActuatorDigitalBase::State::Inactive); // update has written the default desired state (Inactive)

                CHECK(ds1->senseChannel(2, result));
                CHECK(result == ActuatorDigitalBase::State::Inactive);
            }
        }

        WHEN("The latch pull down is turned off before the block is updated")
        {
            ds1mock->setLatchA(true);

            THEN("Updating the block keeps it disabled and returns the correct state")
            ds1->update();

            CHECK(ds1->senseChannel(1, result));
            CHECK(result == ActuatorDigitalBase::State::Inactive);

            CHECK(ds1mock->getLatchA() == true);

            CHECK(ds1mock->getLatchB() == true);

            CHECK(ds1->senseChannel(2, result));
            CHECK(result == ActuatorDigitalBase::State::Inactive);
        }

        AND_WHEN("The DS2413 is disconnected")
        {
            ds1mock->setConnected(false);
            ds1->update();

            THEN("The state is unknown")
            {
                CHECK(!ds1->senseChannel(1, result));
                CHECK(result == ActuatorDigitalBase::State::Unknown);

                CHECK(!ds1->senseChannel(2, result));
                CHECK(result == ActuatorDigitalBase::State::Unknown);
            }

            AND_WHEN("The latch state changes while the DS2413 is disconnected")
            {
                // enable latch pulldown while disconnected
                ds1mock->setLatchA(false);
                ds1->update();

                THEN("The correct desired state is written on reconnect + update")
                {
                    ds1mock->setConnected(true);

                    CHECK(!ds1mock->getLatchA());
                    CHECK(ds1mock->getLatchB());
                    ds1->update();

                    CHECK(ds1->senseChannel(1, result));
                    CHECK(result == ActuatorDigitalBase::State::Inactive);

                    CHECK(ds1->senseChannel(2, result));
                    CHECK(result == ActuatorDigitalBase::State::Inactive);

                    // write active while connected
                    ds1mock->setConnected(true);
                    ds1->writeChannelConfig(1, IoArray::ChannelConfig::ACTIVE_HIGH);
                    ds1->update();

                    // disconnect
                    ds1mock->setConnected(false);

                    // write inactive while disconnected
                    ds1->update();
                    ds1->writeChannelConfig(1, IoArray::ChannelConfig::ACTIVE_LOW);

                    // mock has not changed while disconnected
                    CHECK(!ds1mock->getLatchA());
                    CHECK(ds1mock->getLatchB());

                    // state is unknown
                    CHECK(!ds1->senseChannel(1, result));
                    CHECK(result == ActuatorDigitalBase::State::Unknown);

                    CHECK(!ds1->senseChannel(2, result));
                    CHECK(result == ActuatorDigitalBase::State::Unknown);

                    // reconnect and update
                    ds1mock->setConnected(true);
                    ds1->update();

                    // should result in state being what has been set as desired state while disconnected
                    CHECK(ds1->senseChannel(1, result));
                    CHECK(result == ActuatorDigitalBase::State::Inactive);

                    CHECK(ds1->senseChannel(2, result));
                    CHECK(result == ActuatorDigitalBase::State::Inactive);
                    CHECK(ds1mock->getLatchA()); // latch was toggled in update
                    CHECK(ds1mock->getLatchB());
                }
            }
        }
    }
}
