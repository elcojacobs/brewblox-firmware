#include "driver/i2c.h"
#include "hal/hal_i2c.h"

hal_i2c_err_t to_hal_err(esp_err_t err)
{
    return err; // TODO: convert errors to platform independent enum
}

hal_i2c_err_t hal_i2c_master_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 32,
        .scl_io_num = 33,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master = {.clk_speed = 100000U},
        .clk_flags = 0,
    };
    i2c_param_config(I2C_NUM_0, &conf);
    esp_err_t err = i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
    i2c_set_data_mode(I2C_NUM_0, I2C_DATA_MODE_MSB_FIRST, I2C_DATA_MODE_MSB_FIRST);
    i2c_filter_enable(I2C_NUM_0, 7);

    return to_hal_err(err);
}

hal_i2c_err_t hal_i2c_write(uint8_t address, const uint8_t* data, size_t len, bool stop)
{
    auto cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1U), true);
    i2c_master_write(cmd, const_cast<uint8_t*>(data), len, true);
    if (stop) {
        i2c_master_stop(cmd);
    }
    auto err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return to_hal_err(err);
}

hal_i2c_err_t hal_i2c_read(uint8_t address, uint8_t* data, size_t len, bool stop)
{
    auto cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1U) | 0x01, true);
    i2c_master_read(cmd, const_cast<uint8_t*>(data), len, I2C_MASTER_LAST_NACK);
    if (stop) {
        i2c_master_stop(cmd);
    }
    auto err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return to_hal_err(err);
}