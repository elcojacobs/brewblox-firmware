#include "TicksEsp.h"
#include "Ticks.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <sys/time.h>

utc_seconds_t
TicksEsp::utc() const
{

    timeval tv_now;
    gettimeofday(&tv_now, NULL);
    return tv_now.tv_sec > 1600000000 ? tv_now.tv_sec : 0;
}

void
TicksEsp::setUtc(const utc_seconds_t& t)
{
    timeval tv_now;
    tv_now.tv_sec = t;
    tv_now.tv_usec = 0;
    settimeofday(&tv_now, nullptr);
}

ticks_millis_t
TicksEsp::millis() const
{
    return esp_timer_get_time() / 1000;
}
ticks_micros_t
TicksEsp::micros() const
{
    return esp_timer_get_time();
}

void
TicksEsp::delayMillis(const duration_millis_t& duration) const
{
    vTaskDelay(duration / portTICK_PERIOD_MS);
}
