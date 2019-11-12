#include "TicksWiring.h"
#include "Ticks.h"
#include "delay_hal.h"
#include "rtc_hal.h"
#include "timer_hal.h"

utc_seconds_t
TicksWiring::utc() const
{
    if (HAL_RTC_Time_Is_Valid(nullptr)) {
        return HAL_RTC_Get_UnixTime();
    }
    return 0;
}

void
TicksWiring::setUtc(const utc_seconds_t& t)
{
    HAL_RTC_Set_UnixTime(t);
}

ticks_millis_t
TicksWiring::millis() const
{
    return HAL_Timer_Get_Milli_Seconds();
}
ticks_micros_t
TicksWiring::micros() const
{
    return HAL_Timer_Get_Micro_Seconds();
}

void
TicksWiring::delayMillis(const duration_millis_t& duration) const
{
    HAL_Delay_Milliseconds(duration);
}
