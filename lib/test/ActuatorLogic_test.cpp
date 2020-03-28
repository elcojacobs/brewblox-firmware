/*
 * Copyright 2020 BrewPi B.V.
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

#include "ActuatorLogic.h"
#include "MockIoArray.h"
#include "TicksTypes.h"
#include <memory>

SCENARIO("ActuatorLogic test", "[ActuatorLogic]")
{
    using State = ActuatorDigital::State;

    auto mockIo = std::make_shared<MockIoArray>();

    auto act1 = ActuatorDigital([mockIo]() { return mockIo; }, 1);
    auto act2 = ActuatorDigital([mockIo]() { return mockIo; }, 2);
    auto act3 = ActuatorDigital([mockIo]() { return mockIo; }, 3);
    auto act4 = ActuatorDigital([mockIo]() { return mockIo; }, 4);
    auto act5 = ActuatorDigital([mockIo]() { return mockIo; }, 5);

    auto mock1 = std::make_shared<ActuatorDigitalConstrained>(act1);
    auto mock2 = std::make_shared<ActuatorDigitalConstrained>(act2);
    auto mock3 = std::make_shared<ActuatorDigitalConstrained>(act3);
    auto mock4 = std::make_shared<ActuatorDigitalConstrained>(act4);

    auto target = std::make_shared<ActuatorDigitalConstrained>(act5);
    auto logic = std::make_shared<ActuatorLogic>([target]() { return target; });

    WHEN("ActuatorLogic is empty, it evaluates to Inactive")
    {
        CHECK(logic->state() == State::Inactive);
        logic->update();
        CHECK(target->state() == State::Inactive);
    }

    WHEN("Three mock actuators are combined using OR")
    {
        auto newSection = std::make_unique<ADLogic::OR>();
        newSection->add([mock1]() { return mock1; });
        newSection->add([mock2]() { return mock2; });
        newSection->add([mock3]() { return mock3; });

        logic->addSection(ADLogic::LogicOp::OR, std::move(newSection));

        THEN("The target is active when one or more of the mocks is active")
        {
            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Active);
        }
    }

    WHEN("Three mock actuators are combined using AND")
    {
        auto newSection = std::make_unique<ADLogic::AND>();
        newSection->add([mock1]() { return mock1; });
        newSection->add([mock2]() { return mock2; });
        newSection->add([mock3]() { return mock3; });

        logic->addSection(ADLogic::LogicOp::OR, std::move(newSection));

        THEN("The target is active when all of the mocks are active")
        {
            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Active);
        }
    }

    WHEN("Three mock actuators are combined using XOR")
    {
        auto newSection = std::make_unique<ADLogic::XOR>();
        newSection->add([mock1]() { return mock1; });
        newSection->add([mock2]() { return mock2; });
        newSection->add([mock3]() { return mock3; });

        logic->addSection(ADLogic::LogicOp::OR, std::move(newSection));

        THEN("The target is active only when just 1 of the mocks is active")
        {
            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Inactive);
        }
    }
    WHEN("Three mock actuators are combined using NOR")
    {
        auto newSection = std::make_unique<ADLogic::NOR>();
        newSection->add([mock1]() { return mock1; });
        newSection->add([mock2]() { return mock2; });
        newSection->add([mock3]() { return mock3; });

        logic->addSection(ADLogic::LogicOp::OR, std::move(newSection));

        THEN("The target is active when one or more of the mocks is active")
        {
            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Inactive);
        }
    }

    WHEN("Three mock actuators are combined using NAND")
    {
        auto newSection = std::make_unique<ADLogic::NAND>();
        newSection->add([mock1]() { return mock1; });
        newSection->add([mock2]() { return mock2; });
        newSection->add([mock3]() { return mock3; });

        logic->addSection(ADLogic::LogicOp::OR, std::move(newSection));

        THEN("The target is active when all of the mocks are active")
        {
            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Inactive);
        }
    }

    WHEN("Two AND sections are combined with OR")
    {
        {
            auto newSection = std::make_unique<ADLogic::AND>();
            newSection->add([mock1]() { return mock1; });
            newSection->add([mock2]() { return mock2; });
            logic->addSection(ADLogic::LogicOp::OR, std::move(newSection));
        }

        {
            auto newSection = std::make_unique<ADLogic::AND>();
            newSection->add([mock3]() { return mock3; });
            newSection->add([mock4]() { return mock4; });
            logic->addSection(ADLogic::LogicOp::OR, std::move(newSection));
        }

        THEN("The target is active when one of the AND sections is active")
        {
            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Inactive);
            mock4->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            mock4->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Active);
            mock4->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Inactive);
            mock4->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            mock4->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Active);
        }
    }

    WHEN("Two OR sections are combined with AND")
    {
        {
            auto newSection = std::make_unique<ADLogic::OR>();
            newSection->add([mock1]() { return mock1; });
            newSection->add([mock2]() { return mock2; });
            logic->addSection(ADLogic::LogicOp::OR, std::move(newSection));
        }

        {
            auto newSection = std::make_unique<ADLogic::OR>();
            newSection->add([mock3]() { return mock3; });
            newSection->add([mock4]() { return mock4; });
            logic->addSection(ADLogic::LogicOp::AND, std::move(newSection));
        }

        THEN("The target is active when both of the OR sections are active")
        {
            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Inactive);
            mock4->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            mock4->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Active);
            mock4->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Inactive);
            mock4->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            mock4->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Active);
        }
    }

    WHEN("Two OR sections are combined with NOR")
    {
        {
            auto newSection = std::make_unique<ADLogic::OR>();
            newSection->add([mock1]() { return mock1; });
            newSection->add([mock2]() { return mock2; });
            logic->addSection(ADLogic::LogicOp::OR, std::move(newSection));
        }

        {
            auto newSection = std::make_unique<ADLogic::OR>();
            newSection->add([mock3]() { return mock3; });
            newSection->add([mock4]() { return mock4; });
            logic->addSection(ADLogic::LogicOp::NOR, std::move(newSection));
        }

        THEN("The target is inactive when any of the OR sections are active")
        {
            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Inactive);
            mock4->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            mock4->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Active);
            mock4->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Inactive);
            mock4->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            mock4->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Inactive);
        }
    }

    WHEN("Two OR sections are combined with NAND")
    {
        {
            auto newSection = std::make_unique<ADLogic::OR>();
            newSection->add([mock1]() { return mock1; });
            newSection->add([mock2]() { return mock2; });
            logic->addSection(ADLogic::LogicOp::OR, std::move(newSection));
        }

        {
            auto newSection = std::make_unique<ADLogic::OR>();
            newSection->add([mock3]() { return mock3; });
            newSection->add([mock4]() { return mock4; });
            logic->addSection(ADLogic::LogicOp::NAND, std::move(newSection));
        }

        THEN("The target is inactive when both of the OR sections are active")
        {
            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Inactive);
            mock4->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            mock4->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Active);
            mock4->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Inactive);
            mock4->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            mock4->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Inactive);
        }
    }
}
