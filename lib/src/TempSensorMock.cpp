/*
 * Copyright 2020 BrewPi B.V.
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

#include "TempSensorMock.h"
temp_t
calcFluctuation(const TempSensorMock::Fluctuation& f, ticks_millis_t now)
{
    // use simple first order approximation of sinus for periodic signal, without using floats
    // sin(x) -> x - x^3 / 3!
    // This crosses zero at crosses -1/sqrt(6) and 1/sqrt(6), scale so -500 and 500 will cross zero:
    // t / 240 - t^3 / 50937984, for -500 < t < 500, max amplitude still 1.
    int32_t t = 500 - ((now * 1000 / f.period)) % 1000; // value -500 to 499
    auto result = (t * f.amplitude) / 204 - (int32_t(t * t * t) * f.amplitude) / int32_t{50937984};
    return result;
}

void
TempSensorMock::update(ticks_millis_t now)
{
    m_fluctuationsSum = 0;
    for (const auto& f : m_fluctuations) {
        m_fluctuationsSum += calcFluctuation(f, now);
    }
}

void
TempSensorMock::fluctuations(std::vector<Fluctuation>&& arg)
{
    m_fluctuations = arg;
}