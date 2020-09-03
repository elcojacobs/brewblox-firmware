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

#include "ActuatorDigitalBase.h"
#include "TicksTypes.h"
#include <array>
/*
 * An ActuatorDigitalBase wrapper that logs the most recent changes
 */

class ActuatorDigitalChangeLogged {
public:
    using State = ActuatorDigitalBase::State;

    struct StateChange {
        State newState;
        ticks_millis_t startTime;
    };

private:
    ActuatorDigitalBase& actuator;
    std::array<StateChange, 5> history; // uneven length makes last entry equal to first for toggling (PWM) behavior

protected:
    ticks_millis_t lastUpdateTime = 0;

public:
    ActuatorDigitalChangeLogged(ActuatorDigitalBase& act)
        : actuator(act)
    {
        resetHistory();
    }
    ActuatorDigitalChangeLogged(const ActuatorDigitalChangeLogged&) = delete;
    ActuatorDigitalChangeLogged& operator=(const ActuatorDigitalChangeLogged&) = delete;

    virtual ~ActuatorDigitalChangeLogged() = default;

    void state(const State& val, const ticks_millis_t& now);

    void state(const State& val);

    void setStateUnlogged(const State& val);

    State state() const;

    void update(const ticks_millis_t& now);

    struct StartEnd {
        ticks_millis_t start;
        ticks_millis_t end;
    };

    StartEnd getLastStartEndTime(const State& state, const ticks_millis_t& now) const;

    struct Durations {
        ticks_millis_t currentActive;
        ticks_millis_t currentPeriod;
        ticks_millis_t previousActive;
        ticks_millis_t previousPeriod;
        State lastState;
    };
    Durations activeDurations(const ticks_millis_t& now);

    void resetHistory();

    bool supportsFastIo() const;
};
