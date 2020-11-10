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

#include "ActuatorDigitalConstrained.h"

namespace ADConstraints {

duration_millis_t MinOnTime::allowedImpl(State& desiredState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged& act)
{
    if (act.state() != State::Active) {
        return 0;
    }
    if (desiredState == State::Active) {
        return 0;
    }
    auto times = act.getLastStartEndTime(State::Active, now);
    auto elapsedOn = times.end - times.start;

    if (elapsedOn >= m_limit) {
        return 0;
    }

    return m_limit - elapsedOn;
}

duration_millis_t MinOffTime::allowedImpl(State& desiredState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged& act)
{
    if (act.state() != State::Inactive) {
        return 0;
    }

    if (desiredState == State::Inactive) {
        return 0;
    }

    auto times = act.getLastStartEndTime(State::Inactive, now);
    auto elapsedOff = times.end - times.start;

    if (elapsedOff >= m_limit) {
        return 0;
    }

    return m_limit - elapsedOff;
}

duration_millis_t DelayedOn::allowedImpl(State& desiredState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged&)
{
    if (desiredState == State::Active) {
        if (m_time_requested == 0) {
            m_time_requested = now != 0 ? now : -1;
        }
        auto elapsed = now - m_time_requested;
        auto wait = (m_limit > elapsed) ? m_limit - elapsed : 0;
        return wait;
    }

    m_time_requested = 0;
    return 0;
}

duration_millis_t DelayedOff::allowedImpl(State& desiredState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged&)
{
    if (desiredState == State::Inactive) {
        if (m_time_requested == 0) {
            m_time_requested = now != 0 ? now : -1;
        }
        auto elapsed = now - m_time_requested;
        auto wait = (m_limit > elapsed) ? m_limit - elapsed : 0;
        return wait;
    }

    m_time_requested = 0;
    return 0;
}

duration_millis_t Mutex::allowedImpl(State& desiredState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged& act)
{
    if (desiredState == State::Inactive) {
        if (m_lock) {
            // Release lock if actuator has been off for minimal time
            auto elapsedMinimal = m_useCustomHoldDuration ? m_holdAfterTurnOff : m_lockedMutex->holdAfterTurnOff();
            duration_millis_t elapsedOff = 0;
            if (act.state() == State::Inactive) {
                auto times = act.getLastStartEndTime(State::Inactive, now);
                elapsedOff = times.end - times.start;
            }
            if (elapsedOff < elapsedMinimal) {
                auto wait = elapsedMinimal - elapsedOff;
                m_lockedMutex->timeRemaining(wait);
                if (act.state() != State::Inactive) {
                    return 0; // turning off is allowed, but mutex stays locked
                } else {
                    return wait; // no toggle needed, return wait time to prevent unneccessary updates until it has passed
                }
            }
            m_lockedMutex->timeRemaining(0);
            m_lockedMutex.reset();
            m_lock.unlock();
            return 0;
        }
        // not locked, but no lock needed
        return 0;
    }
    if (desiredState == State::Active) {
        if (m_lock) {
            auto elapsedMinimal = m_useCustomHoldDuration ? m_holdAfterTurnOff : m_lockedMutex->holdAfterTurnOff();
            m_lockedMutex->timeRemaining(elapsedMinimal);
            if (act.state() != State::Active) {
                return 0;
            } else {
                return elapsedMinimal; // no action needed. Return minimum off time to prevent unncessary updates
            }
        }
        auto mutex = m_mutexTarget();
        if (mutex) {
            m_lock = std::unique_lock<std::mutex>(mutex->mut, std::try_to_lock);
            if (m_lock) {
                // successfully aquired lock of target
                // store shared pointer to target so it can't be deleted while locked
                m_lockedMutex = std::move(mutex);
                return 0;
            }
            auto wait = mutex->timeRemaining() + 1;
            return wait;
        }
    }
    return 1000;
}

duration_millis_t MaxOnTime::allowedImpl(State& desiredState, const ticks_millis_t& now, const ActuatorDigitalChangeLogged& act)
{

    if (act.state() != State::Active || desiredState != State::Active) {
        return 0;
    }

    auto times = act.getLastStartEndTime(State::Active, now);
    auto elapsedOn = times.end - times.start;

    if (elapsedOn >= m_limit) {
        desiredState = State::Inactive;
        return 0;
    }
    return m_limit - elapsedOn;
}
}