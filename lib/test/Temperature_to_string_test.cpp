/*
 * Copyright 2020 BrewPi B.V.
 *
 * This file is part of the Brewblox Control Library.
 *
 * Brewblox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Brewblox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Brewblox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <catch.hpp>

#include "../inc/Temperature.h"

TEST_CASE("Test temperature to string conversion in Celcius en Fahrenheit", "[tempformat]")
{

    SECTION("20C to Celsius string")
    {
        temp_t t = 20.0;
        CHECK(temp_to_string(t, 0, TempUnit::Celsius) == "20");
        CHECK(temp_to_string(t, 1, TempUnit::Celsius) == "20.0");
        CHECK(temp_to_string(t, 2, TempUnit::Celsius) == "20.00");
        CHECK(temp_to_string(t, 3, TempUnit::Celsius) == "20.000");
        CHECK(temp_to_string(t, 4, TempUnit::Celsius) == "20.000"); // more than 3 decimals not supported
    }

    SECTION("20C difference to Celsius string")
    {
        temp_t t = 20.0;
        CHECK(tempDiff_to_string(t, 0, TempUnit::Celsius) == "20");
        CHECK(tempDiff_to_string(t, 1, TempUnit::Celsius) == "20.0");
        CHECK(tempDiff_to_string(t, 2, TempUnit::Celsius) == "20.00");
        CHECK(tempDiff_to_string(t, 3, TempUnit::Celsius) == "20.000");
    }

    SECTION("20C to Fahrenheit string")
    {
        temp_t t = 20.0;
        CHECK(temp_to_string(t, 0, TempUnit::Fahrenheit) == "68");
        CHECK(temp_to_string(t, 1, TempUnit::Fahrenheit) == "68.0");
        CHECK(temp_to_string(t, 2, TempUnit::Fahrenheit) == "68.00");
        CHECK(temp_to_string(t, 3, TempUnit::Fahrenheit) == "68.000");
    }

    SECTION("20C difference to Fahrenheit string")
    {
        temp_t t = 20.0;
        CHECK(tempDiff_to_string(t, 0, TempUnit::Fahrenheit) == "36");
        CHECK(tempDiff_to_string(t, 1, TempUnit::Fahrenheit) == "36.0");
        CHECK(tempDiff_to_string(t, 2, TempUnit::Fahrenheit) == "36.00");
        CHECK(tempDiff_to_string(t, 3, TempUnit::Fahrenheit) == "36.000");
    }

    SECTION("33.33333C to Celsius string")
    {
        temp_t t = 33.333333;
        CHECK(temp_to_string(t, 0, TempUnit::Celsius) == "33");
        CHECK(temp_to_string(t, 1, TempUnit::Celsius) == "33.3");
        CHECK(temp_to_string(t, 2, TempUnit::Celsius) == "33.33");
        CHECK(temp_to_string(t, 3, TempUnit::Celsius) == "33.333");
    }

    SECTION("20C to Fahrenheit string")
    {
        temp_t t = 33.33333;
        CHECK(temp_to_string(t, 0, TempUnit::Fahrenheit) == "92");
        CHECK(temp_to_string(t, 1, TempUnit::Fahrenheit) == "92.0");
        CHECK(temp_to_string(t, 2, TempUnit::Fahrenheit) == "92.00");
        CHECK(temp_to_string(t, 3, TempUnit::Fahrenheit) == "92.000");
    }

    SECTION("177.77777C to Celsius string")
    {
        temp_t t = 177.77777;
        CHECK(temp_to_string(t, 0, TempUnit::Celsius) == "178");
        CHECK(temp_to_string(t, 1, TempUnit::Celsius) == "177.8");
        CHECK(temp_to_string(t, 2, TempUnit::Celsius) == "177.78");
        CHECK(temp_to_string(t, 3, TempUnit::Celsius) == "177.778");
    }

    SECTION("157.5173C to Fahrenheit string")
    {
        temp_t t = 157.5173; // 315.53114F
        CHECK(temp_to_string(t, 0, TempUnit::Fahrenheit) == "316");
        CHECK(temp_to_string(t, 1, TempUnit::Fahrenheit) == "315.5");
        CHECK(temp_to_string(t, 2, TempUnit::Fahrenheit) == "315.53");
        CHECK(temp_to_string(t, 3, TempUnit::Fahrenheit) == "315.531");
    }
}
