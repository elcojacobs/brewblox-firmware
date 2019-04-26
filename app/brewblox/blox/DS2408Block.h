#pragma once

#include "DS2408.h"
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
        message.pins = device.readPios();
        message.latches = device.readLatches();
        message.claimed = device.claimed();
        message.connected = device.connected();

        return streamProtoTo(out, &message, blox_DS2408_fields, blox_DS2408_size);
    }

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final
    {
        blox_DS2408 message = blox_DS2408_init_zero;

        message.address = device.getDeviceAddress();
        message.latches = device.readLatches();
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
        if (iface == cbox::interfaceId<OneWireIO>()) {
            // return the member that implements the interface in this case
            OneWireIO* ptr = &device;
            return ptr;
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
