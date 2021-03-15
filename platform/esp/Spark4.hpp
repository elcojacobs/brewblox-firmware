#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <driver/gpio.h>
#include <nvs_flash.h>
#pragma GCC diagnostic pop

#include "hal/hal_i2c.h"

class Spark4 {
public:
    Spark4() = default;
    ~Spark4() = default;

    static void init()
    {
        nvs_flash_init();
        gpio_install_isr_service(0);
        //esp_event_loop_create_default();
        hal_i2c_master_init();
    }

    static void deinit()
    {
        //esp_event_loop_delete_default();
        gpio_uninstall_isr_service();
        nvs_flash_deinit();
    }
};
