#include "I2CTransaction.hpp"
#include "driver/i2c.h"

I2CTransaction::I2CTransaction(uint8_t address)
    : addr(address)
{
    cmd = i2c_cmd_link_create();
}

I2CTransaction::~I2CTransaction()
{
    i2c_cmd_link_delete(cmd);
}

esp_err_t I2CTransaction::process(bool stop)
{
    return i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
}

void I2CTransaction::start_write()
{
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1U), true);
}

void I2CTransaction::start_read()
{
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1U) | 0x01, true);
}

void I2CTransaction::write(const uint8_t& data, bool ack_enable)
{
    i2c_master_write_byte(cmd, data, ack_enable);
}

void I2CTransaction::write(const uint8_t* data, size_t data_len, bool ack_enable)
{
    i2c_master_write(cmd, const_cast<uint8_t*>(data), data_len, ack_enable);
}

void I2CTransaction::read(uint8_t& data, hal_i2c_ack_type_t ack_type)
{
    i2c_master_read_byte(cmd, &data, i2c_ack_type_t(ack_type));
}

void I2CTransaction::read(uint8_t* data, size_t data_len, hal_i2c_ack_type_t ack_type)
{
    i2c_master_read(cmd, data, data_len, i2c_ack_type_t(ack_type));
}

void I2CTransaction::stop()
{
    i2c_master_stop(cmd);
}