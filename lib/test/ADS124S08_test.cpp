/*
 * Copyright 2015 BrewPi/Elco Jacobs.
 *
 * This file is part of BrewPi.
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

#include "ADS124S08.h"
#include <catch.hpp>

SCENARIO("Test CRC calculation", "[ADS124S08]")
{
    SECTION("with status")
    {
        uint8_t data[4] = {0x80, 0xCB, 0xD6, 0x43};
        CHECK(ADS124S08::calculateCrc(data, 4) == uint8_t(0x5A));
    }
    SECTION("without status")
    {
        uint8_t data[3] = {0xCB, 0xD6, 0x43};
        CHECK(ADS124S08::calculateCrc(data, 3) == uint8_t(0x6B));
    }
}
