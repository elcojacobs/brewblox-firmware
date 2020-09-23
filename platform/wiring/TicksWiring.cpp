#include "TicksWiring.h"
#include "Ticks.h"
#include "delay_hal.h"
#include "rtc_hal.h"
#include "timer_hal.h"

utc_seconds_t
TicksWiring::utc() const
{
    if (hal_rtc_time_is_valid(nullptr)) {
        timeval time;
        hal_rtc_get_time(&time, nullptr);
        return time.tv_sec;
    }
    return 0;
}

void
TicksWiring::setUtc(const utc_seconds_t& t)
{
    timeval time;
    time.tv_sec = t;
    time.tv_usec = 0;
    hal_rtc_set_time(&time, nullptr);
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
