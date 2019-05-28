/*
 * Copyright 2018 BrewPi B.V./Elco Jacobs.
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

#include "ActuatorAnalogConstrained.h"
#include "ActuatorAnalogMock.h"
#include "ActuatorDigital.h"
#include "ActuatorDigitalConstrained.h"
#include "ActuatorOffset.h"
#include "ActuatorPwm.h"
#include "MockIoArray.h"
#include "Pid.h"
#include "SetpointSensorPair.h"
#include "TempSensorMock.h"
#include <iostream>
#include <math.h>

SCENARIO("PID Test with mock actuator", "[pid]")
{
    auto sensor = std::make_shared<TempSensorMock>(20.0);

    auto input = std::make_shared<SetpointSensorPair>([&sensor]() { return sensor; });
    input->setting(20);
    input->settingValid(true);

    auto actuator = std::make_shared<ActuatorAnalogMock>();

    auto pid = Pid(
        [&input]() { return input; },
        [&actuator]() { return actuator; });

    pid.enabled(true);

    WHEN("Only proportional gain is active, the output value is correct")
    {
        pid.kp(10);
        pid.ti(0);
        pid.td(0);

        input->setting(21);
        sensor->value(20);

        for (int32_t i = 0; i < 100; ++i) {
            pid.update(); // update 100 times to settle input filter
        }
        CHECK(actuator->setting() == Approx(10).margin(0.01));
    }

    WHEN("Only Kp is zero, the output is zero.")
    {
        pid.kp(0);
        pid.ti(2000);
        pid.td(100);

        input->setting(21);
        sensor->value(20);

        for (int32_t i = 0; i < 1000; ++i) {
            pid.update();
        }

        CHECK(pid.p() == 0);
        CHECK(pid.i() == 0);
        CHECK(pid.d() == 0);
        CHECK(actuator->setting() == 0);
    }

    WHEN("Proportional gain and integrator are enabled, the output value is correct")
    {
        pid.kp(10);
        pid.ti(2000);
        pid.td(0);

        input->setting(21);
        sensor->value(20);

        for (int32_t i = 0; i < 1000; ++i) {
            pid.update();
        }

        CHECK(pid.p() == Approx(10).epsilon(0.001));
        CHECK(pid.i() == Approx(5).epsilon(0.001));
        CHECK(pid.d() == 0);
        CHECK(actuator->setting() == Approx(10.0 * (1.0 + 1000 * 1.0 / 2000)).epsilon(0.001));

        for (int32_t i = 0; i < 1000; ++i) {
            pid.update();
        }

        CHECK(pid.p() == Approx(10).epsilon(0.001));
        CHECK(pid.i() == Approx(10).epsilon(0.001));
        CHECK(pid.integral() == Approx(2000).epsilon(0.001));
        CHECK(pid.d() == 0);
        CHECK(actuator->setting() == Approx(10.0 * (1.0 + 2000 * 1.0 / 2000)).epsilon(0.001));
    }

    WHEN("Proportional, Integral and Derivative are enabled, the output value is correct with positive Kp")
    {
        pid.kp(10);
        pid.ti(2000);
        pid.td(200);

        input->setting(30);
        sensor->value(20);

        temp_t mockVal;
        double accumulatedError = 0;
        for (int32_t i = 0; i <= 900; ++i) {
            mockVal = temp_t(20) + (temp_t(9) * i) / 900;
            sensor->value(mockVal);
            pid.update();
            accumulatedError += pid.error();
        }

        CHECK(mockVal == 29);
        CHECK(pid.error() == Approx(1.23).epsilon(0.01)); // the filter introduces some delay, which is why this is not 1.0
        CHECK(pid.p() == Approx(12.3).epsilon(0.01));
        CHECK(pid.i() == Approx(accumulatedError * (10.0 / 2000)).epsilon(0.001));
        CHECK(pid.integral() == Approx(accumulatedError).epsilon(0.001));
        CHECK(pid.d() == Approx(-10 * 9.0 / 900 * 200).epsilon(0.001));

        CHECK(actuator->setting() == pid.p() + pid.i() + pid.d());
    }

    WHEN("Proportional, Integral and Derivative are enabled, the output value is correct with negative Kp")
    {
        pid.kp(-10);
        pid.ti(2000);
        pid.td(200);

        input->setting(20);
        sensor->value(30);

        temp_t mockVal;
        double accumulatedError = 0;
        for (int32_t i = 0; i <= 900; ++i) {
            mockVal = temp_t(30) - (temp_t(9) * i) / 900;
            sensor->value(mockVal);
            pid.update();
            accumulatedError += pid.error();
        }

        CHECK(mockVal == 21);
        CHECK(pid.error() == Approx(-1.23).epsilon(0.01)); // the filter introduces some delay, which is why this is not 1.0
        CHECK(pid.p() == Approx(12.3).epsilon(0.01));
        CHECK(pid.i() == Approx(accumulatedError * (-10.0 / 2000)).epsilon(0.001));
        CHECK(pid.integral() == Approx(accumulatedError).epsilon(0.001));
        CHECK(pid.d() == Approx(-10 * 9.0 / 900 * 200).epsilon(0.001));

        CHECK(actuator->setting() == pid.p() + pid.i() + pid.d());
    }

    WHEN("The actuator setting is clipped, the integrator is limited by anti-windup (positive kp)")
    {
        pid.kp(10);
        pid.ti(2000);
        pid.td(200);

        input->setting(21);
        sensor->value(20);
        actuator->minSetting(0);
        actuator->maxSetting(20);

        double accumulatedError = 0;
        for (int32_t i = 0; i <= 10000; ++i) {
            pid.update();
            accumulatedError += pid.error();
        }

        double integratorValueWithoutAntiWindup = accumulatedError * (10.0 / 2000);
        CHECK(integratorValueWithoutAntiWindup == Approx(50.0).epsilon(0.01));
        CHECK(pid.p() == Approx(10).epsilon(0.01));
        CHECK(pid.i() == Approx(10).epsilon(0.01)); // anti windup limits this to 10
        CHECK(pid.integral() == Approx(2000).epsilon(0.001));
        CHECK(pid.d() == Approx(0).margin(0.01));

        CHECK(actuator->setting() == Approx(20).epsilon(0.01));

        input->setting(19);
        accumulatedError = 0;
        for (int32_t i = 0; i <= 10000; ++i) {
            pid.update();
            accumulatedError += pid.error();
        }

        integratorValueWithoutAntiWindup = accumulatedError * (10.0 / 2000);
        CHECK(integratorValueWithoutAntiWindup == Approx(-50.0).epsilon(0.01));
        CHECK(pid.p() == Approx(-10).epsilon(0.01));
        CHECK(pid.i() == Approx(0).margin(0.01)); // anti windup limits this to 0
        CHECK(pid.integral() == Approx(0).margin(0.01));
        CHECK(pid.d() == Approx(0).margin(0.01));

        CHECK(actuator->setting() == Approx(0).margin(0.01));
    }

    WHEN("The actuator setting is clipped, the integrator is limited by anti-windup (negative kp)")
    {
        pid.kp(-10);
        pid.ti(2000);
        pid.td(200);

        input->setting(19);
        sensor->value(20);
        actuator->minSetting(0);
        actuator->maxSetting(20);

        double accumulatedError = 0;
        for (int32_t i = 0; i <= 10000; ++i) {
            pid.update();
            accumulatedError += pid.error();
        }

        double integratorValueWithoutAntiWindup = accumulatedError * (-10.0 / 2000);
        CHECK(integratorValueWithoutAntiWindup == Approx(50.0).epsilon(0.01));
        CHECK(pid.p() == Approx(10).epsilon(0.01));
        CHECK(pid.i() == Approx(10).epsilon(0.01)); // anti windup limits this to 10
        CHECK(pid.d() == Approx(0).margin(0.01));

        CHECK(actuator->setting() == Approx(20).epsilon(0.01));

        input->setting(21);
        accumulatedError = 0;
        for (int32_t i = 0; i <= 10000; ++i) {
            pid.update();
            accumulatedError += pid.error();
        }

        integratorValueWithoutAntiWindup = accumulatedError * (-10.0 / 2000);
        CHECK(integratorValueWithoutAntiWindup == Approx(-50.0).epsilon(0.01));
        CHECK(pid.p() == Approx(-10).epsilon(0.01));
        CHECK(pid.i() == Approx(0).margin(0.01)); // anti windup limits this to 0
        CHECK(pid.integral() == Approx(0).margin(0.01));
        CHECK(pid.d() == Approx(0).margin(0.01));

        CHECK(actuator->setting() == Approx(0).margin(0.01));
    }

    WHEN("The actuator value is not reaching setting, the integrator is limited by anti-windup (positive kp)")
    {
        pid.kp(10);
        pid.ti(2000);
        pid.td(200);

        input->setting(21);
        sensor->value(20);
        actuator->minValue(5);
        actuator->maxValue(20);

        double accumulatedError = 0;
        for (int32_t i = 0; i <= 10000; ++i) {
            pid.update();
            accumulatedError += pid.error();
        }
        pid.update();

        double integratorValueWithoutAntiWindup = accumulatedError * (10.0 / 2000);
        CHECK(integratorValueWithoutAntiWindup == Approx(50.0).epsilon(0.01));
        CHECK(pid.p() == Approx(10).epsilon(0.01));
        CHECK(pid.i() == Approx(13.33).epsilon(0.01)); // anti windup limits this to 13.33 (clipped output + error / 3)
        CHECK(pid.d() == Approx(0).margin(0.01));

        CHECK(actuator->setting() == Approx(23.33).epsilon(0.01));

        input->setting(19);
        accumulatedError = 0;
        for (int32_t i = 0; i <= 10000; ++i) {
            pid.update();
            accumulatedError += pid.error();
        }

        integratorValueWithoutAntiWindup = accumulatedError * (10.0 / 2000);
        CHECK(integratorValueWithoutAntiWindup == Approx(-50.0).epsilon(0.01));
        CHECK(pid.p() == Approx(-10).epsilon(0.01));
        CHECK(pid.i() == Approx(0).margin(0.01)); // anti windup limits this to 0
        CHECK(pid.d() == Approx(0).margin(0.01));

        CHECK(actuator->setting() == Approx(-10).margin(0.01));
    }

    WHEN("The actuator value is not reaching setting, the integrator is limited by anti-windup (negative kp)")
    {
        pid.kp(-10);
        pid.ti(2000);
        pid.td(200);

        input->setting(19);
        sensor->value(20);
        actuator->minValue(5);
        actuator->maxValue(20);

        double accumulatedError = 0;
        for (int32_t i = 0; i <= 10000; ++i) {
            pid.update();
            accumulatedError += pid.error();
        }

        double integratorValueWithoutAntiWindup = accumulatedError * (-10.0 / 2000);
        CHECK(integratorValueWithoutAntiWindup == Approx(50.0).epsilon(0.01));
        CHECK(pid.p() == Approx(10).epsilon(0.01));
        CHECK(pid.i() == Approx(13.33).epsilon(0.01)); // anti windup limits this to 13.33 (clipped output + proportional part / 3)
        CHECK(pid.d() == Approx(0).margin(0.01));

        CHECK(actuator->setting() == Approx(23.33).epsilon(0.01));

        input->setting(21);
        accumulatedError = 0;
        for (int32_t i = 0; i <= 10000; ++i) {
            pid.update();
            accumulatedError += pid.error();
        }

        integratorValueWithoutAntiWindup = accumulatedError * (-10.0 / 2000);
        CHECK(integratorValueWithoutAntiWindup == Approx(-50.0).epsilon(0.01));
        CHECK(pid.p() == Approx(-10).epsilon(0.01));
        CHECK(pid.i() == Approx(0).margin(0.01)); // anti windup limits this to 0
        CHECK(pid.d() == Approx(0).margin(0.01));

        CHECK(actuator->setting() == Approx(-10).margin(0.01));
    }

    WHEN("The PID input is invalid for over 10 seconds, the actuator is set to invalid and the PID is inactive")
    {
        pid.kp(10);
        pid.ti(2000);
        pid.td(200);

        input->setting(21);
        sensor->value(20);

        int32_t i = 0;
        for (; i <= 10000; ++i) {
            if (i == 2000) {
                sensor->connected(false);
            }
            pid.update();
            if (!pid.active()) {
                break;
            }
        }
        CHECK(i == 2010);
        CHECK(actuator->settingValid() == false);

        AND_WHEN("The input becomes valid again, the pid and actuator become active again")
        {
            sensor->connected(true);
            pid.update();

            CHECK(pid.active() == true);
            CHECK(actuator->settingValid() == true);
        }
    }
}

SCENARIO("PID Test with offset actuator", "[pid]")
{
    auto targetSensor = std::make_shared<TempSensorMock>(65.0);

    auto referenceSensor = std::make_shared<TempSensorMock>(65.0);

    auto target = std::make_shared<SetpointSensorPair>(
        [targetSensor]() { return targetSensor; });
    target->setting(65);
    target->settingValid(true);

    auto reference = std::make_shared<SetpointSensorPair>(
        [referenceSensor]() { return referenceSensor; });
    reference->setting(67);
    reference->settingValid(true);

    auto actuator = std::make_shared<ActuatorOffset>(
        [target]() { return target; },
        [reference]() { return reference; });

    auto pid = Pid(
        [&reference]() { return reference; },
        [&actuator]() { return actuator; });

    pid.enabled(true);

    pid.kp(2);
    pid.ti(0);
    pid.td(0);

    for (int32_t i = 0; i < 100; ++i) {
        pid.update(); // update 100 times to settle input filter
    }

    WHEN("The PID has updated, the target setpoint is set correctly")
    {
        CHECK(actuator->setting() == Approx(4.0).margin(0.01));
        CHECK(target->setting() == Approx(71.0).margin(0.01));
        CHECK(actuator->settingValid() == true);
    }

    WHEN("The PID input sensor becomes invalid")
    {
        referenceSensor->connected(false);
        pid.update();

        THEN("The target setpoint is set to invalid after 10 failed updates")
        {
            for (uint8_t i = 0; i < 10; ++i) {
                CHECK(pid.active() == true);
                CHECK(target->settingValid() == true);
                pid.update();
            }

            CHECK(pid.active() == false);
            CHECK(target->settingValid() == false);
        }

        AND_WHEN("The sensor comes back alive, the pid and setpoint are active/valid again")
        {
            referenceSensor->connected(true);
            pid.update();

            CHECK(pid.active() == true);
            CHECK(target->settingValid() == true);
        }
    }

    WHEN("The PID is disabled")
    {
        pid.enabled(false);
        pid.update();

        THEN("The target setpoint is set to invalid")
        {
            CHECK(target->settingValid() == false);
        }

        WHEN("Something else sets the target setpoint later")
        {
            CHECK(target->settingValid() == false);
            target->setting(20.0);
            target->settingValid(true);
            CHECK(target->settingValid() == true);
            pid.update();

            THEN("The already disabled PID doesn't affect it")
            {
                CHECK(target->settingValid() == true);
                CHECK(target->setting() == 20.0);
            }
        }
    }
}

SCENARIO("PID Test with PWM actuator", "[pid]")
{
    auto now = ticks_millis_t(0);
    auto sensor = std::make_shared<TempSensorMock>(20.0);

    auto input = std::make_shared<SetpointSensorPair>(
        [&sensor]() { return sensor; });
    input->settingValid(true);
    input->setting(20);

    auto mockIo = std::make_shared<MockIoArray>();
    auto mock = ActuatorDigital([mockIo]() { return mockIo; }, 1);
    auto constrainedDigital = std::make_shared<ActuatorDigitalConstrained>(mock);

    auto pwm = ActuatorPwm(
        [constrainedDigital]() { return constrainedDigital; },
        4000);

    auto actuator = std::make_shared<ActuatorAnalogConstrained>(pwm);

    auto pid = Pid(
        [&input]() { return input; },
        [&actuator]() { return actuator; });

    pid.enabled(true);

    pid.configureFilter(0, Pid::in_t(1));

    auto nextPwmUpdate = now;
    auto nextPidUpdate = now;

    auto run1000seconds = [&]() {
        auto start = now;
        while (++now < start + 1000'000) {
            if (now >= nextPwmUpdate) {
                nextPwmUpdate = pwm.update(now);
            }
            if (now >= nextPidUpdate) {
                pid.update();
                actuator->update();
                nextPidUpdate = now + 1000;
            }
        }
    };

    WHEN("Only proportional gain is active, the output value is correct")
    {
        pid.kp(10);
        pid.ti(0);
        pid.td(0);

        input->setting(21);
        sensor->value(20);

        run1000seconds();

        CHECK(actuator->setting() == Approx(10).margin(0.01));
    }

    WHEN("Proportional gain and integrator are enabled, the output value is correct")
    {
        pid.kp(10);
        pid.ti(2000);
        pid.td(0);

        input->setting(21);
        sensor->value(20);

        run1000seconds();

        CHECK(pid.p() == Approx(10).epsilon(0.001));
        CHECK(pid.i() == Approx(5).epsilon(0.01));
        CHECK(pid.d() == 0);
        CHECK(actuator->setting() == Approx(10.0 * (1.0 + 1000 * 1.0 / 2000)).epsilon(0.02));

        run1000seconds();

        CHECK(pid.p() == Approx(10).epsilon(0.001));
        CHECK(pid.i() == Approx(10).epsilon(0.01));
        CHECK(pid.d() == 0);
        CHECK(actuator->setting() == Approx(10.0 * (1.0 + 2000 * 1.0 / 2000)).epsilon(0.02));
    }

    WHEN("Proportional, Integral and Derivative are enabled, the output value is correct with positive Kp")
    {
        pid.kp(10);
        pid.ti(2000);
        pid.td(200);

        input->setting(30);
        sensor->value(20);

        temp_t mockVal;
        double accumulatedError = 0;

        auto start = now;
        while (now <= start + 900'000) {
            if (now >= nextPwmUpdate) {
                nextPwmUpdate = pwm.update(now);
            }
            if (now >= nextPidUpdate) {
                mockVal = temp_t(20.0 + 9.0 * (now - start) / 900'000);
                sensor->value(mockVal);
                pid.update();
                actuator->update();
                accumulatedError += pid.error();
                nextPidUpdate = now + 1000;
            }
            ++now;
        }

        CHECK(mockVal == 29);
        CHECK(pid.error() == Approx(1.23).epsilon(0.01)); // the filter introduces some delay, which is why this is not 1.0
        CHECK(pid.p() == Approx(12.3).epsilon(0.01));
        CHECK(pid.i() == Approx(accumulatedError * (10.0 / 2000)).epsilon(0.05)); // some integral anti-windup will occur at the start
        CHECK(pid.d() == Approx(-10 * 9.0 / 900 * 200).epsilon(0.01));

        CHECK(actuator->setting() == pid.p() + pid.i() + pid.d());
    }

    WHEN("Proportional, Integral and Derivative are enabled, the output value is correct with negative Kp")
    {
        pid.kp(-10);
        pid.ti(2000);
        pid.td(200);

        input->setting(20);
        sensor->value(30);

        temp_t mockVal;
        double accumulatedError = 0;

        auto start = now;
        while (now <= start + 900'000) {
            if (now >= nextPwmUpdate) {
                nextPwmUpdate = pwm.update(now);
            }
            if (now >= nextPidUpdate) {
                mockVal = temp_t(30.0 - 9.0 * (now - start) / 900'000);
                sensor->value(mockVal);
                pid.update();
                actuator->update();
                accumulatedError += pid.error();
                nextPidUpdate = now + 1000;
            }
            ++now;
        }

        CHECK(mockVal == 21);
        CHECK(pid.error() == Approx(-1.23).epsilon(0.01)); // the filter introduces some delay, which is why this is not 1.0
        CHECK(pid.p() == Approx(12.3).epsilon(0.01));
        CHECK(pid.i() == Approx(accumulatedError * (-10.0 / 2000)).epsilon(0.05)); // some integral anti-windup will occur at the start
        CHECK(pid.d() == Approx(-10 * 9.0 / 900 * 200).epsilon(0.01));

        CHECK(actuator->setting() == pid.p() + pid.i() + pid.d());
    }

    WHEN("When changing Ti")
    {
        pid.kp(-10);
        pid.ti(2000);
        pid.td(200);

        input->setting(20);
        sensor->value(21);

        auto start = now;
        while (now <= start + 1000'000) {
            if (now >= nextPwmUpdate) {
                nextPwmUpdate = pwm.update(now);
            }
            if (now >= nextPidUpdate) {
                pid.update();
                actuator->update();
                nextPidUpdate = now + 1000;
            }
            ++now;
        }

        CHECK(pid.error() == Approx(-1).epsilon(0.01));
        CHECK(pid.p() == Approx(10).epsilon(0.01));
        CHECK(pid.i() == Approx(10.0 * 1000 / 2000).epsilon(0.01));
        CHECK(pid.d() == Approx(0.0).margin(0.01));

        pid.ti(1000);
        pid.update();

        THEN("The integral action is unchanged")
        {
            CHECK(pid.i() == Approx(10.0 * 1000 / 2000).epsilon(0.01));
        }
        THEN("The integral is scaled with the inverse factor of the change")
        {
            CHECK(pid.integral() == Approx(-500).epsilon(0.01));
        }
    }

    WHEN("When a maximum constraint is added to the actuator that limits to a value under the proportional part")
    {
        pid.kp(-10);
        pid.ti(2000);
        pid.td(200);

        input->setting(20);
        sensor->value(25);

        auto start = now;
        while (now <= start + 1000'000) {
            if (now >= nextPwmUpdate) {
                nextPwmUpdate = pwm.update(now);
            }
            if (now >= nextPidUpdate) {
                pid.update();
                actuator->update();
                nextPidUpdate = now + 1000;
            }
            ++now;
        }

        CHECK(pid.p() == Approx(50).epsilon(0.01));
        CHECK(pid.i() == Approx(50.0 * 1000 / 2000).epsilon(0.01));
        CHECK(pid.d() == Approx(0.0).margin(0.01));

        CHECK(pid.p() + pid.i() + pid.d() == actuator->setting());

        THEN("The integral will be reduced back to zero by the anti-windup after adding the constraint")
        {
            actuator->addConstraint(std::make_unique<AAConstraints::Maximum<1>>(40));

            start = now;
            while (now <= start + 1000'000) {
                if (now >= nextPwmUpdate) {
                    nextPwmUpdate = pwm.update(now);
                }
                if (now >= nextPidUpdate) {
                    pid.update();
                    actuator->update();
                    nextPidUpdate = now + 1000;
                }
                ++now;
            }

            pid.update();

            CHECK(pid.p() == Approx(50).epsilon(0.01));
            CHECK(pid.i() == Approx(0.0).margin(0.05));
            CHECK(pid.d() == Approx(0.0).margin(0.01));

            CHECK(pid.p() + pid.i() + pid.d() != actuator->setting());
        }

        THEN("The integral will be reduced back to zero if increased Kp makes the proportional part over 100")
        {
            pid.kp(-25);

            start = now;
            while (now <= start + 1000'000) {
                if (now >= nextPwmUpdate) {
                    nextPwmUpdate = pwm.update(now);
                }
                if (now >= nextPidUpdate) {
                    pid.update();
                    actuator->update();
                    nextPidUpdate = now + 1000;
                }
                ++now;
            }

            pid.update();

            CHECK(pid.p() == Approx(125).epsilon(0.01));
            CHECK(pid.i() == Approx(0.0).margin(0.1));
            CHECK(pid.d() == Approx(0.0).margin(0.01));

            CHECK(pid.p() + pid.i() + pid.d() != actuator->setting());
        }
    }

    WHEN("The PID is disabled")
    {
        pid.enabled(false);
        pid.update();

        THEN("The pwm setting is set to invalid")
        {
            CHECK(actuator->settingValid() == false);
        }

        AND_WHEN("It is set manually later")
        {
            actuator->setting(50);
            actuator->settingValid(true);
            CHECK(actuator->settingValid() == true);
            CHECK(actuator->setting() == 50);
            pid.update();

            THEN("The disabled PID doesn't affect it")
            {
                CHECK(actuator->settingValid() == true);
                CHECK(actuator->setting() == 50.0);
            }
        }
    }
}