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

#include "ActuatorAnalog.h"
#include <memory>
#include <vector>

namespace AAConstraints {
using value_t = ActuatorAnalog::value_t;

class Base {
public:
    // constraints are movable but not copyable
    Base() = default;
    Base(const Base&) = delete;
    Base& operator=(const Base&) = delete;

    virtual ~Base() = default;

    virtual value_t constrain(const value_t& val) const = 0;

    virtual uint8_t id() const = 0;

    virtual uint8_t order() const = 0;
};

template <uint8_t ID>
class Minimum : public Base {
private:
    value_t m_min;

public:
    Minimum(const value_t& v)
        : m_min(v)
    {
    }

    virtual value_t constrain(const value_t& val) const override final
    {
        return std::max(val, m_min);
    }

    virtual uint8_t id() const override final
    {
        return ID;
    }

    value_t min() const
    {
        return m_min;
    }

    virtual uint8_t order() const override final
    {
        return 0;
    }
};

template <uint8_t ID>
class Maximum : public Base {
private:
    value_t m_max;

public:
    Maximum(const value_t& v)
        : m_max(v)
    {
    }

    virtual value_t constrain(const value_t& val) const override final
    {
        return std::min(val, m_max);
    }

    virtual uint8_t id() const override final
    {
        return ID;
    }

    value_t max() const
    {
        return m_max;
    }

    virtual uint8_t order() const override final
    {
        return 1;
    }
};
}

/*
 * An ActuatorAnalog has a range output
 */

class ActuatorAnalogConstrained : public ActuatorAnalog {
public:
    using Constraint = AAConstraints::Base;

private:
    std::vector<std::unique_ptr<Constraint>> constraints;
    ActuatorAnalog& actuator;
    uint8_t m_limiting = 0x00;
    value_t m_desiredSetting = 0;

public:
    ActuatorAnalogConstrained(ActuatorAnalog& act)
        : actuator(act)
    {
    }

    ActuatorAnalogConstrained(const ActuatorAnalogConstrained&) = delete;
    ActuatorAnalogConstrained& operator=(const ActuatorAnalogConstrained&) = delete;

    virtual ~ActuatorAnalogConstrained() = default;

    void addConstraint(std::unique_ptr<Constraint>&& newConstraint);

    void removeAllConstraints();

    value_t constrain(const value_t& val);

    virtual void setting(const value_t& val) override final;

    void update()
    {
        if (actuator.settingValid()) {
            setting(m_desiredSetting); // re-apply constraints
        }
    }

    virtual value_t
    setting() const override final
    {
        return actuator.setting();
    }

    virtual value_t
    value() const override final
    {
        return actuator.value();
    }

    virtual bool
    valueValid() const override final
    {
        return actuator.valueValid();
    }

    virtual bool
    settingValid() const override final
    {
        return actuator.settingValid();
    }

    virtual void
    settingValid(bool v) override final;

    value_t
    desiredSetting() const
    {
        return m_desiredSetting;
    }

    uint8_t
    limiting() const
    {
        return m_limiting;
    }

    const auto&
    constraintsList() const
    {
        return constraints;
    }
};