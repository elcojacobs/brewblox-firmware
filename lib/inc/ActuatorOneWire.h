/*
 * Copyright 2013 BrewPi/Elco Jacobs.
 * Copyright 2013 Matthew McGowan
 *
 * This file is part of BrewPi.
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

#include "ActuatorDigital.h"
#include "DS2413.h"
#include <memory>

/*
 * An actuator or sensor that operates by communicating with a DS2413 device.
 *
 */
class ActuatorOneWire final : public ActuatorDigital {
private:
    const std::function<std::shared_ptr<DS2413>()> m_device;
    bool m_invert = true;
    DS2413::Pio m_channel = DS2413::Pio::UNSET;

public:
    explicit ActuatorOneWire(std::function<std::shared_ptr<DS2413>()>&& device)
        : m_device(device)
    {
    }
    ~ActuatorOneWire() = default;

    virtual void state(const State& state) override final
    {
        bool bitVal = (state == State::Active) ^ m_invert;
        if (m_channel != DS2413::Pio::UNSET) {
            if (auto devPtr = m_device()) {
                devPtr->writeLatchBit(m_channel, bitVal, true);
            }
        }
    }

    virtual State state() const override final
    {
        bool result;
        if (m_channel != DS2413::Pio::UNSET) {
            if (auto devPtr = m_device()) {
                if (devPtr->latchReadCached(m_channel, result)) {
                    return (result ^ m_invert) ? State::Active : State::Inactive;
                }
            }
        }
        return State::Unknown;
    }

    State sense()
    {
        if (m_channel != DS2413::Pio::UNSET) {
            if (auto devPtr = m_device()) {
                if (devPtr->writeLatchBit(m_channel, false, false)) {
                    bool result;
                    if (devPtr->readLatchBit(m_channel, result, false)) {
                        return (result ^ m_invert) ? State::Active : State::Inactive;
                    }
                }
            }
        }
        return State::Unknown;
    }

    void update()
    {
        if (m_channel != DS2413::Pio::UNSET) {
            if (auto devPtr = m_device()) {
                devPtr->update();
            }
        }
    }

    bool invert() const
    {
        return m_invert;
    }

    void invert(bool inv)
    {
        auto active = state();
        m_invert = inv;
        state(active);
    }

    DS2413::Pio channel() const
    {
        return m_channel;
    }

    void channel(DS2413::Pio newChannel)
    {
        m_channel = newChannel;
    }
};
