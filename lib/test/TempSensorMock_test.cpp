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
        TempSensorMock mock;

        CHECK(mock.value() == temp_t(0));
        CHECK(mock.valid() == false);
    }

    WHEN("A mock sensor is initialized with an initial value, it reads as valid and that value")
    {
        TempSensorMock mock(20.0);

        CHECK(mock.value() == temp_t(20.0));
        CHECK(mock.valid() == true);
    }

    WHEN("A mock sensor is disconnected, valid() returns false")
    {
        TempSensorMock mock(20.0);
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
        auto test = [](uint32_t period) {
            TempSensorMock mock(20.0);
            mock.fluctuations({{temp_t{2}, period}});

            auto min = temp_t{20};
            auto max = temp_t{20};
            mock.update(0);
            auto previous = mock.value();
            auto maxDiff = temp_t{0.0};
            auto zero_cross_time1 = ticks_millis_t{0};
            auto zero_cross_time2 = ticks_millis_t{0};
            auto zero_cross_time3 = ticks_millis_t{0};

            ticks_millis_t now = 0;
            while (now < period * 2) {
                auto nextUpdate = mock.update(now);
                auto val = mock.value();
                min = std::min(min, val);
                max = std::max(max, val);
                maxDiff = std::max(maxDiff, temp_t(cnl::abs(val - previous)));
                previous = val;
                if (!zero_cross_time1) {
                    if (val < temp_t{20.0}) {
                        zero_cross_time1 = now;
                    }
                } else if (!zero_cross_time2) {
                    if (val >= temp_t{20.0}) {
                        zero_cross_time2 = now;
                    }
                } else if (!zero_cross_time3) {
                    if (val < temp_t{20.0}) {
                        zero_cross_time3 = now;
                    }
                }
                now = nextUpdate;
            }
            INFO(period);
            THEN("The period is correct within 512ms")
            {
                // rounding in calculation limits period precision to 512ms worst case
                CHECK(zero_cross_time3 - zero_cross_time1 == Approx(period).margin(512));
            }
            THEN("Minimum and maximum are at configured amplitude")
            {
                CHECK(min == Approx(18).epsilon(0.01));
                CHECK(max == Approx(22).epsilon(0.01));
            }

            THEN("The fluctuation is continuous")
            {
                CHECK(maxDiff < temp_t{0.1});
            }
        };

        test(10000ul);              // 10s
        test(30000ul);              // 30s
        test(120 * 1000ul);         // 2m
        test(2 * 60 * 60 * 1000ul); // 2h
        test(6 * 60 * 60 * 1000ul); // 6h
    }

    WHEN("Mock the fluctuation period is zero")
    {
        TempSensorMock mock(20.0);
        mock.fluctuations({{temp_t{2}, 0}});

        duration_millis_t now = 0;
        while (now < 10000) {
            now = mock.update(now);
        }
        THEN("The amplitude is added as a constant value")
        {
            CHECK(mock.value() == temp_t{22});
        }
    }

    WHEN("Mock sensor has 2 fluctuations configured")
    {
        TempSensorMock mock1(20.0);
        TempSensorMock mock2(20.0);
        TempSensorMock mock3(20.0);
        mock1.fluctuations({{temp_t{2}, 50000}});
        mock2.fluctuations({{temp_t{0.5}, 20000}});
        mock3.fluctuations({{temp_t{2}, 50000}, {temp_t{0.5}, 20000}});

        THEN("The result is a superposition of the fluctuations")
        {
            duration_millis_t now = 0;
            while (now < 50000) {
                mock1.update(now);
                mock2.update(now);
                now = mock3.update(now);
                CHECK(mock3.value() == mock1.value() + mock2.value() - temp_t{20.0});
            }
        }
    }
}
