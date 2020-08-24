/*
 * Copyright 2015 BrewPi/Elco Jacobs.
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

#include "ActuatorAnalog.h"
#include "SetpointSensorPair.h"
#include <functional>
#include <memory>

/*
 * An analog actuator that sets a target to reference value + offset
 */
class ActuatorOffset final : public ActuatorAnalog {
public:
    enum ReferenceKind {
        SETTING,
        VALUE
    };

private:
    const std::function<std::shared_ptr<SetpointSensorPair>()> m_target;
    const std::function<std::shared_ptr<SetpointSensorPair>()> m_reference;
    value_t m_setting = 0;
    value_t m_value = 0;
    bool m_settingValid = false;
    bool m_valueValid = false;
    // separate flag for manually disabling the offset actuator
    bool m_enabled = true;

    ReferenceKind m_selectedReference = ReferenceKind::SETTING;

public:
    explicit ActuatorOffset(
        std::function<std::shared_ptr<SetpointSensorPair>()>&& target,    // process value to manipulate
        std::function<std::shared_ptr<SetpointSensorPair>()>&& reference) // process value to offset from
        : m_target(target)
        , m_reference(reference)
    {
    }
    ActuatorOffset(const ActuatorOffset&) = delete;
    ActuatorOffset& operator=(const ActuatorOffset&) = delete;

    virtual ~ActuatorOffset() = default;

    virtual void setting(value_t const& val) override final
    {
        m_setting = val;
        update();
    }

    virtual value_t setting() const override final
    {
        return m_setting;
    }

    // value() returns the actually achieved offset
    // By returning the actually achieved value, instead of the difference between the setpoints,
    // a PID can read back the actual actuator value and perform integrator anti-windup
    virtual value_t value() const override final
    {
        return m_value;
    }

    virtual bool valueValid() const override final
    {
        return m_valueValid;
    }

    virtual bool settingValid() const override final
    {
        return m_settingValid;
    }

    virtual void settingValid(bool v) override final
    {
        if (m_enabled) {

            if (auto targetPtr = m_target()) {
                targetPtr->settingValid(v);
            }
        }
    }

    void selectedReference(const ReferenceKind& sel)
    {
        m_selectedReference = sel;
    }

    ReferenceKind selectedReference() const
    {
        return m_selectedReference;
    }

    void update()
    {
        m_valueValid = false;
        m_value = 0;
        bool newTargetSettingValid = false;
        auto newTargetSetting = value_t(0);

        if (auto targetPtr = m_target()) {
            if (auto refPtr = m_reference()) {
                if (m_selectedReference == ReferenceKind::SETTING) {
                    if (refPtr->settingValid()) {
                        newTargetSetting = refPtr->setting() + m_setting;
                        newTargetSettingValid = true;
                        if (targetPtr->valueValid()) {
                            m_value = targetPtr->value() - refPtr->setting();
                            m_valueValid = true;
                        }
                    }
                } else {
                    if (refPtr->valueValid()) {
                        newTargetSetting = refPtr->value() + m_setting;
                        newTargetSettingValid = true;
                        if (targetPtr->valueValid()) {
                            m_value = targetPtr->value() - refPtr->value();
                            m_valueValid = true;
                        }
                    }
                }
            }
            if (newTargetSettingValid && m_enabled) {
                targetPtr->settingValid(true); // try to make target valid
                targetPtr->setting(newTargetSetting);
                m_settingValid = true;
            } else if (m_settingValid) {
                targetPtr->settingValid(false); // invalidate target once
                m_settingValid = false;
            }
        }
    }

    bool enabled() const
    {
        return m_enabled;
    }

    void enabled(bool v)
    {
        m_enabled = v;
    }
};
