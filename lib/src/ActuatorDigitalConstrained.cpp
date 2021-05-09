/*
 * Copyright 2021 BrewPi B.V.
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

#include "ActuatorDigitalConstrained.h"
#include <algorithm>

void ActuatorDigitalConstrained::addConstraint(std::unique_ptr<Constraint>&& newConstraint)
{
    if (constraints.size() < 8) {
        constraints.push_back(std::move(newConstraint));
    }
    std::sort(constraints.begin(), constraints.end(),
              [](const std::unique_ptr<Constraint>& a, const std::unique_ptr<Constraint>& b) { return a->order() < b->order(); });
}

// remove all constraints and return vector of removed constraints
std::vector<std::unique_ptr<ActuatorDigitalConstrained::Constraint>> ActuatorDigitalConstrained::removeAllConstraints()
{
    auto oldConstraints = std::move(constraints);
    constraints = std::vector<std::unique_ptr<Constraint>>();
    return oldConstraints;
}

duration_millis_t ActuatorDigitalConstrained::checkConstraints(const State& val, const ticks_millis_t& now)
{
    for (auto& c : constraints) {
        auto remaining = c->allowed(val, now, *this);
        if (remaining > 0) {
            return remaining;
        }
    }
    return 0;
}

duration_millis_t ActuatorDigitalConstrained::desiredState(const State& val, const ticks_millis_t& now)
{
    lastUpdateTime = now; // always update fallback time for state setter without time
    m_desiredState = val;
    auto timeRemaining = checkConstraints(val, now);
    if (timeRemaining == 0) {
        ActuatorDigitalChangeLogged::state(val, now);
    }
    return timeRemaining;
}