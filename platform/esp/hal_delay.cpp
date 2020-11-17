#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/hal_delay.h"
#include <sys/time.h>

// void delay_us(uint32_t duration)
// {
// }
void delay_ms(uint32_t duration)
{
    vTaskDelay(duration / portTICK_PERIOD_MS);
}