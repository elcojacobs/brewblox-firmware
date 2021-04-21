/*
 * Copyright 2021 BrewPi B.V.
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

bool SX1508::write_reg(RegAddr addr, uint8_t data)
{
    return i2c_write({static_cast<uint8_t>(addr), data});
}

bool SX1508::read_reg(RegAddr addr, uint8_t& result)
{
    if (i2c_write(static_cast<uint8_t>(addr), true)) {
        auto answer = i2c_read(1);
        if (answer.size()) {
            result = answer[0];
            return true;
        }
    }
    return false;
}

void SX1508::reset()
{
    write_reg(RegAddr::reset, 0x12);
    write_reg(RegAddr::reset, 0x34);
}