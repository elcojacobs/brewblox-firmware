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

class TimedMutex {
private:
    std::mutex m_mutex;
    duration_millis_t m_differentActuatorWait = 0;
    duration_millis_t m_waitRemaining = 0;
    ticks_millis_t lastActive = 0;

    ticks_millis_t lastUpdate = 0;
    const ActuatorDigitalChangeLogged* lastActuator = nullptr;

public:
    TimedMutex() = default;
    ~TimedMutex() = default;

    bool try_lock(const ActuatorDigitalChangeLogged& act)
    {
        auto allowed = m_mutex.try_lock();
        if (allowed && lastActuator != &act && lastActuator != nullptr) {
            // also check minimum wait time if last actuator that was active was not requester
            if (m_waitRemaining) {
                m_mutex.unlock();
                allowed = false;
            }
        }
        return allowed;
    }

    void unlock(const ActuatorDigitalChangeLogged& act)
    {
        if (act.state() == ActuatorDigitalChangeLogged::State::Active) {
            lastActive = lastUpdate;
            lastActuator = &act;
            update(lastUpdate);
        }
        m_mutex.unlock();
    }

    duration_millis_t differentActuatorWait() const
    {
        return m_differentActuatorWait;
    }

    duration_millis_t waitRemaining() const
    {
        return m_waitRemaining;
    }

    void differentActuatorWait(const duration_millis_t& v)
    {
        m_differentActuatorWait = v;
    }

    void update(const ticks_millis_t& now)
    {
        lastUpdate = now;
        auto elapsed = now - lastActive;
        if (lastActuator == nullptr || elapsed > m_differentActuatorWait) {
            m_waitRemaining = 0;
        } else {
            m_waitRemaining = m_differentActuatorWait - elapsed;
        }
    }
};

namespace ADConstraints {
using State = ActuatorDigitalBase::State;

class Base {
public:
    Base() = default;
    virtual ~Base() = default;

    virtual bool allowed(const State& newState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged& act) = 0;

    virtual uint8_t id() const = 0;

    virtual uint8_t order() const = 0;
};

template <uint8_t ID>
class MinOnTime : public Base {
private:
    duration_millis_t m_limit;

public:
    MinOnTime(const duration_millis_t& min)
        : m_limit(min)
    {
    }

    bool allowed(const State& newState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged& act) override final
    {
        if (act.state() != State::Active) {
            return true;
        }
        auto times = act.getLastStartEndTime(State::Active, now);
        return newState == State::Active || times.end - times.start >= m_limit;
    }

    virtual uint8_t id() const override final
    {
        return ID;
    }

    duration_millis_t limit()
    {
        return m_limit;
    }

    virtual uint8_t order() const override final
    {
        return 1;
    }
};

template <uint8_t ID>
class MinOffTime : public Base {
private:
    duration_millis_t m_limit;

public:
    MinOffTime(const duration_millis_t& min)
        : m_limit(min)
    {
    }

    virtual bool allowed(const State& newState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged& act) override final
    {
        if (act.state() != State::Inactive) {
            return true;
        }
        auto times = act.getLastStartEndTime(State::Inactive, now);
        return newState == State::Inactive || times.end - times.start >= m_limit;
    }

    virtual uint8_t id() const override final
    {
        return ID;
    }

    duration_millis_t limit()
    {
        return m_limit;
    }

    virtual uint8_t order() const override final
    {
        return 0;
    }
};

template <uint8_t ID>
class Mutex : public Base {
private:
    const std::function<std::shared_ptr<TimedMutex>()> m_mutex;
    bool hasLock = false;

public:
    explicit Mutex(
        std::function<std::shared_ptr<TimedMutex>()>&& mut)
        : m_mutex(mut)
    {
    }
    ~Mutex() = default;

    virtual bool allowed(const State& newState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged& act) override final
    {
        if (newState == State::Inactive) {
            // always allow switching OFF, but release mutex
            if (act.state() == State::Active || hasLock) {
                if (auto mutPtr = m_mutex()) {
                    mutPtr->unlock(act);
                    hasLock = false;
                }
            }
            return true;
        }

        if (newState == State::Active) {
            if (hasLock) {
                return true; // already owner of lock
            }

            if (auto mutPtr = m_mutex()) {
                if (act.state() != State::Active && newState == State::Active) {
                    // if turning on, try to acquire mutex
                    hasLock = mutPtr->try_lock(act);
                    return hasLock;
                }
            }
        }
        return false;
    }

    virtual uint8_t id() const override final
    {
        return ID;
    }

    virtual uint8_t order() const override final
    {
        return 2;
    }
};

} // end namespace ADConstraints

class ActuatorDigitalConstrained : private ActuatorDigitalChangeLogged {
public:
    using Constraint = ADConstraints::Base;

private:
    std::vector<std::unique_ptr<Constraint>> constraints;
    uint8_t m_limiting = 0x00;
    State m_unconstrained = State::Inactive;

public:
    ActuatorDigitalConstrained(ActuatorDigitalBase& act)
        : ActuatorDigitalChangeLogged(act){};

    ActuatorDigitalConstrained(const ActuatorDigitalConstrained&) = delete;
    ActuatorDigitalConstrained& operator=(const ActuatorDigitalConstrained&) = delete;
    ActuatorDigitalConstrained(ActuatorDigitalConstrained&&) = default;
    ActuatorDigitalConstrained& operator=(ActuatorDigitalConstrained&&) = default;

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

    void removeAllConstraints()
    {
        constraints.clear();
    }

    void resetHistory()
    {
        ActuatorDigitalChangeLogged::resetHistory();
    }

    uint8_t checkConstraints(const State& val, const ticks_millis_t& now)
    {
        uint8_t limiting = 0x00;
        uint8_t bit = 0x01;
        for (auto& c : constraints) {
            if (!c->allowed(val, now, *this)) {
                limiting = limiting | bit;
                break;
            }
            bit = bit << 1;
        }
        return limiting;
    }

    uint8_t limiting() const
    {
        return m_limiting;
    }

    void state(const State& val, const ticks_millis_t& now)
    {
        lastUpdateTime = now; // always update fallback time for state setter without time
        m_unconstrained = val;
        m_limiting = checkConstraints(val, now);
        if (m_limiting == 0) {
            ActuatorDigitalChangeLogged::state(val, now);
        }
    }

    void state(const State& val)
    {
        state(val, lastUpdateTime);
    }

    State state() const
    {
        return ActuatorDigitalChangeLogged::state();
    }

    void update(const ticks_millis_t& now)
    {
        state(m_unconstrained, now); // re-apply constraints for new update time
    }

    State unconstrained() const
    {
        return m_unconstrained;
    }

    const std::vector<std::unique_ptr<Constraint>>& constraintsList() const
    {
        return constraints;
    };
};
