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

#include "ActuatorAnalogConstrained.h"
#include <algorithm>

using value_t = ActuatorAnalog::value_t;

void
ActuatorAnalogConstrained::addConstraint(std::unique_ptr<Constraint>&& newConstraint)
{
    if (constraints.size() < 8) {
        constraints.push_back(std::move(newConstraint));
    }

    std::sort(constraints.begin(), constraints.end(),
              [](const std::unique_ptr<Constraint>& a, const std::unique_ptr<Constraint>& b) { return a->order() < b->order(); });
}

void
ActuatorAnalogConstrained::removeAllConstraints()
{
    constraints.clear();
}

value_t
ActuatorAnalogConstrained::constrain(const value_t& val)
{
    // keep track of which constraints limit the setting in a bitfield
    m_limiting = 0x00;
    uint8_t bit = 0x01;

    value_t result = val;

    for (auto& c : constraints) {
        auto constrained = c->constrain(result);
        if (constrained != result) {
            m_limiting = m_limiting | bit;
            result = constrained;
        }
        bit = bit << 1;
    }

    return result;
}

void
ActuatorAnalogConstrained::setting(const value_t& val)
{
    // first set actuator to requested value to check whether it constrains the setting itself
    actuator.setting(val);
    m_desiredSetting = actuator.setting();

    // then set it to the constrained value
    if (actuator.settingValid()) {
        actuator.setting(constrain(m_desiredSetting));
    } else {
        constrain(0);
    }
}

void
ActuatorAnalogConstrained::settingValid(bool v)
{
    auto old = actuator.settingValid();
    actuator.settingValid(v);
    if (old != actuator.settingValid()) {
        // update constraints state in case setting valid has changed the limits inside the actuator itself
        constrain(actuator.setting());
    }
}