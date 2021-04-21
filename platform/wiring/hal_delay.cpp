#include "delay_hal.h"

void hal_delay_us(uint32_t duration)
{
    HAL_Delay_Microseconds(duration);
}

void hal_delay_ms(uint32_t duration)
{
    HAL_Delay_Milliseconds(duration);
}