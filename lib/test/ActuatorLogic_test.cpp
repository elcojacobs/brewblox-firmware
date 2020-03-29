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
#include "SetpointSensorPair.h"
#include "TempSensorMock.h"
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
        CHECK(logic->evaluate() == State::Inactive);
        logic->update();
        CHECK(target->state() == State::Inactive);
    }

    WHEN("Three mock actuators are combined using OR")
    {
        auto newSection = std::make_unique<ADLogic::ActuatorSection>(ADLogic::SectionOp::OR, ADLogic::CombineOp::OR);
        newSection->add([mock1]() { return mock1; }, false);
        newSection->add([mock2]() { return mock2; }, false);
        newSection->add([mock3]() { return mock3; }, false);

        logic->addSection(std::move(newSection));

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
        auto newSection = std::make_unique<ADLogic::ActuatorSection>(ADLogic::SectionOp::AND, ADLogic::CombineOp::OR);
        newSection->add([mock1]() { return mock1; }, false);
        newSection->add([mock2]() { return mock2; }, false);
        newSection->add([mock3]() { return mock3; }, false);

        logic->addSection(std::move(newSection));

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
        auto newSection = std::make_unique<ADLogic::ActuatorSection>(ADLogic::SectionOp::XOR, ADLogic::CombineOp::OR);
        newSection->add([mock1]() { return mock1; }, false);
        newSection->add([mock2]() { return mock2; }, false);
        newSection->add([mock3]() { return mock3; }, false);

        logic->addSection(std::move(newSection));

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
    WHEN("Three mock actuators are combined using OR and combine op OR_NOT")
    {
        auto newSection = std::make_unique<ADLogic::ActuatorSection>(ADLogic::SectionOp::OR, ADLogic::CombineOp::OR_NOT);
        newSection->add([mock1]() { return mock1; }, false);
        newSection->add([mock2]() { return mock2; }, false);
        newSection->add([mock3]() { return mock3; }, false);

        logic->addSection(std::move(newSection));

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

    WHEN("Three mock actuators are combined using AND and combine op OR_NOT")
    {
        auto newSection = std::make_unique<ADLogic::ActuatorSection>(ADLogic::SectionOp::AND, ADLogic::CombineOp::OR_NOT);
        newSection->add([mock1]() { return mock1; }, false);
        newSection->add([mock2]() { return mock2; }, false);
        newSection->add([mock3]() { return mock3; }, false);

        logic->addSection(std::move(newSection));

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

    WHEN("A setpoint value is compared with a threshold (GE)")
    {
        auto sensor = std::make_shared<TempSensorMock>(20.0);

        auto pair = std::make_shared<SetpointSensorPair>([sensor]() { return sensor; });
        pair->filterChoice(0); // no filtering
        pair->setting(temp_t{20.0});

        auto newSection = std::make_unique<ADLogic::CompareSection>(
            ADLogic::SectionOp::GE,
            ADLogic::CombineOp::OR,
            [pair] { return pair; },
            false,
            temp_t(20));

        logic->addSection(std::move(newSection));

        THEN("The target is active when the threshold is equal or exceeded")
        {
            sensor->setting(temp_t{15.0});
            pair->setting(temp_t{25});
            pair->update();
            logic->update();
            CHECK(target->state() == State::Inactive);

            sensor->setting(temp_t{20.0});
            pair->setting(temp_t{15});
            pair->update();
            logic->update();
            CHECK(target->state() == State::Active);

            sensor->setting(temp_t{25.0});
            pair->setting(temp_t{20});
            pair->update();
            logic->update();
            CHECK(target->state() == State::Active);

            sensor->connected(false);
            // value will be flagged invalid after 10 failed reads
            for (uint8_t i = 0; i <= 10; i++) {
                pair->update();
            }
            logic->update();
            CHECK(target->state() == State::Inactive);
        }
    }

    WHEN("A setpoint setting is compared with a threshold (LE)")
    {
        auto sensor = std::make_shared<TempSensorMock>(20.0);
        auto pair = std::make_shared<SetpointSensorPair>([sensor]() { return sensor; });
        pair->filterChoice(0); // no filtering

        auto newSection = std::make_unique<ADLogic::CompareSection>(
            ADLogic::SectionOp::LE,
            ADLogic::CombineOp::OR,
            [pair] { return pair; },
            true,
            temp_t(20));

        logic->addSection(std::move(newSection));

        THEN("The target is active when the threshold is equal or exceeded")
        {
            sensor->setting(temp_t{15.0});
            pair->setting(temp_t{20});
            pair->update();
            logic->update();
            CHECK(target->state() == State::Inactive);

            pair->settingValid(true);

            sensor->setting(temp_t{15.0});
            pair->setting(temp_t{20});
            pair->update();
            logic->update();
            CHECK(target->state() == State::Active);

            sensor->setting(temp_t{15.0});
            pair->setting(temp_t{25});
            pair->update();
            logic->update();
            CHECK(target->state() == State::Inactive);

            sensor->setting(temp_t{25.0});
            pair->setting(temp_t{15});
            pair->update();
            logic->update();
            CHECK(target->state() == State::Active);

            sensor->setting(temp_t{25.0});
            pair->setting(temp_t{20});
            pair->update();
            logic->update();
            CHECK(target->state() == State::Active);
        }
    }

    WHEN("Two AND sections are combined with OR")
    {
        {
            auto newSection = std::make_unique<ADLogic::ActuatorSection>(ADLogic::SectionOp::AND, ADLogic::CombineOp::OR);
            newSection->add([mock1]() { return mock1; }, false);
            newSection->add([mock2]() { return mock2; }, false);
            logic->addSection(std::move(newSection));
        }

        {
            auto newSection = std::make_unique<ADLogic::ActuatorSection>(ADLogic::SectionOp::AND, ADLogic::CombineOp::OR);
            newSection->add([mock3]() { return mock3; }, false);
            newSection->add([mock4]() { return mock4; }, false);
            logic->addSection(std::move(newSection));
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

    WHEN("Two AND sections are combined with OR, with some actuators inverted")
    {
        {
            auto newSection = std::make_unique<ADLogic::ActuatorSection>(ADLogic::SectionOp::AND, ADLogic::CombineOp::OR);
            newSection->add([mock1]() { return mock1; }, true);
            newSection->add([mock2]() { return mock2; }, false);
            logic->addSection(std::move(newSection));
        }

        {
            auto newSection = std::make_unique<ADLogic::ActuatorSection>(ADLogic::SectionOp::AND, ADLogic::CombineOp::OR);
            newSection->add([mock3]() { return mock3; }, true);
            newSection->add([mock4]() { return mock4; }, false);
            logic->addSection(std::move(newSection));
        }

        THEN("The target is active when one of the AND sections is active")
        {
            mock1->desiredState(State::Active);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Active);
            mock4->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Inactive);
            mock4->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Inactive);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Inactive);
            mock3->desiredState(State::Inactive);
            mock4->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            mock4->desiredState(State::Inactive);
            logic->update();
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Inactive);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Inactive);
            mock4->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Active);
        }
    }

    WHEN("Two OR sections are combined with AND")
    {
        {
            auto newSection = std::make_unique<ADLogic::ActuatorSection>(ADLogic::SectionOp::OR, ADLogic::CombineOp::OR);
            newSection->add([mock1]() { return mock1; }, false);
            newSection->add([mock2]() { return mock2; }, false);
            logic->addSection(std::move(newSection));
        }

        {
            auto newSection = std::make_unique<ADLogic::ActuatorSection>(ADLogic::SectionOp::OR, ADLogic::CombineOp::AND);
            newSection->add([mock3]() { return mock3; }, false);
            newSection->add([mock4]() { return mock4; }, false);
            logic->addSection(std::move(newSection));
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

    WHEN("Two OR sections are combined with OR_NOT")
    {
        {
            auto newSection = std::make_unique<ADLogic::ActuatorSection>(ADLogic::SectionOp::OR, ADLogic::CombineOp::OR);
            newSection->add([mock1]() { return mock1; }, false);
            newSection->add([mock2]() { return mock2; }, false);
            logic->addSection(std::move(newSection));
        }

        {
            auto newSection = std::make_unique<ADLogic::ActuatorSection>(ADLogic::SectionOp::OR, ADLogic::CombineOp::OR_NOT);
            newSection->add([mock3]() { return mock3; }, false);
            newSection->add([mock4]() { return mock4; }, false);
            logic->addSection(std::move(newSection));
        }

        THEN("The target is Active when the first section is true or the second is not true")
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
            CHECK(target->state() == State::Active);

            mock1->desiredState(State::Active);
            mock2->desiredState(State::Active);
            mock3->desiredState(State::Active);
            mock4->desiredState(State::Active);
            logic->update();
            CHECK(target->state() == State::Active);
        }
    }

    WHEN("Two OR sections are combined with AND_NOT")
    {
        {
            auto newSection = std::make_unique<ADLogic::ActuatorSection>(ADLogic::SectionOp::OR, ADLogic::CombineOp::OR);
            newSection->add([mock1]() { return mock1; }, false);
            newSection->add([mock2]() { return mock2; }, false);
            logic->addSection(std::move(newSection));
        }

        {
            auto newSection = std::make_unique<ADLogic::ActuatorSection>(ADLogic::SectionOp::OR, ADLogic::CombineOp::AND_NOT);
            newSection->add([mock3]() { return mock3; }, false);
            newSection->add([mock4]() { return mock4; }, false);
            logic->addSection(std::move(newSection));
        }

        THEN("The target is Active when the first OR section is true and the second is not")
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
            CHECK(target->state() == State::Inactive);

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
