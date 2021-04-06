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

using hal_i2c_err_t = int32_t; // 0 is no error

enum hal_i2c_ack_type_t {
    HAL_I2C_MASTER_ACK = 0x0,       /*!< I2C ack for each byte read */
    HAL_I2C_MASTER_NACK = 0x1,      /*!< I2C nack for each byte read */
    HAL_I2C_MASTER_LAST_NACK = 0x2, /*!< I2C nack for the last byte*/
    HAL_I2C_MASTER_ACK_MAX,
};

hal_i2c_err_t hal_i2c_master_init();