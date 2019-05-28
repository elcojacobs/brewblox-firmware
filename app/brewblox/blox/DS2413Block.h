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

#pragma once

#include "DS2413.h"
#include "IoArrayHelpers.h"
#include "blox/Block.h"
#include "proto/cpp/DS2413.pb.h"

OneWire&
theOneWire();

class DS2413Block : public Block<BrewbloxOptions_BlockType_DS2413> {
private:
    DS2413 device;

public:
    DS2413Block()
        : device(theOneWire())
    {
    }

    virtual cbox::CboxError streamFrom(cbox::DataIn& in) override final
    {
        blox_DS2413 newData = blox_DS2413_init_zero;
        cbox::CboxError res = streamProtoFrom(in, &newData, blox_DS2413_fields, blox_DS2413_size);
        /* if no errors occur, write new settings to wrapped object */
        if (res == cbox::CboxError::OK) {
            device.setDeviceAddress(OneWireAddress(newData.address));
            writeIoConfig(device, 1, newData.channels[0].config);
            writeIoConfig(device, 2, newData.channels[1].config);
        }
        return res;
    }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
    {
        blox_DS2413 message = blox_DS2413_init_zero;

        message.address = device.getDeviceAddress();
        message.connected = device.connected();

        readIoConfig(device, 1, message.channels[0].config);
        readIoConfig(device, 2, message.channels[1].config);

        return streamProtoTo(out, &message, blox_DS2413_fields, blox_DS2413_size);
    }

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final
    {
        blox_DS2413 message = blox_DS2413_init_zero;

        message.address = device.getDeviceAddress();
        return streamProtoTo(out, &message, blox_DS2413_fields, blox_DS2413_size);
    }

    virtual cbox::update_t update(const cbox::update_t& now) override final
    {
        device.update();
        return update_1s(now);
    }

    virtual void* implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewbloxOptions_BlockType_DS2413) {
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

    DS2413& get()
    {
        return device;
    }
};
