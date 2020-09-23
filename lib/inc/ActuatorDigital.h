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

#include "ActuatorDigitalBase.h"
#include <cstdint>
#include <functional>
#include <memory>

class IoArray;

/*
 * A digital actuator that toggles a channel of an ArrayIo object.
 *
 */
class ActuatorDigital : public ActuatorDigitalBase {
private:
    const std::function<std::shared_ptr<IoArray>()> m_target;
    bool m_invert = false;
    uint8_t m_channel = 0;
    uint8_t m_desiredChannel = 0;

public:
    explicit ActuatorDigital(std::function<std::shared_ptr<IoArray>()>&& target, uint8_t chan)
        : m_target(target)
    {
        channel(chan);
    }

    ActuatorDigital(const ActuatorDigital&) = delete;
    ActuatorDigital& operator=(const ActuatorDigital&) = delete;

    virtual ~ActuatorDigital()
    {
        channel(0); // release channel before destruction
    }

    virtual void state(const State& v) override final;

    virtual State state() const override final;

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
        return m_desiredChannel;
    }

    void claimChannel();

    bool channelReady() const
    {
        return m_desiredChannel == m_channel;
    }

    void update()
    {
        if (!channelReady()) {
            // Periodic retry to claim channel in case target didn't exist
            // at earlier tries
            claimChannel();
        }
    }

    void channel(uint8_t newChannel)
    {
        m_desiredChannel = newChannel;
        update();
    }

    virtual bool supportsFastIo() const override final;
};
