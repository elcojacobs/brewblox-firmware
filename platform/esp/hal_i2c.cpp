#include "hal/hal_i2c.h"
#include "driver/i2c.h"

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
    return err;
}

hal_i2c_err_t hal_i2c_master_write(uint8_t addr, const uint8_t* data, size_t data_len, bool ack_enable)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1), ack_enable);
    i2c_master_write(cmd, const_cast<uint8_t*>(data), data_len, ack_enable);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

hal_i2c_err_t hal_i2c_master_read(uint8_t addr, uint8_t* data, size_t data_len, hal_i2c_ack_type_t read_ack_type, bool write_ack_enable)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | 0x1, write_ack_enable);
    i2c_master_read(cmd, data, data_len, i2c_ack_type_t(read_ack_type));
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

void hal_i2c_master_reset_all()
{
    uint8_t data = 0x06;
    hal_i2c_master_write(0x00, &data, 1, true);
}