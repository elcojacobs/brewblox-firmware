/*
 * Copyright 2019 BrewPi B.V.
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

#pragma once

#include "TicksTypes.h"

/*
 * A process value has a setting and an current value
 */
template <ticks_millis_t INTERVAL>
class IntervalHelper {
private:
    duration_millis_t m_accumulatedUpdateLateness = 0;
    ticks_millis_t m_lastUpdate = -INTERVAL;

public:
    IntervalHelper() = default;

    ~IntervalHelper() = default;

    ticks_millis_t update(const ticks_millis_t& now, bool& doUpdate)
    {
        // interval is max 1000ms. Can be shortened to make up for previous updates that were overdue
        auto interval = m_accumulatedUpdateLateness >= INTERVAL ? 0 : INTERVAL - m_accumulatedUpdateLateness;
        auto elapsed = now - m_lastUpdate;

        if (elapsed >= interval) {
            doUpdate = true;
            m_lastUpdate = now;
            m_accumulatedUpdateLateness += elapsed;
            m_accumulatedUpdateLateness = m_accumulatedUpdateLateness > INTERVAL ? m_accumulatedUpdateLateness - INTERVAL : 0;
            interval = m_accumulatedUpdateLateness >= INTERVAL ? 0 : INTERVAL - m_accumulatedUpdateLateness;
            return now + interval;
        }
        return now + interval - elapsed;
    }
};
