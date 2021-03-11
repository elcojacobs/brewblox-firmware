#include "hal/hal_gpio.h"
#include "driver/gpio.h"

void hal_gpio_write(hal_pin_t pin, bool isHigh)
{
    gpio_set_level(gpio_num_t(pin), isHigh);
}

bool hal_gpio_read(hal_pin_t pin)
{
    return gpio_get_level(gpio_num_t(pin)) != 0;
}