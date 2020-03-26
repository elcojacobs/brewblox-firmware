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

#pragma once

#include "TempSensor.h"
#include "Temperature.h"
#include "TicksTypes.h"
#include <vector>

class TempSensorMock final : public TempSensor {
private:
    temp_t m_setting = 0;
    temp_t m_fluctuationsSum = 0;
    bool m_connected = false;

public:
    struct Fluctuation {
        temp_t amplitude;
        duration_millis_t period;
    };

private:
    std::vector<Fluctuation> m_fluctuations;

public:
    TempSensorMock()
    {
    }

    TempSensorMock(temp_t initial)
        : m_setting(initial)
        , m_fluctuationsSum(0)
        , m_connected(true)
    {
    }

    void fluctuations(std::vector<Fluctuation>&& arg);

    const std::vector<Fluctuation>& fluctuations() const
    {
        return m_fluctuations;
    }

    void connected(bool _connected)
    {
        m_connected = _connected;
    }

    bool connected() const
    {
        return m_connected;
    }

    void setting(temp_t val)
    {
        m_setting = val;
    }

    temp_t setting() const
    {
        return m_setting;
    }

    temp_t value() const override final
    {
        return m_setting + m_fluctuationsSum;
    }

    virtual bool valid() const override final
    {
        return m_connected;
    }

    void add(temp_t delta)
    {
        m_setting += delta;
    }

    void update(ticks_millis_t now);
};
