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
ActuatorPwm::setting(value_t const& val)
{
    m_dutySetting = std::clamp(val, value_t(0), value_t(100));
    m_dutyTime = duration_millis_t((m_dutySetting * m_period) / value_t(100));
    settingValid(true);
}

// returns the actual achieved PWM value, not the set value
ActuatorPwm::value_t
ActuatorPwm::value() const
{
    return m_dutyAchieved;
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
            m_dutyAchieved = 0;
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
    if (auto actPtr = m_target()) {
        if (actPtr->state() != State::Active) {
            if (m_fastPwmElapsed < m_dutyTime) {
                actPtr->setStateUnlogged(State::Active);
            }
            if (m_fastPwmElapsed == 1) {
                m_dutyAchieved = 0; // never active in previous cycle
            }
        } else {
            if (m_fastPwmElapsed >= m_dutyTime) {
                actPtr->setStateUnlogged(State::Inactive);
                m_dutyAchieved = m_fastPwmElapsed;
            } else {
                if (m_fastPwmElapsed == 99) {
                    m_dutyAchieved = 100; // never inactive in this cycle
                }
            }
        }
    }
    m_fastPwmElapsed = (m_fastPwmElapsed + 1) % 100;
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
        auto twoPeriodTotalTime = durations.previousPeriod + durations.currentPeriod;
        auto twoPeriodHighTime = durations.previousActive + durations.currentActive;
        auto twoPeriodLowTime = twoPeriodTotalTime - twoPeriodHighTime;
        auto currentHighTime = durations.currentActive;
        auto currentPeriod = durations.currentPeriod;
        auto previousPeriod = durations.previousPeriod;
        auto previousHighTime = durations.previousActive;
        auto previousLowTime = previousPeriod - previousHighTime;
        auto wait = duration_millis_t(0);
        auto currentState = actPtr->state();
        auto invDutyTime = m_period - m_dutyTime;

        if (currentState == State::Active) {
            if (m_dutySetting == value_t(100)) {
                m_dutyAchieved = 100;
                return now + 1000;
            }

            if (m_dutySetting <= value_t(50)) {
                // high period is fixed, low period adapts
                if (currentHighTime < m_dutyTime) {
                    wait = m_dutyTime - currentHighTime;
                }
            } else {
                // for checking the currently achieved value, for this cycle so far and the previous
                auto twoPeriodTargetHighTime = duration_millis_t(twoPeriodTotalTime * (m_dutySetting / 100));

                // high period can adapt between boundaries
                // maximum high time is the highest value among:
                // - 1.5x the previous hight time
                // - 1.5x the normal high time
                auto maxHighTime = std::max(m_dutyTime, previousHighTime) * 3 / 2;

                // make sure that periods following each other do not continuously alternate in shortend/stretched cycle
                // by converging to the mean or unadjusted, whichever is higher
                auto mean = std::max(m_dutyTime, twoPeriodTargetHighTime / 2);
                if (currentHighTime > mean && previousHighTime < mean) {
                    twoPeriodTargetHighTime -= (currentHighTime - previousHighTime) / 4;
                }

                if (currentHighTime < maxHighTime) {
                    if (twoPeriodHighTime < twoPeriodTargetHighTime) {
                        wait = std::min(twoPeriodTargetHighTime - twoPeriodHighTime, maxHighTime - currentHighTime);
                    }
                }
            }
            m_valueValid = true;
        } else if (currentState == State::Inactive) {
            if (m_dutySetting == value_t(0)) {
                m_dutyAchieved = 0;
                return now + 1000;
            }
            // for checking the currently achieved value, look back max 2 periods (toggles)
            auto twoPeriodTargetLowTime = duration_millis_t(twoPeriodTotalTime * ((value_t(100) - m_dutySetting) / 100));

            auto thisPeriodLowTime = currentPeriod - currentHighTime;

            if (m_dutySetting > value_t(50)) {
                // low period is fixed, high period adapts
                if (thisPeriodLowTime < invDutyTime) {
                    wait = invDutyTime - thisPeriodLowTime;
                }
            } else {
                // low period can adapt between boundaries
                // maximum low time is the highest value among:
                // - 1.5x the previous low time
                // - 1.5x the normal low time
                auto maxLowTime = std::max(invDutyTime, previousLowTime) * 3 / 2;

                // make sure that periods following each other do not continuously alternate in shortend/stretched cycle
                // by converging to the mean or the unadjusted time, whichever is higher
                auto mean = std::max(invDutyTime, twoPeriodTargetLowTime / 2);
                if (thisPeriodLowTime > mean && previousLowTime < mean) {
                    twoPeriodTargetLowTime -= (thisPeriodLowTime - previousLowTime) / 4;
                }

                if (thisPeriodLowTime < maxLowTime) {
                    if (twoPeriodLowTime < twoPeriodTargetLowTime) {
                        wait = std::min(twoPeriodTargetLowTime - twoPeriodLowTime, maxLowTime - thisPeriodLowTime);
                    }
                }
            }
            m_valueValid = true;
        } else {
            m_valueValid = false;
        }

        bool toggled = false;

        // Toggle actuator if necessary
        if (m_enabled && m_settingValid && wait == 0) {
            if (currentState == State::Inactive) {
                actPtr->desiredState(State::Active, now);
            } else {
                actPtr->desiredState(State::Inactive, now);
            }
            if (currentState != actPtr->state()) {
                toggled = true;
            } else if (currentState == State::Inactive && m_dutySetting < 5) {
                // for duty cycle under 5%, set output to inactive explicitly to cancel any pending active state
                // when the toggle was blocked. This prevents a low duty cycle to create a lingering pending active state in the mutex
                // The PWM will continue to try to activate the pin
                actPtr->desiredState(State::Inactive, now);
                // increase wait time to not keep retrying each millisecond
                wait = std::min(duration_millis_t(1000), m_period >> 5);
            }
        }

        twoPeriodTotalTime += wait;
        if (twoPeriodTotalTime == 0) {
            m_dutyAchieved = 0;
        } else {
            // calculate achieved duty cycle
            auto dutyAchieved = (value_t(100) * twoPeriodHighTime) / twoPeriodTotalTime;
            if (toggled // end of high or low time or
                        // current period is long enough to start using the current achieved value including this period
                || (currentState == State::Inactive && dutyAchieved < m_dutyAchieved)
                || (currentState == State::Active && dutyAchieved > m_dutyAchieved)) {
                m_dutyAchieved = dutyAchieved;
            }
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
