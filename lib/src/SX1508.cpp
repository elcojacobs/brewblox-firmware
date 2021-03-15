/*
 * Copyright 2020 BrewPi B.V.
 *
 * This file is part of the Brewblox Control Library.
 *
 * BrewPi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BrewPi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Brewblox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SX1508.hpp"

hal_i2c_err_t SX1508::write_reg(RegAddr addr, uint8_t data)
{
    uint8_t bytes[2] = {static_cast<uint8_t>(addr), data};
    return i2c_write(bytes, 2, true);
}

/*
hal_i2c_err_t SX1508::write_regs(char RegAdr, const char* data, int len)
{
    int i;

    i2c.start();
    i2c.write(_slaveAddress);
    i2c.write(RegAdr);

    for (i = 0; i < len; i++) {
        i2c.write(data[i]);
        wait_ms(1);
    }

    i2c.stop();
}*/

hal_i2c_err_t SX1508::read_reg(RegAddr addr, uint8_t& result)
{
    uint8_t data = static_cast<uint8_t>(addr);
    auto err = i2c_write(&data, 1, true);
    if (err) {
        return err;
    };
    return i2c_read(&result, 1, hal_i2c_ack_type_t::I2C_MASTER_LAST_NACK, true);
}

void SX1508::reset()
{
    write_reg(RegAddr::reset, 0x12);
    write_reg(RegAddr::reset, 0x34);
}