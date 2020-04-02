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

#include "FixedPoint.h"

// workaround for arm 5.3 not having some to_string functions
#include <string_operators.h>

std::string
to_string_dec(const fp12_t& t, uint8_t decimals)
{
    static constexpr const int32_t one = cnl::unwrap(fp12_t{1});
    static constexpr const int32_t rounder_up = cnl::unwrap(fp12_t{0.5});
    static constexpr const int32_t rounder_down = cnl::unwrap(fp12_t{-0.5});

    int32_t scale = 10;
    for (uint8_t i = decimals; i > 1; --i) {
        scale *= 10;
    }

    int32_t unwrapped = cnl::unwrap(t);
    int32_t rounder = (unwrapped >= 0) ? rounder_up : rounder_down;
    int32_t asInt = (scale * unwrapped + rounder) / one;

    std::string s;
    s << asInt;

    int missingZeros = int(decimals) + 1 - s.length() + (asInt < 0);
    auto insertAt = s.begin() + (asInt < 0);
    if (missingZeros > 0) {
        insertAt = s.insert(insertAt, missingZeros, '0'); // leading zeros
    }
    auto periodPos = s.end() - decimals;
    s.insert(periodPos, '.');
    return s;
}