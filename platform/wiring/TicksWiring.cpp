#include "TicksWiring.h"
#include "Ticks.h"
#include "delay_hal.h"
#include "timer_hal.h"

ticks_seconds_t
TicksWiring::seconds()
{
    return HAL_Timer_Get_Micro_Seconds() / 1000;
}
ticks_millis_t
TicksWiring::millis()
{
    return HAL_Timer_Get_Milli_Seconds();
}
ticks_micros_t
TicksWiring::micros()
{
    return HAL_Timer_Get_Micro_Seconds();
}

void
TicksWiring::delayMillis(const duration_millis_t& duration)
{
    HAL_Delay_Milliseconds(duration);
}
