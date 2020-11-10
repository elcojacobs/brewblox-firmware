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

#include "ActuatorDigitalChangeLogged.h"
#include "TicksTypes.h"
#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

class ActuatorDigitalConstrained;
namespace ADConstraints {
using State = ActuatorDigitalBase::State;

template <class T>
uint8_t typeId();

class Base {
public:
    Base() = default;
    virtual ~Base() = default;

    virtual duration_millis_t allowedImpl(State& desiredState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged& act) = 0;

    virtual uint8_t id() const = 0;

    virtual uint8_t order() const = 0;

    void timeRemaining(duration_millis_t v)
    {
        m_timeRemaining = v;
    }

    duration_millis_t timeRemaining() const
    {
        return m_timeRemaining;
    }

    duration_millis_t allowed(State& newState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged& act)
    {
        m_timeRemaining = allowedImpl(newState, now, act);
        return m_timeRemaining;
    }

private:
    duration_millis_t m_timeRemaining = 0;
};

template <class T>
class BaseImpl : public Base {
    virtual uint8_t id() const override final
    {
        return typeId<T>();
    }
};
} // end namespace ADConstraints

class ActuatorDigitalConstrained : private ActuatorDigitalChangeLogged {
public:
    using Constraint = ADConstraints::Base;

private:
    std::vector<std::unique_ptr<Constraint>> constraints;
    State m_desiredState = State::Inactive;

public:
    ActuatorDigitalConstrained(ActuatorDigitalBase& act)
        : ActuatorDigitalChangeLogged(act)
    {
    }

    ActuatorDigitalConstrained(const ActuatorDigitalConstrained&) = delete;
    ActuatorDigitalConstrained& operator=(const ActuatorDigitalConstrained&) = delete;
    ActuatorDigitalConstrained& operator=(ActuatorDigitalConstrained&&) = delete;
    ActuatorDigitalConstrained(ActuatorDigitalConstrained&&) = delete;

    virtual ~ActuatorDigitalConstrained() = default;

    // ActuatorDigitalChangeLogged is inherited privately to prevent bypassing constraints.
    // explicitly make functions available that should be in public interface here.
    using ActuatorDigitalChangeLogged::activeDurations;
    using ActuatorDigitalChangeLogged::getLastStartEndTime;
    using ActuatorDigitalChangeLogged::setStateUnlogged;
    using ActuatorDigitalChangeLogged::supportsFastIo;

    void addConstraint(std::unique_ptr<Constraint>&& newConstraint)
    {
        if (constraints.size() < 8) {
            constraints.push_back(std::move(newConstraint));
        }
        std::sort(constraints.begin(), constraints.end(),
                  [](const std::unique_ptr<Constraint>& a, const std::unique_ptr<Constraint>& b) { return a->order() < b->order(); });
    }

    // remove all constraints and return vector of removed constraints
    auto removeAllConstraints()
    {
        auto oldConstraints = std::move(constraints);
        constraints = std::vector<std::unique_ptr<Constraint>>();
        return oldConstraints;
    }

    void resetHistory()
    {
        ActuatorDigitalChangeLogged::resetHistory();
    }

    duration_millis_t checkConstraints(State& val, const ticks_millis_t& now)
    {
        for (auto& c : constraints) {
            auto remaining = c->allowed(val, now, *this);
            if (remaining > 0) {
                return remaining;
            }
        }
        return 0;
    }

    duration_millis_t desiredState(const State& val, const ticks_millis_t& now)
    {
        m_desiredState = val;
        return updateImpl(now);
    }

    duration_millis_t desiredState(const State& val)
    {
        return desiredState(val, lastUpdateTime);
    }

    State state() const
    {
        return ActuatorDigitalChangeLogged::state();
    }

    void setStateUnlogged(const State& val)
    {
        m_desiredState = val;
        ActuatorDigitalChangeLogged::setStateUnlogged(val);
    }

    ticks_millis_t update(ticks_millis_t now)
    {
        auto wait = updateImpl(now);
        if (wait == 0 && m_desiredState == state()) {
            wait = 1000; // no pending changes
        }

        return now + wait;
    }

    State
    desiredState() const
    {
        return m_desiredState;
    }

    const auto&
    constraintsList() const
    {
        return constraints;
    }

private:
    duration_millis_t updateImpl(ticks_millis_t now)
    {
        // re-apply constraints for new update time
        // constraints can change the desired state (maxOn time does this)
        lastUpdateTime = now; // always update fallback time for state setter without time
        auto timeRemaining = checkConstraints(m_desiredState, now);
        if (timeRemaining > 1000) {
            return 1000;
        }
        if (timeRemaining == 0) {
            ActuatorDigitalChangeLogged::state(m_desiredState, now);
        }
        return timeRemaining;
    }
};

