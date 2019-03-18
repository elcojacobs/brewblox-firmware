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
#include "blox/ActuatorPinBlock.h"
#include "blox/ActuatorPwmBlock.h"
#include "blox/BalancerBlock.h"
#include "blox/MutexBlock.h"
#include "proto/test/cpp/ActuatorPin_test.pb.h"
#include "proto/test/cpp/ActuatorPwm_test.pb.h"
#include "proto/test/cpp/Balancer_test.pb.h"
#include "proto/test/cpp/Mutex_test.pb.h"

SCENARIO("Two pin actuators are constrained by a mutex", "[balancer, mutex]")
{
    BrewBloxTestBox testBox;
    using commands = cbox::Box::CommandID;

    testBox.reset();

    // create mutex
    testBox.put(uint16_t(0)); // msg id
    testBox.put(commands::CREATE_OBJECT);
    testBox.put(cbox::obj_id_t(101));
    testBox.put(uint8_t(0xFF));
    testBox.put(MutexBlock::staticTypeId());
    {
        auto newMutex = blox::Mutex();
        newMutex.set_differentactuatorwait(100);
        testBox.put(newMutex);
    }
    testBox.processInput();
    CHECK(testBox.lastReplyHasStatusOk());

    auto setPin = [&testBox](cbox::obj_id_t id, blox::AD_State state) {
        // configure pin actuator by writing to the object
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::WRITE_OBJECT);
        testBox.put(id);
        testBox.put(uint8_t(0xFF));
        testBox.put(ActuatorPinBlock::staticTypeId());

        auto pinMsg = blox::ActuatorPin();
        pinMsg.set_state(state);
        pinMsg.set_invert(false);
        auto constraintPtr = pinMsg.mutable_constrainedby()->add_constraints();
        constraintPtr->set_mutex(101);
        testBox.put(pinMsg);

        testBox.processInput();
        CHECK(testBox.lastReplyHasStatusOk());
    };

    // configure pin actuator 1
    setPin(10, blox::AD_State_Active);
    // configure pin actuator 2
    setPin(11, blox::AD_State_Active);

    THEN("The objects read back as expected")
    {
        // read mutex
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::READ_OBJECT);
        testBox.put(cbox::obj_id_t(101));

        {
            auto decoded = blox::Mutex();
            testBox.processInputToProto(decoded);
            CHECK(testBox.lastReplyHasStatusOk());
            CHECK(decoded.ShortDebugString() == "differentActuatorWait: 100");
        }

        // read a pin actuator 1, which is active
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::READ_OBJECT);
        testBox.put(cbox::obj_id_t(10));

        {
            auto decoded = blox::ActuatorPin();
            testBox.processInputToProto(decoded);
            CHECK(testBox.lastReplyHasStatusOk());
            CHECK(decoded.ShortDebugString() == "state: Active constrainedBy { constraints { mutex: 101 } unconstrained: Active }");
        }

        // read a pin actuator 2, which is constrained and inactive
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::READ_OBJECT);
        testBox.put(cbox::obj_id_t(11));

        {
            auto decoded = blox::ActuatorPin();
            testBox.processInputToProto(decoded);
            CHECK(testBox.lastReplyHasStatusOk());
            CHECK(decoded.ShortDebugString() == "constrainedBy { constraints { mutex: 101 limiting: true } unconstrained: Active }");
        }
    }

    WHEN("actuator 2 is toggled, it remains constrained while actuator 1 is active")
    {
        auto readPin = [&testBox](cbox::obj_id_t id) {
            testBox.put(uint16_t(0)); // msg id
            testBox.put(commands::READ_OBJECT);
            testBox.put(id);

            auto decoded = blox::ActuatorPin();
            testBox.processInputToProto(decoded);
            CHECK(testBox.lastReplyHasStatusOk());

            return decoded.state();
        };
        setPin(10, blox::AD_State_Active);
        setPin(11, blox::AD_State_Inactive);
        testBox.update(1);

        setPin(11, blox::AD_State_Active);
        CHECK(readPin(11) == blox::AD_State_Inactive);
        testBox.update(2);

        setPin(11, blox::AD_State_Active);
        CHECK(readPin(11) == blox::AD_State_Inactive);

        setPin(11, blox::AD_State_Inactive);
        CHECK(readPin(11) == blox::AD_State_Inactive);
        testBox.update(3);

        setPin(11, blox::AD_State_Active);
        CHECK(readPin(11) == blox::AD_State_Inactive);
        testBox.update(4);

        AND_WHEN("Actuator 1 is turned OFF")
        {
            setPin(10, blox::AD_State_Inactive);
            CHECK(readPin(10) == blox::AD_State_Inactive);
            testBox.update(5);

            THEN("Actuator 2 can only turn on after the minimum wait time has passed")
            {
                testBox.update(6);
                setPin(11, blox::AD_State_Active);
                CHECK(readPin(11) == blox::AD_State_Inactive);

                testBox.update(10);
                setPin(11, blox::AD_State_Active);
                CHECK(readPin(11) == blox::AD_State_Inactive);

                testBox.update(103);
                setPin(11, blox::AD_State_Active);
                CHECK(readPin(11) == blox::AD_State_Inactive);

                testBox.update(104);
                setPin(11, blox::AD_State_Active);
                CHECK(readPin(11) == blox::AD_State_Active);
            }

            THEN("Activating actuator 1 resets the wait time")
            {
                setPin(11, blox::AD_State_Active);
                CHECK(readPin(11) == blox::AD_State_Inactive);

                testBox.update(10);
                setPin(11, blox::AD_State_Active);
                CHECK(readPin(11) == blox::AD_State_Inactive);

                testBox.update(50);
                setPin(10, blox::AD_State_Active);
                CHECK(readPin(10) == blox::AD_State_Active);

                testBox.update(60);
                setPin(10, blox::AD_State_Inactive);
                CHECK(readPin(10) == blox::AD_State_Inactive);

                testBox.update(61);
                setPin(11, blox::AD_State_Active);
                CHECK(readPin(11) == blox::AD_State_Inactive);

                testBox.update(120);
                setPin(11, blox::AD_State_Active);
                CHECK(readPin(11) == blox::AD_State_Inactive);

                testBox.update(155);
                setPin(11, blox::AD_State_Active);
                CHECK(readPin(11) == blox::AD_State_Inactive);

                testBox.update(160);
                setPin(11, blox::AD_State_Active);
                CHECK(readPin(11) == blox::AD_State_Active);
            }
        }
    }

    WHEN("The pins are driven by 2 balanced PWM actuators")
    {

        // create balancer
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::CREATE_OBJECT);
        testBox.put(cbox::obj_id_t(100));
        testBox.put(uint8_t(0xFF));
        testBox.put(BalancerBlock::staticTypeId());
        {
            auto newBalancer = blox::Balancer();
            testBox.put(newBalancer);
        }
        testBox.processInput();
        CHECK(testBox.lastReplyHasStatusOk());

        // create pwm actuator 1
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::CREATE_OBJECT);
        testBox.put(cbox::obj_id_t(201));
        testBox.put(uint8_t(0xFF));
        testBox.put(ActuatorPwmBlock::staticTypeId());

        {
            auto newPwm = blox::ActuatorPwm();
            newPwm.set_actuatorid(10);
            newPwm.set_setting(cnl::unwrap(ActuatorAnalog::value_t(80)));
            newPwm.set_period(4000);
            newPwm.set_enabled(true);

            auto c = newPwm.mutable_constrainedby()->add_constraints();
            auto balanced = new blox::AnalogConstraint_Balanced();
            balanced->set_balancerid(100);
            c->set_allocated_balanced(balanced);

            testBox.put(newPwm);
        }
        testBox.processInput();
        CHECK(testBox.lastReplyHasStatusOk());

        // create pwm actuator 2
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::CREATE_OBJECT);
        testBox.put(cbox::obj_id_t(301));
        testBox.put(uint8_t(0xFF));
        testBox.put(ActuatorPwmBlock::staticTypeId());

        {
            auto newPwm = blox::ActuatorPwm();
            newPwm.set_actuatorid(11);
            newPwm.set_setting(cnl::unwrap(ActuatorAnalog::value_t(80)));
            newPwm.set_period(4000);
            newPwm.set_enabled(true);

            auto c = newPwm.mutable_constrainedby()->add_constraints();
            auto balanced = new blox::AnalogConstraint_Balanced();
            balanced->set_balancerid(100);
            c->set_allocated_balanced(balanced);

            testBox.put(newPwm);
        }

        testBox.processInput();
        CHECK(testBox.lastReplyHasStatusOk());

        testBox.update(0);
        testBox.update(1000);

        // read balancer
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::READ_OBJECT);
        testBox.put(cbox::obj_id_t(100));

        {
            auto decoded = blox::Balancer();
            testBox.processInputToProto(decoded);
            CHECK(testBox.lastReplyHasStatusOk());
            CHECK(decoded.ShortDebugString() == "clients { id: 1 requested: 327680 granted: 204800 } "  // 80*4096, 50*4096
                                                "clients { id: 2 requested: 327680 granted: 204800 }"); // 80*4096, 50*4096
        }

        // read a pwm actuator 1
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::READ_OBJECT);
        testBox.put(cbox::obj_id_t(201));

        {
            auto decoded = blox::ActuatorPwm();
            testBox.processInputToProto(decoded);
            CHECK(testBox.lastReplyHasStatusOk());
            CHECK(decoded.ShortDebugString() == "actuatorId: 10 "
                                                "period: 4000 setting: 204800 "
                                                "constrainedBy { "
                                                "constraints { "
                                                "balanced { balancerId: 100 granted: 204800 id: 1 } "
                                                "limiting: true } "
                                                "unconstrained: 327680 } "
                                                "drivenActuatorId: 10 "
                                                "enabled: true");
        }

        // read a pwm actuator 2
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::READ_OBJECT);
        testBox.put(cbox::obj_id_t(301));

        {
            auto decoded = blox::ActuatorPwm();
            testBox.processInputToProto(decoded);
            CHECK(testBox.lastReplyHasStatusOk());
            CHECK(decoded.ShortDebugString() == "actuatorId: 11 "
                                                "period: 4000 setting: 204800 "
                                                "constrainedBy { "
                                                "constraints { "
                                                "balanced { balancerId: 100 granted: 204800 id: 2 } "
                                                "limiting: true } "
                                                "unconstrained: 327680 } "
                                                "drivenActuatorId: 11 "
                                                "enabled: true");
        }

        // run for a while
        for (ticks_millis_t now = 1001; now < 50000; ++now) {
            testBox.update(now);
        }

        // read a pwm actuator 1
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::READ_OBJECT);
        testBox.put(cbox::obj_id_t(201));

        {
            auto decoded = blox::ActuatorPwm();
            testBox.processInputToProto(decoded);
            CHECK(decoded.value() == Approx(cnl::unwrap(ActuatorAnalog::value_t(50))).epsilon(0.03));
        }

        // read a pwm actuator 2
        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::READ_OBJECT);
        testBox.put(cbox::obj_id_t(301));

        {
            auto decoded = blox::ActuatorPwm();
            testBox.processInputToProto(decoded);
            CHECK(decoded.value() == Approx(cnl::unwrap(ActuatorAnalog::value_t(50))).epsilon(0.03));
        }
    }
}
