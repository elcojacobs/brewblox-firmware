/*
 * Copyright 2020 BrewPi B.V.
 *
 * This file is part of the BrewBlox Control Library.
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

#include "../inc/string_operators.h"

TEST_CASE("Test customs string operators that avoid sstream")
{
    std::string s = "test ";

    SECTION("int32 20")
    {
        int32_t v1 = 20;
        CHECK(s << v1 == "test 20");
    }

    SECTION("int32 max")
    {
        int32_t v1 = std::numeric_limits<int32_t>::max();
        CHECK(s << v1 == "test 2147483647");
    }

    SECTION("int32 min")
    {
        int32_t v1 = std::numeric_limits<int32_t>::lowest();
        CHECK(s << v1 == "test -2147483648");
    }
    SECTION("uint32 20")
    {
        uint32_t v2 = 20;
        CHECK(s << v2 == "test 20");
    }
    SECTION("uint32 max")
    {
        uint32_t v2 = std::numeric_limits<uint32_t>::max();
        CHECK(s << v2 == "test 4294967295");
    }
    SECTION("uint32 min")
    {
        uint32_t v2 = std::numeric_limits<uint32_t>::lowest();
        CHECK(s << v2 == "test 0");
    }
    SECTION("stream operator concats other string")
    {
        std::string v2 = "other";
        CHECK(s << v2 == "test other");
    }
    SECTION("stream operator concats char array")
    {
        const char buf[] = "other";
        CHECK(s << buf == "test other");
    }
    SECTION("Original string is modified (appended)")
    {
        std::string v2 = "appended.";
        CHECK(s << v2 == "test appended.");
        CHECK(s << v2 == "test appended.appended.");
        CHECK(s << int32_t{-10} == "test appended.appended.-10");
    }
}
