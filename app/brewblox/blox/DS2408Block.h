/*
 * Copyright 2019 BrewPi B.V.
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

#include "DS2408.h"
#include "IoArrayHelpers.h"
#include "blox/Block.h"
#include "proto/cpp/DS2408.pb.h"

OneWire&
theOneWire();

class DS2408Block : public Block<BrewbloxOptions_BlockType_DS2408> {
private:
    DS2408 device;

public:
    DS2408Block()
        : device(theOneWire())
    {
    }

    virtual cbox::CboxError streamFrom(cbox::DataIn& in) override final
    {
        blox_DS2408 newData = blox_DS2408_init_zero;
        cbox::CboxError res = streamProtoFrom(in, &newData, blox_DS2408_fields, blox_DS2408_size);
        /* if no errors occur, write new settings to wrapped object */
        if (res == cbox::CboxError::OK) {
            device.setDeviceAddress(OneWireAddress(newData.address));
        }
        return res;
    }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
    {
        blox_DS2408 message = blox_DS2408_init_zero;

        message.address = device.getDeviceAddress();
        message.connected = device.connected();

        message.pins_count = 8;
        message.pins[0].which_Pin = blox_DS2408_IoPin_A_tag;
        readIo(device, 1, message.pins[0].Pin.A);
        message.pins[1].which_Pin = blox_DS2408_IoPin_B_tag;
        readIo(device, 2, message.pins[1].Pin.B);
        message.pins[2].which_Pin = blox_DS2408_IoPin_C_tag;
        readIo(device, 3, message.pins[2].Pin.C);
        message.pins[3].which_Pin = blox_DS2408_IoPin_D_tag;
        readIo(device, 4, message.pins[3].Pin.D);
        message.pins[4].which_Pin = blox_DS2408_IoPin_E_tag;
        readIo(device, 5, message.pins[4].Pin.E);
        message.pins[5].which_Pin = blox_DS2408_IoPin_F_tag;
        readIo(device, 6, message.pins[5].Pin.F);
        message.pins[6].which_Pin = blox_DS2408_IoPin_G_tag;
        readIo(device, 7, message.pins[6].Pin.G);
        message.pins[7].which_Pin = blox_DS2408_IoPin_H_tag;
        readIo(device, 8, message.pins[7].Pin.H);

        return streamProtoTo(out, &message, blox_DS2408_fields, blox_DS2408_size);
    }

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final
    {
        blox_DS2408 message = blox_DS2408_init_zero;

        message.address = device.getDeviceAddress();
        return streamProtoTo(out, &message, blox_DS2408_fields, blox_DS2408_size);
    }

    virtual cbox::update_t update(const cbox::update_t& now) override final
    {
        device.update();
        return update_1s(now);
    }

    virtual void* implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewbloxOptions_BlockType_DS2408) {
            return this; // me!
        }
        if (iface == cbox::interfaceId<IoArray>()) {
            // return the member that implements the interface in this case
            IoArray* ptr = &device;
            return ptr;
        }
        if (iface == cbox::interfaceId<DS2408>()) {
            // return the member that implements the interface in this case
            return &device;
        }
        if (iface == cbox::interfaceId<OneWireDevice>()) {
            // return the member that implements the interface in this case
            DS2408* dsPtr = &device;
            OneWireDevice* devicePtr = dsPtr;
            return devicePtr;
        }
        return nullptr;
    }

    DS2408& get()
    {
        return device;
    }
};
