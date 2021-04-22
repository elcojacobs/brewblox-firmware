#include "hal/hal_i2c.h"

hal_i2c_err_t hal_i2c_master_init()
{
    return 0;
}

hal_i2c_err_t hal_i2c_write(uint8_t /* address */, const uint8_t* /* data */, size_t /*len*/, bool /*stop*/)
{
    return 0;
}

hal_i2c_err_t hal_i2c_read(uint8_t /* address */, uint8_t* /* data */, size_t /*len*/, bool /*stop*/)
{
    return 0;
}