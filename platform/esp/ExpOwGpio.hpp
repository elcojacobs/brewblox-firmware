#include "DRV8908.hpp"
#include "TCA9538.hpp"
#include "esp_err.h"
#include "esp_log.h"
#include "hal/hal_delay.h"

class ExpOwGpio {
public:
    ExpOwGpio(uint8_t lower_address)
        : expander(lower_address)
        , gpio(
              0, -1,
              [this]() { expander.set_output(0, false); },
              [this]() { expander.set_output(0, true); })
    {
        expander.set_outputs(0b11111101);
        expander.set_config(0b11111000);
    }

    void gpio_status()
    {
        uint8_t result = 0xFF;
        ESP_ERROR_CHECK_WITHOUT_ABORT(gpio.readRegister(DRV8908::RegAddr::IC_STAT, result));
        ESP_LOGI("GPIO", "%u, %u", result, gpio.status());
    }

    void gpio_test()
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(gpio.writeRegister(DRV8908::RegAddr::OLD_CTRL_2, 0b01000000));
        ESP_ERROR_CHECK_WITHOUT_ABORT(gpio.writeRegister(DRV8908::RegAddr::OP_CTRL_1, 0b00000001));
        ESP_ERROR_CHECK_WITHOUT_ABORT(gpio.writeRegister(DRV8908::RegAddr::OP_CTRL_2, 0b00000010));
        hal_delay_ms(5000);
        expander.set_output(1, true);
    }

private:
    TCA9538 expander;
    DRV8908 gpio;
};