/*
 * Copyright 2018 BrewPi B.V.
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

#include "../inc/TempSensorMock.h"
#include "../inc/Temperature.h"

#include <math.h>

SCENARIO("TempSensorMockTest", "[mocktest]")
{

    WHEN("A mock sensor is initialized without providing an initial value, it reads as invalid and 0")
    {
        auto mock = TempSensorMock();

        CHECK(mock.value() == temp_t(0));
        CHECK(mock.valid() == false);
    }

    WHEN("A mock sensor is initialized with an initial value, it reads as valid and that value")
    {
        auto mock = TempSensorMock(20.0);

        CHECK(mock.value() == temp_t(20.0));
        CHECK(mock.valid() == true);
    }

    WHEN("A mock sensor is disconnected, valid() returns false")
    {
        auto mock = TempSensorMock(20.0);
        mock.connected(false);

        CHECK(mock.valid() == false);

        AND_WHEN("It is reconnected, valid() returns true and it reads as the set value again")
        {
            mock.connected(true);

            CHECK(mock.valid() == true);
            CHECK(mock.value() == temp_t(20.0));
        }
    }

    WHEN("Mock sensor has 1 fluctuation configured")
    {
        auto mock = TempSensorMock(20.0);
        mock.fluctuations({{temp_t{2}, 3000}});

        auto min = temp_t{20};
        auto max = temp_t{20};

        for (ticks_millis_t t = 0; t <= 3000; t += 100) {
            mock.update(t);
            auto val = mock.value();
            min = std::min(min, val);
            max = std::max(max, val);
        }
        THEN("Minimum and maximum are at configured amplitude")
        {
            CHECK(min == Approx(18).epsilon(0.01));
            CHECK(max == Approx(22).epsilon(0.01));
        }
        THEN("Flucutation is zero at full period")
        {
            CHECK(mock.value() == Approx(20).epsilon(0.01));
        }
    }

    WHEN("Mock the fluctuation period is zero")
    {
        auto mock = TempSensorMock(20.0);
        mock.fluctuations({{temp_t{2}, 0}});

        for (ticks_millis_t t = 0; t <= 1000; t += 100) {
            mock.update(t);
        }
        THEN("The amplitude is added as a constant value")
        {
            CHECK(mock.value() == temp_t{22});
        }
    }

    WHEN("Mock sensor has 2 fluctuations configured")
    {
        auto mock1 = TempSensorMock(20.0);
        auto mock2 = TempSensorMock(20.0);
        auto mock3 = TempSensorMock(20.0);
        mock1.fluctuations({{temp_t{2}, 5000}});
        mock2.fluctuations({{temp_t{0.5}, 1000}});
        mock3.fluctuations({{temp_t{2}, 5000}, {temp_t{0.5}, 1000}});

        THEN("The result is a superposition of the fluctuations")
        {
            for (ticks_millis_t t = 0; t <= 5000; t += 100) {
                mock1.update(t);
                mock2.update(t);
                mock3.update(t);
                CHECK(mock3.value() == mock1.value() + mock2.value() - temp_t{20.0});
            }
        }
    }
}
