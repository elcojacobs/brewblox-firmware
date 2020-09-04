/*
 * Copyright 2019 BrewPi B.V.
 *
 * This file is part of BrewBlox.
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
#include "DS2408.h"
#include <functional>
#include <memory>

/*
 * A digital actuator that toggles a channel of an ArrayIo object.
 *
 */
class MotorValve : public ActuatorDigitalBase {

public:
    enum class ValveState : uint8_t {
        Unknown = 0,
        Open = 1,
        Closed = 2,
        Opening = 3,
        Closing = 4,
        HalfOpenIdle = 5,
        InitIdle = 6,
    };

    static const uint8_t chanIsClosed = 0;
    static const uint8_t chanIsOpen = 1;
    static const uint8_t chanOpeningHigh = 2;
    static const uint8_t chanClosingHigh = 3;

private:
    const std::function<std::shared_ptr<DS2408>()> m_target;
    uint8_t m_startChannel = 0;
    uint8_t m_desiredChannel = 0;

    ValveState m_desiredValveState = ValveState::InitIdle;
    ValveState m_actualValveState = ValveState::InitIdle;

public:
    explicit MotorValve(std::function<std::shared_ptr<DS2408>()>&& target, uint8_t startChan)
        : m_target(target)
    {
        startChannel(startChan);
    }
    MotorValve(const MotorValve&) = delete;
    MotorValve& operator=(const MotorValve&) = delete;

    virtual ~MotorValve()
    {
        startChannel(0); // release channels before destruction
    }

    virtual void state(const State& v) override final;
    virtual State state() const override final;

    ValveState valveState() const
    {
        return m_actualValveState;
    }

    void applyValveState(ValveState v, std::shared_ptr<DS2408>& devPtr);

    ValveState getValveState(const std::shared_ptr<DS2408>& devPtr) const;

    void update();

    uint8_t startChannel() const
    {
        return m_desiredChannel;
    }

    bool channelReady() const
    {
        return m_desiredChannel == m_startChannel;
    }

    void startChannel(uint8_t newChannel)
    {
        m_desiredChannel = newChannel;
        update();
    }

    void claimChannel();

    virtual bool
    supportsFastIo() const override final
    {
        return false;
    }
};