class MutexTarget {
public:
    MutexTarget() = default;
    ~MutexTarget() = default;
    std::mutex mut;

    duration_millis_t holdAfterTurnOff() const
    {
        return m_holdAfterTurnOff;
    }

    void holdAfterTurnOff(duration_millis_t v)
    {
        m_holdAfterTurnOff = v;
    }

    void timeRemaining(duration_millis_t v)
    {
        m_timeRemaining = v;
    }

    duration_millis_t timeRemaining() const
    {
        return m_timeRemaining;
    }

private:
    duration_millis_t m_timeRemaining = 0;
    duration_millis_t m_holdAfterTurnOff = 0;
};

namespace ADConstraints {
class MinOnTime : public BaseImpl<MinOnTime> {
private:
    duration_millis_t m_limit;

public:
    MinOnTime(const duration_millis_t& min)
        : m_limit(min)
    {
    }

    duration_millis_t allowedImpl(State& desiredState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged& act) override final;

    duration_millis_t limit()
    {
        return m_limit;
    }

    virtual uint8_t order() const override final
    {
        return 1;
    }
};

class MinOffTime : public BaseImpl<MinOffTime> {
private:
    duration_millis_t m_limit;

public:
    MinOffTime(const duration_millis_t& min)
        : m_limit(min)
    {
    }

    virtual duration_millis_t allowedImpl(State& desiredState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged& act) override final;

    duration_millis_t limit()
    {
        return m_limit;
    }

    virtual uint8_t order() const override final
    {
        return 0;
    }
};

class DelayedOn : public BaseImpl<DelayedOn> {
private:
    duration_millis_t m_limit;
    ticks_millis_t m_time_requested = 0;

public:
    DelayedOn(const duration_millis_t& delay)
        : m_limit(delay)
    {
    }

    duration_millis_t allowedImpl(State& desiredState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged&) override final;

    duration_millis_t limit()
    {
        return m_limit;
    }

    virtual uint8_t order() const override final
    {
        return 2;
    }
};

class DelayedOff : public BaseImpl<DelayedOff> {
private:
    duration_millis_t m_limit;
    ticks_millis_t m_time_requested = 0;

public:
    DelayedOff(const duration_millis_t& delay)
        : m_limit(delay)
    {
    }

    duration_millis_t allowedImpl(State& desiredState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged&) override final;

    duration_millis_t limit()
    {
        return m_limit;
    }

    virtual uint8_t order() const override final
    {
        return 3;
    }
};

class Mutex : public BaseImpl<Mutex> {
private:
    const std::function<std::shared_ptr<MutexTarget>()> m_mutexTarget;
    duration_millis_t m_holdAfterTurnOff;
    bool m_useCustomHoldDuration;
    // keep shared pointer to mutex, so it cannot be destroyed while locked
    std::shared_ptr<MutexTarget> m_lockedMutex;
    std::unique_lock<std::mutex> m_lock;

public:
    explicit Mutex(
        std::function<std::shared_ptr<MutexTarget>()>&& mut, duration_millis_t hold, bool useCustomHold)
        : m_mutexTarget(mut)
        , m_holdAfterTurnOff(hold)
        , m_useCustomHoldDuration(useCustomHold)
    {
    }
    virtual ~Mutex() = default;

    virtual duration_millis_t allowedImpl(State& desiredState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged& act) override final;

    auto holdAfterTurnOff() const
    {
        return m_holdAfterTurnOff;
    }

    void holdAfterTurnOff(duration_millis_t v)
    {
        m_holdAfterTurnOff = v;
    }

    bool useCustomHoldDuration() const
    {
        return m_useCustomHoldDuration;
    }

    void useCustomHoldDuration(bool v)
    {
        m_useCustomHoldDuration = v;
    }

    bool hasLock() const
    {
        return bool(m_lock);
    }

    virtual uint8_t order() const override final
    {
        return 4;
    }
};

class MaxOnTime : public BaseImpl<MaxOnTime> {
private:
    duration_millis_t m_limit;

public:
    MaxOnTime(const duration_millis_t& min)
        : m_limit(min)
    {
    }

    duration_millis_t allowedImpl(State& desiredState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged& act) override final;

    duration_millis_t limit()
    {
        return m_limit;
    }

    virtual uint8_t order() const override final
    {
        return 5;
    }
};
} // end namespace ADConstraints
