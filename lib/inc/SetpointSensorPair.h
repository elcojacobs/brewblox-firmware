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

#include "ProcessValue.h"
#include "TempSensor.h"
#include "Temperature.h"
#include <functional>
#include <memory>

/*
 * A process value has a setting and an current value
 */
class SetpointSensorPair : public ProcessValue<temp_t> {
private:
    temp_t m_setpoint = 20;
    bool m_settingEnabled = false;
    bool m_setpointValid = true;
    const std::function<std::shared_ptr<TempSensor>()> m_sensor;

public:
    explicit SetpointSensorPair(
        std::function<std::shared_ptr<TempSensor>()>&& _sensor)
        : m_sensor(_sensor)
    {
    }

    virtual ~SetpointSensorPair() = default;

    virtual void setting(temp_t const& setting) override final
    {
        m_setpoint = setting;
    }

    virtual temp_t setting() const override final
    {
        return m_setpoint;
    }

    virtual temp_t value() const override final
    {
        if (auto sPtr = m_sensor()) {
            return sPtr->value();
        } else {
            return 0;
        }
    }

    bool valueValid() const override final
    {
        if (auto sens = m_sensor()) {
            return sens->valid();
        }
        return false;
    }

    bool settingValid() const override final
    {
        return m_settingEnabled && m_setpointValid;
    }

    virtual void settingValid(bool v) override final
    {
        m_setpointValid = v;
    }

    bool settingEnabled() const
    {
        return m_settingEnabled;
    }

    virtual void settingEnabled(bool v)
    {
        m_settingEnabled = v;
    }
};
