/*
 * Copyright 2021 BrewPi B.V.
 *
 * This file is part of the BrewBlox Control Library.
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
 * along with BrewPi.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include "I2CTransaction.hpp"

class I2CDevice {
public:
    virtual uint8_t address() = 0;
    virtual uint8_t family_address() = 0;
};

template <uint8_t address_base>
class I2CDeviceBase : public I2CDevice {
public:
    I2CDeviceBase(uint8_t lower_address)
        : addr(family_address() + lower_address)
    {
    }

    virtual uint8_t family_address() override final
    {
        return address_base;
    };

    virtual uint8_t address() override final
    {
        return addr;
    };

protected:
    I2CTransaction i2cTransaction()
    {
        return I2CTransaction(addr);
    }

private:
    uint8_t addr;
};