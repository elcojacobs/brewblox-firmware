/*
 * Copyright 2018 BrewPi B.V.
 *
 * This file is part of the BrewBlox Control Library.
 *
 * BrewBlox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BrewBlox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BrewBlox. If not, see <http://www.gnu.org/licenses/>.
 */

#include <catch.hpp>

#include "ActuatorDigitalMock.h"
#include "ActuatorLogic.h"
#include <memory>

SCENARIO("ActuatorLogic test", "[ActuatorLogic]")
{
    using State = ActuatorDigital::State;

    auto mock1 = std::make_shared<ActuatorDigitalMock>();
    auto mock2 = std::make_shared<ActuatorDigitalMock>();
    auto mock3 = std::make_shared<ActuatorDigitalMock>();
    auto mock4 = std::make_shared<ActuatorDigitalMock>();
    auto target = ActuatorDigitalMock();
    auto logic = ActuatorLogic(target);

    WHEN("ActuatorLogic is empty, it evaluates to Inactive")
    {
        CHECK(logic.state() == State::Inactive);
        logic.update();
        CHECK(target.state() == State::Inactive);
    }

    WHEN("Three mock actuators are combined using OR")
    {
        auto newSection = std::make_unique<ADLogic::OR>();
        newSection->add([&mock1]() { return mock1; });
        newSection->add([&mock2]() { return mock2; });
        newSection->add([&mock3]() { return mock3; });

        logic.addSection(ADLogic::LogicOp::OR, std::move(newSection));

        THEN("The target is active when one or more of the mocks is active")
        {
            mock1->state(State::Inactive);
            mock2->state(State::Inactive);
            mock3->state(State::Inactive);
            logic.update();
            CHECK(target.state() == State::Inactive);

            mock1->state(State::Active);
            mock2->state(State::Inactive);
            mock3->state(State::Inactive);
            logic.update();
            CHECK(target.state() == State::Active);

            mock1->state(State::Inactive);
            mock2->state(State::Active);
            mock3->state(State::Inactive);
            logic.update();
            CHECK(target.state() == State::Active);

            mock1->state(State::Inactive);
            mock2->state(State::Active);
            mock3->state(State::Active);
            logic.update();
            CHECK(target.state() == State::Active);

            mock1->state(State::Active);
            mock2->state(State::Active);
            mock3->state(State::Active);
            logic.update();
            CHECK(target.state() == State::Active);
        }
    }

    WHEN("Three mock actuators are combined using AND")
    {
        auto newSection = std::make_unique<ADLogic::AND>();
        newSection->add([&mock1]() { return mock1; });
        newSection->add([&mock2]() { return mock2; });
        newSection->add([&mock3]() { return mock3; });
        Active
            logic.addSection(ADLogic::LogicOp::OR, std::move(newSection));

        THEN("The target is active when all of the mocks are active")
        {
            mock1->state(State::Inactive);
            mock2->state(State::Inactive);
            mock3->state(State::Inactive);
            logic.update();
            CHECK(target.state() == State::Inactive);

            mock1->state(State::Active);
            mock2->state(State::Inactive);
            mock3->state(State::Inactive);
            logic.update();
            CHECK(target.state() == State::Inactive);

            mock1->state(State::Inactive);
            mock2->state(State::Active);
            mock3->state(State::Inactive);
            logic.update();
            CHECK(target.state() == State::Inactive);

            mock1->state(State::Inactive);
            mock2->state(State::Active);
            mock3->state(State::Active);
            logic.update();
            CHECK(target.state() == State::Inactive);

            mock1->state(State::Active);
            mock2->state(State::Active);
            mock3->state(State::Active);
            logic.update();
            CHECK(target.state() == State::Active);
        }
    }

    WHEN("Two AND sections are combined with OR")
    {
        {
            auto newSection = std::make_unique<ADLogic::AND>();
            newSection->add([&mock1]() { return mock1; });
            newSection->add([&mock2]() { return mock2; });
            logic.addSection(ADLogic::LogicOp::OR, std::move(newSection));
        }

        {
            auto newSection = std::make_unique<ADLogic::AND>();
            newSection->add([&mock3]() { return mock3; });
            newSection->add([&mock4]() { return mock4; });
            logic.addSection(ADLogic::LogicOp::OR, std::move(newSection));
        }

        THEN("The target is active when one of the AND sections is active")
        {
            mock1->state(State::Inactive);
            mock2->state(State::Inactive);
            mock3->state(State::Inactive);
            mock4->state(State::Inactive);
            logic.update();
            CHECK(target.state() == State::Inactive);

            mock1->state(State::Inactive);
            mock2->state(State::Active);
            mock3->state(State::Active);
            mock4->state(State::Inactive);
            logic.update();
            CHECK(target.state() == State::Inactive);

            mock1->state(State::Inactive);
            mock2->state(State::Inactive);
            mock3->state(State::Active);
            mock4->state(State::Active);
            logic.update();
            CHECK(target.state() == State::Active);

            mock1->state(State::Active);
            mock2->state(State::Active);
            mock3->state(State::Inactive);
            mock4->state(State::Inactive);
            logic.update();
            CHECK(target.state() == State::Active);

            mock1->state(State::Active);
            mock2->state(State::Active);
            mock3->state(State::Active);
            mock4->state(State::Active);
            logic.update();
            CHECK(target.state() == State::Active);
        }
    }

    WHEN("Two OR sections are combined with AND")
    {
        {
            auto newSection = std::make_unique<ADLogic::OR>();
            newSection->add([&mock1]() { return mock1; });
            newSection->add([&mock2]() { return mock2; });
            logic.addSection(ADLogic::LogicOp::OR, std::move(newSection));
        }

        {
            auto newSection = std::make_unique<ADLogic::OR>();
            newSection->add([&mock3]() { return mock3; });
            newSection->add([&mock4]() { return mock4; });
            logic.addSection(ADLogic::LogicOp::AND, std::move(newSection));
        }

        THEN("The target is active when both of the OR sections are active")
        {
            mock1->state(State::Inactive);
            mock2->state(State::Inactive);
            mock3->state(State::Inactive);
            mock4->state(State::Inactive);
            logic.update();
            CHECK(target.state() == State::Inactive);

            mock1->state(State::Inactive);
            mock2->state(State::Active);
            mock3->state(State::Active);
            mock4->state(State::Inactive);
            logic.update();
            CHECK(target.state() == State::Active);

            mock1->state(State::Inactive);
            mock2->state(State::Inactive);
            mock3->state(State::Active);
            mock4->state(State::Active);
            logic.update();
            CHECK(target.state() == State::Inactive);

            mock1->state(State::Active);
            mock2->state(State::Active);
            mock3->state(State::Inactive);
            mock4->state(State::Inactive);
            logic.update();
            CHECK(target.state() == State::Inactive);

            mock1->state(State::Active);
            mock2->state(State::Active);
            mock3->state(State::Active);
            mock4->state(State::Active);
            logic.update();
            CHECK(target.state() == State::Active);
        }
    }
}
