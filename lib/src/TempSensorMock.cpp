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
    if (f.period == 0) {
        return f.amplitude;
    }

    // use simple first order approximation of sinus for periodic signal, without using floats
    // sin(x) -> x - x^3 / 3!
    // This crosses zero at crosses -1/sqrt(6) and 1/sqrt(6), scale so -511 and 512 will cross zero:
    // t / 209 - t^3 / (512*512*209), for -512 < t < 512, max amplitude still 1.
    // = (t << 18 - t*t*t) / (209 <<18)

    int32_t scaledPeriod = (f.period + 512) >> 10;
    if (!scaledPeriod) {
        scaledPeriod = 1;
    }
    int32_t t = 512 - ((now / scaledPeriod) & 0x3FF); // value -511 to 512 // prevents overflow below
    auto scale = safe_elastic_fixed_point<2, 28>(cnl::quotient((t << 18) - t * t * t, int32_t{209} << 18));

    return temp_t(f.amplitude * scale);
}

duration_millis_t
TempSensorMock::update(ticks_millis_t now)
{
    m_fluctuationsSum = 0;
    for (const auto& f : m_fluctuations) {
        m_fluctuationsSum += calcFluctuation(f, now);
    }

    return now + m_updateInterval;
}

void
TempSensorMock::fluctuations(std::vector<Fluctuation>&& arg)
{
    m_fluctuations = arg;
    m_updateInterval = 1000;
    for (auto& f : m_fluctuations) {
        auto intervalForPeriod = f.period >> 8;
        if (intervalForPeriod < m_updateInterval) {
            m_updateInterval = intervalForPeriod;
        }
    }
    if (m_updateInterval < 10) {
        m_updateInterval = 10;
    }
}