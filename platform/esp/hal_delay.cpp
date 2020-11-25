#include "hal/hal_delay.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <sys/time.h>

// void delay_us(uint32_t duration)
// {
// }
void hal_delay_ms(uint32_t duration)
{
    vTaskDelay(duration / portTICK_PERIOD_MS);
}