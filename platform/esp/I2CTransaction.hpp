#pragma once
#include "hal/hal_i2c.h"

class I2CTransaction {
    using i2c_cmd_handle_t = void*;

public:
    I2CTransaction(uint8_t address);
    ~I2CTransaction();

    // not copyable, transaction should be unique
    I2CTransaction(const I2CTransaction&) = delete;

    hal_i2c_err_t process(bool stop = true);
    void start_write();
    void start_read();
    void write(const uint8_t& data, bool ack_enable = true);
    void write(const uint8_t* data, size_t data_len, bool ack_enable = true);
    void read(uint8_t& data, hal_i2c_ack_type_t ack_type = HAL_I2C_MASTER_LAST_NACK);
    void read(uint8_t* data, size_t data_len, hal_i2c_ack_type_t ack_type = HAL_I2C_MASTER_LAST_NACK);
    void stop();

private:
    i2c_cmd_handle_t cmd;
    uint8_t addr;
};
