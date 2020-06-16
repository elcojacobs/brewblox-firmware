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

#include "OneWireMockDevice.h"
#include "OneWireMockDriver.h"

uint8_t
OneWireMockDevice::read()
{
    // process all incoming bytes and generate replies first
    while (!masterToSlave.empty()) {
        process();
    }
    uint8_t b = 0xFF;
    if (!slaveToMaster.empty()) {
        b = slaveToMaster.front();
        slaveToMaster.pop_front();
    }
    return b;
}

void
OneWireMockDevice::write(uint8_t b)
{
    if (!dropped) {
        masterToSlave.push_back(b);
    }
}

void
OneWireMockDevice::recv(uint8_t* buf, uint16_t count)
{
    for (uint16_t i = 0; i < count; i++) {
        buf[i] = recv();
    }
}

uint8_t
OneWireMockDevice::recv()
{
    uint8_t retv = 0xFF;
    if (!masterToSlave.empty()) {
        retv = masterToSlave.front();
        masterToSlave.pop_front();
    }
    return retv;
}

void
OneWireMockDevice::send(uint8_t b)
{
    slaveToMaster.push_back(b);
}

void
OneWireMockDevice::send(uint8_t* buf, uint16_t count)
{
    for (uint16_t i = 0; i < count; i++) {
        send(buf[i]);
    }
}

bool
OneWireMockDevice::reset()
{
    while (!masterToSlave.empty()) {
        process(); // process any outstanding writes that don't expect a reply
    }
    dropped = false;
    masterToSlave.clear();
    slaveToMaster.clear();
    return connected;
}

void
OneWireMockDevice::process()
{
    if (dropped) {
        masterToSlave.clear();
        return;
    }
    uint8_t cmd = recv();
    switch (cmd) {
    case 0xFF: // nothing received
        return;
    case 0x33: // Read ROM
        send(address.asUint8ptr(), 8);
        break;
    case 0x55: // Match ROM
    case 0x69: // Overdrive Match
    {
        OneWireAddress selectedAddress;
        recv(selectedAddress.asUint8ptr(), 8);
        dropped = !match(selectedAddress);
    } break;
    case 0xF0: // Search ROM
        search_bitnr = 0;
        break;
    case 0xCC: // Skip ROM
    case 0x3C: // Overdrive skip
    case 0xA5: // Resume
        break;
    default:
        // not a generic OneWire command, forward to device implementation
        processImpl(cmd);
    }
}

bool
OneWireMockDevice::getAddressBit()
{
    uint64_t mask = uint64_t{0x01} << search_bitnr;
    return (mask & uint64_t(address)) > 0;
}

void
OneWireMockDevice::search_triplet_read(bool* id_bit, bool* cmp_id_bit)
{
    while (!masterToSlave.empty()) {
        // process any outstanding writes first.
        // The reset search command is still pending without this
        process();
    }
    bool my_bit = getAddressBit();
    if (!dropped && my_bit) {
        *id_bit = false; // pulls down bit if match
    }
    if (!dropped && !my_bit) {
        *cmp_id_bit = false; // pulls down bit if match with complement
    }
}

void
OneWireMockDevice::search_triplet_write(bool bit)
{
    bool my_bit = getAddressBit();
    if (my_bit != bit) {
        dropped = true;
    }
    if (search_bitnr < 63) {
        search_bitnr++;
    }
}
