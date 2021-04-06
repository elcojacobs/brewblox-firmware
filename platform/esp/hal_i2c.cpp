#include "hal/hal_i2c.h"
#include "driver/i2c.h"
#include "esp_log.h"

hal_i2c_err_t hal_i2c_master_init()
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = 32;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
    conf.scl_io_num = 33;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
    conf.master.clk_speed = 100000;
    i2c_param_config(I2C_NUM_0, &conf);
    esp_err_t err = i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
    i2c_set_data_mode(I2C_NUM_0, I2C_DATA_MODE_MSB_FIRST, I2C_DATA_MODE_MSB_FIRST);
    i2c_filter_enable(I2C_NUM_0, 7);
    int setup_time, hold_time;
    i2c_get_start_timing(I2C_NUM_0, &setup_time, &hold_time);

    return err;
}

void hal_i2c_master_reset_all()
{
    uint8_t data = 0x06;
    hal_i2c_master_write(0x00, &data, 1, true);
}