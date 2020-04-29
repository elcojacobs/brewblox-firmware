#include "ActuatorPwm.h"
#include "future_std.h"
#include <cstdint>

#if PLATFORM_ID != PLATFORM_GCC
#include "TimerInterrupts.h"
#endif

ActuatorPwm::ActuatorPwm(
    std::function<std::shared_ptr<ActuatorDigitalConstrained>()>&& target_,
    duration_millis_t period_)
    : m_target(target_)
{
    period(period_);
}

void
ActuatorPwm::setting(const value_t& val)
{
    if (val <= value_t{0}) {
        m_dutySetting = value_t{0};
        m_dutyTime = 0;
    } else if (val >= maxDuty()) {
        m_dutySetting = maxDuty();
        m_dutyTime = m_period;
    } else {
        m_dutySetting = val;
        auto unScaledTime = m_dutySetting * m_period;
        m_dutyTime = uint64_t(unScaledTime) / 100;
    }

    settingValid(true);
}

// returns the actual achieved PWM value, not the set value
ActuatorPwm::value_t
ActuatorPwm::value() const
{
    return m_dutyAchieved;
}

safe_elastic_fixed_point<2, 28>
ActuatorPwm::dutyFraction() const
{
    constexpr auto rounder = (cnl::numeric_limits<value_t>::min() >> 1);
    return safe_elastic_fixed_point<2, 28>{cnl::quotient(m_dutySetting + rounder, maxDuty())};
}

#if PLATFORM_ID != PLATFORM_GCC
void
ActuatorPwm::manageTimerTask()
{
    if (m_period < 1000 && m_enabled) {
        m_period = 100;
        if (!timerFuncId) {
            timerFuncId = TimerInterrupts::add([this]() { timerTask(); });
        }
    } else {
        if (timerFuncId) {
            TimerInterrupts::remove(timerFuncId);
            timerFuncId = 0;
            m_dutyAchieved = value_t{0};
        }
    }
}
#endif

void
ActuatorPwm::period(const duration_millis_t& p)
{
    m_period = p;
    if (auto actPtr = m_target()) {
        if (p < 1000 && !actPtr->supportsFastIo()) {
            m_period = 1000;
        }
    }
#if PLATFORM_ID != PLATFORM_GCC
    manageTimerTask();
#endif
}

duration_millis_t
ActuatorPwm::period() const
{
#if PLATFORM_ID != PLATFORM_GCC
    if (m_period < 1000) {
        return 10; // internally 100 is used for timer based pwm, but return 10ms, the actual period
    }
#endif
    return m_period;
}

#if PLATFORM_ID != PLATFORM_GCC
void
ActuatorPwm::timerTask()
{

    // timer clock is 10 kHz, 100 steps at 100Hz
    if (m_fastPwmElapsed == 0) {
        auto actPtr = m_target();
        if (m_dutyTime != 0 && actPtr) {
            if (actPtr->state() == State::Active) {
                m_dutyAchieved = maxDuty(); // was never low
            } else {
                actPtr->setStateUnlogged(State::Active);
            }
        } else {
            m_dutyAchieved = value_t{0};
        }
        m_fastPwmElapsed = 0;
    } else {
        if (m_fastPwmElapsed == m_dutyTime) {
            if (auto actPtr = m_target()) {
                actPtr->setStateUnlogged(State::Inactive);
                m_dutyAchieved = m_dutySetting;
            }
        }
    }
    m_fastPwmElapsed = m_fastPwmElapsed == 99 ? 0 : m_fastPwmElapsed + 1;
}

ActuatorPwm::update_t
ActuatorPwm::update(const update_t& now)
{
    if (timerFuncId) {
        return now + 1000;
    }
    return slowPwmUpdate(now);
}
#else
ActuatorPwm::update_t
ActuatorPwm::update(const update_t& now)
{
    return slowPwmUpdate(now);
}
#endif

