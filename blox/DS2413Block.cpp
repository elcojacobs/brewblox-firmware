/*
 * Copyright 2018 BrewPi B.V.
 *
 * This file is part of BrewBlox
 *
 * BrewBlox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BrewBlox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DS2413Block.h"
#include "IoArrayHelpers.h"
#include "proto/cpp/DS2413.pb.h"

cbox::CboxError
DS2413Block::streamFrom(cbox::DataIn& in)
{
    blox_DS2413 newData = blox_DS2413_init_zero;
    cbox::CboxError res = streamProtoFrom(in, &newData, blox_DS2413_fields, blox_DS2413_size);
    /* if no errors occur, write new settings to wrapped object */
    if (res == cbox::CboxError::OK) {
        device.address(OneWireAddress(newData.address));
    }
    return res;
}

cbox::CboxError
DS2413Block::streamTo(cbox::DataOut& out) const
{
    blox_DS2413 message = blox_DS2413_init_zero;

    message.address = device.address();
    message.connected = device.connected();

    message.pins_count = 2;
    message.pins[0].which_Pin = blox_DS2413IoPin_A_tag;
    readIo(device, 1, message.pins[0].Pin.A);
    message.pins[1].which_Pin = blox_DS2413IoPin_B_tag;
    readIo(device, 2, message.pins[1].Pin.B);

    return streamProtoTo(out, &message, blox_DS2413_fields, blox_DS2413_size);
}

cbox::CboxError
DS2413Block::streamPersistedTo(cbox::DataOut& out) const
{
    blox_DS2413 message = blox_DS2413_init_zero;

    message.address = device.address();
    return streamProtoTo(out, &message, blox_DS2413_fields, blox_DS2413_size);
}

cbox::update_t
DS2413Block::update(const cbox::update_t& now)
{
    device.update();
    return update_1s(now);
}

void*
DS2413Block::implements(const cbox::obj_type_t& iface)
{
    if (iface == BrewBloxTypes_BlockType_DS2413) {
        return this; // me!
    }
    if (iface == cbox::interfaceId<IoArray>()) {
        // return the member that implements the interface in this case
        IoArray* ptr = &device;
        return ptr;
    }
    if (iface == cbox::interfaceId<OneWireDevice>()) {
        // return the member that implements the interface in this case
        DS2413* dsPtr = &device;
        OneWireDevice* devicePtr = dsPtr;
        return devicePtr;
    }
    return nullptr;
}
