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

#include "FixedPoint.h"
#include "FpFilterChain.h"
#include "ProcessValue.h"
#include "TempSensor.h"
#include "Temperature.h"
#include <functional>
#include <memory>

/*
 * A process value has a setting and an current value
 */
class SetpointSensorPair : public ProcessValue<temp_t> {
public:
    using derivative_t = safe_elastic_fixed_point<1, 23>;

private:
    temp_t m_setting = 20;
    bool m_settingEnabled = false;
    const std::function<std::shared_ptr<TempSensor>()> m_sensor;
    FpFilterChain<temp_t> m_filter;
    uint8_t m_sensorFailureCount = 255; // force a reset on init
    uint8_t m_filterNr = 1;

public:
    explicit SetpointSensorPair(
        std::function<std::shared_ptr<TempSensor>()>&& _sensor)
        : m_sensor(_sensor)
        , m_filter(1)
    {
        update();
    }

    SetpointSensorPair(const SetpointSensorPair&) = delete;
    SetpointSensorPair& operator=(const SetpointSensorPair&) = delete;

    virtual ~SetpointSensorPair() = default;

    virtual void setting(temp_t const& setting) override final
    {
        m_setting = setting;
    }

    virtual temp_t setting() const override final
    {
        return m_setting;
    }

    virtual temp_t value() const override final
    {
        if (m_filterNr == 0) {
            return m_filter.readLastInput();
        }
        return m_filter.read(m_filterNr - 1);
    }

    temp_t valueUnfiltered() const
    {
        if (auto sPtr = m_sensor()) {
            return sPtr->value();
        } else {
            return 0;
        }
    }

    bool valueValid() const override final
    {
        return m_sensorFailureCount <= 10;
    }

    bool sensorValid() const
    {
        if (auto sens = m_sensor()) {
            return sens->valid();
        }
        return false;
    }

    bool settingValid() const override final
    {
        return m_settingEnabled;
    }

    virtual void settingValid(bool v) override final
    {
        m_settingEnabled = v;
    }

    auto filterChoice() const
    {
        return m_filterNr;
    }

    auto filterThreshold() const
    {
        return m_filter.getStepThreshold();
    }

    void filterChoice(uint8_t choice)
    {
        m_filterNr = choice;
    }

    auto filterThreshold(temp_t threshold)
    {
        if (threshold == 0) {
            threshold = 5;
        }
        m_filter.setStepThreshold(threshold);
    }

    void update()
    {
        if (sensorValid()) {
            auto val = valueUnfiltered();
            if (!valueValid()) {
                m_filter.reset(val);
            }
            m_filter.add(val);
            m_sensorFailureCount = 0;
        } else {
            if (m_sensorFailureCount < 255) {
                m_sensorFailureCount++;
            }
        }
    }

    auto error()
    {
        return setting() - value();
    }

    auto readDerivative(uint8_t filterNr)
    {
        if (filterNr < 1) {
            filterNr = 1;
        }
        return m_filter.readDerivative<derivative_t>(filterNr - 1);
    }

    auto filterLength()
    {
        return m_filter.length();
    }

    auto resizeFilterIfNeeded(uint8_t filterNr)
    {
        m_filter.expandStages(filterNr);
    }

    void resetFilter()
    {
        if (sensorValid()) {
            m_filter.reset(valueUnfiltered());
            m_sensorFailureCount = 0;
        }
    }
};