ActuatorPwm::update_t
ActuatorPwm::slowPwmUpdate(const update_t& now)
{
    if (auto actPtr = m_target()) {
        auto durations = actPtr->activeDurations(now);
        auto currentHighTime = durations.currentActive;
        auto previousHighTime = durations.previousActive;
        auto previousPeriod = durations.previousPeriod;
        auto currentPeriod = durations.currentPeriod;
        auto lastHistoricState = durations.lastState;
        auto invDutyTime = m_period - m_dutyTime;

        auto wait = duration_millis_t(0);

        // limit history length taken into account to 2.5 periods duration.
        // 2.5 periods gives room to correct jitter, 2 periods is too tight at the end of the period
        // Note that future of this period is also counted later, so this implements 'max 2 periods in the past', not 2 periods total.

        // Scenario 1: current period is longer than 2*m_period. If value is 0% or 100%, limit history to 2 normal periods
        const uint32_t twoPeriods = 2 * m_period;
        if (currentPeriod > twoPeriods) {
            if (lastHistoricState == State::Active && m_dutySetting == maxDuty()) {
                currentPeriod = twoPeriods;
                if (currentHighTime > twoPeriods) {
                    // high for over 2 * m_period
                    currentHighTime = currentPeriod;
                }
                previousPeriod = 0;
                previousHighTime = 0;
            } else if (lastHistoricState == State::Inactive && m_dutySetting == 0) {
                if (currentPeriod > twoPeriods + currentHighTime) {
                    // low for over 2 * m_period
                    currentHighTime = 0;
                } else {
                    currentHighTime = twoPeriods - (currentPeriod - currentHighTime);
                }
                currentPeriod = twoPeriods;
                previousPeriod = 0;
                previousHighTime = 0;
            }
        }
        // scenario 2: both periods together are longer than 2.5 * m_period
        const uint32_t twoAndAHalfPeriods = 2 * m_period + (m_period >> 1);
        if (previousPeriod + currentPeriod > twoAndAHalfPeriods) {
            // compress the previous period, limit length to current period length
            // Combined with the adjustment below it will morph the previous period into a perfect desired period at current duty

            if (currentPeriod > previousPeriod) {
                if (lastHistoricState == State::Active) {
                    // limit low time of previous period (oldest history) to current low time
                    auto currentLowTime = currentPeriod - currentHighTime;
                    auto previousLowTime = previousPeriod - previousHighTime;
                    if (previousLowTime > currentLowTime) {
                        previousPeriod = previousHighTime + currentLowTime;
                    }
                } else if (lastHistoricState == State::Inactive) {
                    // limit high time of previous period (oldest history) to current high time
                    if (previousHighTime > currentHighTime) {
                        auto previousLowTime = previousPeriod - previousHighTime;
                        previousHighTime = currentHighTime;
                        previousPeriod = previousHighTime + previousLowTime;
                    }
                }
            }
        }

        // if previous period was shortened, lengthen it again with the state that would result it bringing duty closer to desired duty
        if (previousPeriod < m_period) {
            auto shortenedBy = m_period - previousPeriod;
            previousPeriod += shortenedBy;
            if (previousHighTime < m_dutyTime) {
                previousHighTime = std::min(previousHighTime + shortenedBy, m_dutyTime);
            }
        }

        auto twoPeriodElapsed = previousPeriod + currentPeriod;
        auto twoPeriodHighTime = previousHighTime + currentHighTime;

        if (lastHistoricState == State::Active) {
            if (m_dutySetting == maxDuty()) {
                auto actWait = actPtr->desiredState(State::Active, now); // ensure desired state is correct
                if (currentPeriod + 1000 <= m_period) {
                    wait = m_period - currentPeriod; // runs from high to 1000
                } else {
                    wait = 1000;
                }
                wait = std::max(actWait, wait);
            } else if (m_dutySetting <= (maxDuty() >> 1)) {
                // high period is fixed, low period adapts
                if (currentHighTime < m_dutyTime) {
                    wait = m_dutyTime - currentHighTime;
                }
            } else {
                // for checking the currently achieved value, for this cycle so far and the previous

                // high period can adapt between boundaries
                // maximum high time is the highest value among:
                // - 1.5x the previous high time
                // - 1.5x the normal high time
                // minimum high time is 75% of normal high time

                // use unadjusted time to calculate max time

                auto minHighTime = invDutyTime - (invDutyTime >> 2);
                if (currentHighTime < minHighTime) {
                    wait = minHighTime - currentHighTime;
                } else {
                    auto maxHighTime = std::max(m_dutyTime, durations.previousActive);
                    if (durations.previousPeriod >= m_period) {
                        maxHighTime += maxHighTime / 2; // stretching allowed if previous period was not shortened
                    }

                    if (currentHighTime < maxHighTime) {
                        // for checking the currently achieved value, look back max 2 periods (toggles)
                        auto twoPeriodTargetHighTime = duration_millis_t(twoPeriodElapsed * dutyFraction());

                        // make sure that periods following each other do not continuously alternate in shortend/stretched cycle
                        // by converging to the mean or unadjusted, whichever is higher
                        auto mean = std::max(m_dutyTime, twoPeriodTargetHighTime / 2);
                        if (currentHighTime > mean && previousHighTime < mean) {
                            twoPeriodTargetHighTime -= (currentHighTime - previousHighTime) / 4;
                        }

                        if (twoPeriodHighTime < twoPeriodTargetHighTime) {
                            wait = std::min(twoPeriodTargetHighTime - twoPeriodHighTime, maxHighTime - currentHighTime);
                        }
                    }
                }
            }
        } else if (lastHistoricState == State::Inactive) {
            auto currentLowTime = currentPeriod - currentHighTime;
            if (m_dutySetting == value_t{0}) {
                auto actWait = actPtr->desiredState(State::Inactive, now); // ensure desired state is correct
                if (currentPeriod + 1000 <= m_period) {
                    wait = m_period - currentPeriod; // runs from high to 1000
                } else {
                    wait = 1000;
                }
                wait = std::max(actWait, wait);
            } else if (m_dutySetting > (maxDuty() >> 1)) {
                // low period is fixed, high period adapts
                if (currentLowTime < invDutyTime) {
                    wait = invDutyTime - currentLowTime;
                }
            } else {
                // low period can adapt between boundaries
                // maximum low time is the highest value among:
                // - 1.5x the previous low time
                // - 1.5x the normal low time
                // minimum low time is 75% of normal low time

                auto minLowTime = invDutyTime - (invDutyTime >> 2);
                if (currentLowTime < minLowTime) {
                    wait = minLowTime - currentLowTime;
                } else {
                    auto maxLowTime = std::max(invDutyTime, durations.previousPeriod - durations.previousActive);
                    if (durations.previousPeriod >= m_period) {
                        maxLowTime += maxLowTime / 2; // stretching only allowed if previous period was not shortened
                    }

                    if (currentLowTime < maxLowTime) {
                        // for checking the currently achieved value, look back max 2 periods (toggles)
                        auto twoPeriodTargetLowTime = twoPeriodElapsed - duration_millis_t(dutyFraction() * twoPeriodElapsed);
                        auto previousLowTime = previousPeriod - previousHighTime;

                        // make sure that periods following each other do not continuously alternate in shortend/stretched cycle
                        // by converging to the mean or the unadjusted time, whichever is higher
                        auto mean = std::max(invDutyTime, twoPeriodTargetLowTime / 2);
                        if (currentLowTime > mean && previousLowTime < mean) {
                            twoPeriodTargetLowTime -= (currentLowTime - previousLowTime) / 4;
                        }
                        auto twoPeriodLowTime = twoPeriodElapsed - twoPeriodHighTime;
                        if (twoPeriodLowTime < twoPeriodTargetLowTime) {
                            wait = std::min(twoPeriodTargetLowTime - twoPeriodLowTime, maxLowTime - currentLowTime);
                        }
                    }
                }
            }
        }

        // Toggle actuator if necessary
        if (m_enabled && m_settingValid && (wait == 0)) {
            // if actuator is blocked, update wait time to prevent useless pwm updates
            if (lastHistoricState == State::Inactive) {
                wait = actPtr->desiredState(State::Active, now);
            } else {
                wait = actPtr->desiredState(State::Inactive, now);
            }
            // update state in case we toggled
            lastHistoricState = actPtr->state();
        }

        // take into account period until next update in calculating achieved
        auto twoPeriodTotal = twoPeriodElapsed + wait;
        if (lastHistoricState == State::Active) {
            twoPeriodHighTime += wait;
        }

        auto dutyAchieved = value_t{cnl::quotient(100 * twoPeriodHighTime, twoPeriodTotal)};
        // calculate achieved duty cycle
        // only update achieved if current cycle is long enough
        // to let current state move achieved in the right direction
        m_valueValid = true;

        if (lastHistoricState == State::Active) {
            if (dutyAchieved >= m_dutyAchieved) {
                m_dutyAchieved = dutyAchieved;
            }
        } else if (lastHistoricState == State::Inactive) {
            if (dutyAchieved <= m_dutyAchieved) {
                m_dutyAchieved = dutyAchieved;
            }
        } else {
            m_valueValid = false;
            m_dutyAchieved = m_dutySetting;
        }

        return now + std::min(update_t(1000), (wait >> 1) + 1);
    }
    return now + 1000;
}

bool
ActuatorPwm::valueValid() const
{
    return m_valueValid;
}

bool
ActuatorPwm::settingValid() const
{
    return m_settingValid;
}

void
ActuatorPwm::settingValid(bool v)
{
    if (!v && m_enabled) {
        if (auto actPtr = m_target()) {
            actPtr->desiredState(State::Inactive);
        }
    }
    m_settingValid = v;
}
