/*
 * Copyright 2020 BrewPi B.V./Elco Jacobs.
 *
 * This file is part of Brewblox.
 * 
 * Brewblox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Brewblox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Brewblox.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstddef>
#include <cstdint>

using hal_i2c_err_t = int32_t;

enum class hal_i2c_ack_type_t {
    I2C_MASTER_ACK = 0x0,       /*!< I2C ack for each byte read */
    I2C_MASTER_NACK = 0x1,      /*!< I2C nack for each byte read */
    I2C_MASTER_LAST_NACK = 0x2, /*!< I2C nack for the last byte*/
    I2C_MASTER_ACK_MAX,
};

// true on success
hal_i2c_err_t hal_i2c_master_init();
hal_i2c_err_t hal_i2c_master_write(uint8_t addr, const uint8_t* data, size_t data_len, bool ack_enable = true);
hal_i2c_err_t hal_i2c_master_read(uint8_t addr, uint8_t* data, size_t data_len, hal_i2c_ack_type_t read_ack_type, bool write_ack_enable = true);
void hal_i2c_master_reset_all();