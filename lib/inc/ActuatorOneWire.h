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
#include "IoArray.h"
#include <functional>
#include <memory>

/*
 * An actuator that operates by communicating with a OneWire IO externder (DS2413/DS2408).
 *
 */
class ActuatorOneWire final : public ActuatorDigital {
private:
    const std::function<std::shared_ptr<IoArray>()> m_target;
    bool m_invert = true;
    uint8_t m_channel = 0;

public:
    explicit ActuatorOneWire(std::function<std::shared_ptr<IoArray>()>&& target)
        : m_target(target)
    {
    }
    ~ActuatorOneWire() = default;

    virtual void state(const State& state) override final
    {
        if (auto devPtr = m_target()) {
            auto newState = state;
            if (m_invert) {
                newState = invertState(state);
            }
            IoArray::ChannelConfig config = newState == State::Active ? IoArray::ChannelConfig::ACTIVE_HIGH : IoArray::ChannelConfig::ACTIVE_HIGH;
            devPtr->writeChannel(m_channel, config);
        }
    }

    virtual State state() const override final
    {
        if (auto devPtr = m_target()) {
            State result;
            if (devPtr->senseChannel(m_channel, result)) {
                if (m_invert) {
                    result = invertState(result);
                }
                return result;
            }
        }

        return State::Unknown;
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

    uint8_t channel() const
    {
        return m_channel;
    }

    void channel(uint8_t newChannel)
    {
        m_channel = newChannel;
    }
};
