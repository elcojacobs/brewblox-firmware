/*
 * Copyright 2020 BrewPi B.V.
 *
 * This file is part of the BrewBlox Control Library.
 *
 * BrewBlox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BrewBlox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Brewblox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../inc/OneWireDevice.h"
#include "../inc/OneWire.h"
#include "../inc/OneWireAddress.h"

/**
 * Constructor
 * /param oneWire_ The oneWire bus the device is connected to
 * /param address_ The oneWire address of the device to use.
 */
OneWireDevice::OneWireDevice(OneWire& oneWire_, const OneWireAddress& address_)
    : oneWire(oneWire_)
    , address(address_)
{
}

/**
 * Get the device address
 * @return device address
 */
OneWireAddress
OneWireDevice::getDeviceAddress() const
{
    return address;
}

/**
 * Set the device address
 * @param new device address
 */
void
OneWireDevice::setDeviceAddress(const OneWireAddress& addr)
{
    address = addr;
}

/**
 * Checks if the address is valid by performing a crc8 check on it
 * @return bool, true if valid
 */
bool
OneWireDevice::validAddress() const
{
    const uint8_t* addr = address.asUint8ptr();

    return addr[0] && (OneWire::crc8(addr, 7) == addr[7]);
}
