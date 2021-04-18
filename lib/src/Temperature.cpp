/*
 * Copyright 2018-2020 BrewPi B.V.
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

#include "../inc/Temperature.h"

fp12_t
scale_fahrenheit(fp12_t t)
{
    return cnl::quotient(t * int8_t{9}, int8_t{5});
}

std::string
tempDiff_to_string(const temp_t& t, uint8_t decimals, const TempUnit& unit)
{
    fp12_t val = t;
    if (unit == TempUnit::Fahrenheit) {
        val = scale_fahrenheit(t);
    }
    return to_string_dec(val, decimals);
}

std::string
temp_to_string(const temp_t& t, uint8_t decimals, const TempUnit& unit)
{
    fp12_t val = t;
    if (unit == TempUnit::Fahrenheit) {
        val = scale_fahrenheit(t);
        val += fp12_t(32);
    }
    return to_string_dec(val, decimals);
}