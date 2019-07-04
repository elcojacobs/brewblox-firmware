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

#include "../inc/IntervalHelper.h"
#include <stdlib.h> /* srand, rand */

SCENARIO("IntervalHelper test")
{
    WHEN("The interval helper is irregularly updated, it will still give the correct number of updates")
    {
        auto testInterval = [](int maxDelay) {
            IntervalHelper<1000> ivh;

            ticks_millis_t now = 0;
            int numUpdates = 0;
            ticks_millis_t nextUpdate = 0;
            while (now < 1000'000) {
                bool doUpdate = false;

                nextUpdate = ivh.update(now, doUpdate);
                CHECK(nextUpdate <= now + 1000);
                if (doUpdate) {
                    numUpdates++;
                }
                auto delay = std::rand() % maxDelay / 5;
                if (delay > 4 * maxDelay / 5) {
                    delay = delay * 5; // only use maximum delay in 20% of the cases
                }
                now = now + delay;
            }
            CHECK(numUpdates == 1000);
        };

        testInterval(100);
        testInterval(1000);
        testInterval(1500);
        testInterval(3000);
    }
}
