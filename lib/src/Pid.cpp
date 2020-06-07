/*
 * Copyright 2015 BrewPi / Elco Jacobs
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

#include "../inc/Pid.h"
#include "../inc/future_std.h"

void
Pid::update()
{
    auto input = m_inputPtr();
    auto setpoint = in_t{0};
    if (input && input->settingValid() && input->valueValid()) {
        if (m_enabled) {
            active(true);
        }
        setpoint = input->setting();
        m_boilModeActive = setpoint >= in_t{100} + m_boilPointAdjust;
        m_error = input->error();

        checkFilterLength();
        m_derivative = input->readDerivative(m_derivativeFilterNr);
    } else {
        if (active()) {
            active(false);
        }
        m_integral = 0;
    }
    if (!active()) {
        return;
    }

    // calculate PID parts.

    m_p = m_kp * m_error;

    m_d = -m_kp * fp12_t(m_derivative * m_td);
    if ((m_kp >= 0 && m_d < -m_kp)
        || (m_kp <= 0 && m_d > -m_kp)) {
        m_d = -m_kp; // clip to -kp max, prevents large spikes
    }

    decltype(m_integral) integral_increase = 0;
    if (m_ti != 0 && m_kp != 0 && !m_boilModeActive) {
        integral_increase = cnl::quotient(m_p + m_d, m_kp);
        m_integral += integral_increase;
        m_i = m_integral * safe_elastic_fixed_point<4, 27>(cnl::quotient(m_kp, m_ti));
    } else {
        m_integral = integral_t{0};
        m_i = 0;
    }

    auto pidResult = m_p + m_i + m_d;

    out_t outputValue = pidResult;

    if (m_boilModeActive) {
        outputValue = std::max(outputValue, m_boilMinOutput);
    }

    // try to set the output to the desired setting
    if (m_enabled) {
        if (auto output = m_outputPtr()) {
            output->settingValid(true);
            output->setting(outputValue);

            if (m_boilModeActive) {
                return;
            }

            // get the clipped setting from the actuator for anti-windup
            if (output->settingValid()) {
                auto outputSetting = output->setting();

                if (m_ti != 0) { // 0 has been chosen to indicate that the integrator is disabled. This also prevents divide by zero.
                                 // update integral with anti-windup back calculation
                                 // pidResult - output is zero when actuator is not saturated

                    auto antiWindup = integral_t{0};
                    auto antiWindupValue = outputSetting; // P + I + D, clipped

                    if (m_kp != 0) { // prevent divide by zero
                        if (pidResult == outputSetting && output->valueValid()) {
                            // Actuator could be set to desired value, but might not reach set value due to physics or limits in its target actuator
                            // Get the actual achieved value in actuator. This could differ due to slowness time/mutex limits
                            antiWindupValue = output->value();
                        } else {
                            // else: clipped to actuator min or max set in target actuator
                            // calculate anti-windup from setting instead of actual value, so it doesn't dip under the maximum
                            // make sure anti-windup is at least the integral increase when clipping to prevent further windup
                            // Extra anti-windup can still be added below to reduce integral quicker
                            antiWindup += integral_increase;
                        }

                        out_t excess = cnl::quotient(pidResult - antiWindupValue, m_kp);
                        antiWindup += int8_t(3) * excess; // anti windup gain is 3
                    }
                    // make sure integral does not cross zero and does not increase by anti-windup
                    integral_t newIntegral = m_integral - antiWindup;
                    if (m_integral >= integral_t{0}) {
                        m_integral = std::clamp(newIntegral, integral_t{0}, m_integral);
                    } else {
                        m_integral = std::clamp(newIntegral, m_integral, integral_t{0});
                    }
                }
            }
        }
    }
}

void
Pid::kp(const in_t& arg)
{
    if (arg != 0) {
        // scale integral history so integral action doesn't change
        m_integral = m_integral * safe_elastic_fixed_point<15, 15>(cnl::quotient(m_kp, arg));
    }
    m_kp = arg;
}

void
Pid::ti(const uint16_t& arg)
{
    if (m_ti != 0) {
        // scale integral history so integral action doesn't change
        m_integral = cnl::wrap<integral_t>((int64_t(cnl::unwrap(m_integral)) * arg) / m_ti);
    }
    m_ti = arg;
}

void
Pid::td(const uint16_t& arg)
{
    m_td = arg;
    m_derivativeFilterNr = 0; // trigger automatic filter selection
    checkFilterLength();
}

void
Pid::setIntegral(const out_t& newIntegratorPart)
{
    if (m_kp == 0) {
        return;
    }
    m_integral = m_ti * safe_elastic_fixed_point<14, 16>(cnl::quotient(newIntegratorPart, m_kp));
}

void
Pid::checkFilterLength()
{

    // delay for each filter between input step and max derivative: 8, 34, 85, 188, 492, 1428
    const uint16_t limits[6] = {20, 89, 179, 359, 959, 1799};
    if (!m_derivativeFilterNr) {
        if (auto input = m_inputPtr()) {
            // selected filter must use an update interval a lot faster than Td to be meaningful
            // The filter delay is roughly 6x the update rate.
            m_derivativeFilterNr = 1;
            while (m_derivativeFilterNr < 6) {
                if (limits[m_derivativeFilterNr - 1] >= m_td) {
                    break;
                }
                ++m_derivativeFilterNr;
            };
            input->resizeFilterIfNeeded(m_derivativeFilterNr);
        }
    }
}