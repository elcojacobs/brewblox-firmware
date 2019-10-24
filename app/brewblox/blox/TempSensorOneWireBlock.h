#pragma once

#include "TempSensorOneWire.h"
#include "Temperature.h"
#include "blox/Block.h"
#include "blox/FieldTags.h"
#include "proto/cpp/TempSensorOneWire.pb.h"

OneWire&
theOneWire();

class TempSensorOneWireBlock : public Block<BrewBloxTypes_BlockType_TempSensorOneWire> {
private:
    TempSensorOneWire sensor;

public:
    TempSensorOneWireBlock()
        : sensor(theOneWire())
    {
    }

    virtual cbox::CboxError streamFrom(cbox::DataIn& in) override final
    {
        blox_TempSensorOneWire newData = blox_TempSensorOneWire_init_zero;
        cbox::CboxError res = streamProtoFrom(in, &newData, blox_TempSensorOneWire_fields, blox_TempSensorOneWire_size);
        /* if no errors occur, write new settings to wrapped object */
        if (res == cbox::CboxError::OK) {
            sensor.setDeviceAddress(OneWireAddress(newData.address));
            sensor.setCalibration(cnl::wrap<temp_t>(newData.offset));
        }
        return res;
    }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
    {
        blox_TempSensorOneWire message = blox_TempSensorOneWire_init_zero;
        FieldTags stripped;

        if (sensor.valid()) {
            message.value = cnl::unwrap((sensor.value()));
        } else {
            stripped.add(blox_TempSensorOneWire_value_tag);
        }

        message.address = sensor.getDeviceAddress();
        message.offset = cnl::unwrap(sensor.getCalibration());

        stripped.copyToMessage(message.strippedFields, message.strippedFields_count, 1);
        return streamProtoTo(out, &message, blox_TempSensorOneWire_fields, blox_TempSensorOneWire_size);
    }

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final
    {
        blox_TempSensorOneWire message = blox_TempSensorOneWire_init_zero;
        message.address = sensor.getDeviceAddress();
        message.offset = cnl::unwrap(sensor.getCalibration());
        return streamProtoTo(out, &message, blox_TempSensorOneWire_fields, blox_TempSensorOneWire_size);
    }

    virtual cbox::update_t update(const cbox::update_t& now) override final
    {
        sensor.update();
        return update_1s(now);
    }

    virtual void* implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewBloxTypes_BlockType_TempSensorOneWire) {
            return this; // me!
        }
        if (iface == cbox::interfaceId<TempSensor>()) {
            // return the member that implements the interface in this case
            TempSensorOneWire* owptr = &sensor;
            TempSensor* ptr = owptr;
            return ptr;
        }
        if (iface == cbox::interfaceId<OneWireDevice>()) {
            // return the member that implements the interface in this case
            TempSensorOneWire* sensorPtr = &sensor;
            OneWireDevice* devicePtr = sensorPtr;
            return devicePtr;
        }
        return nullptr;
    }

    TempSensorOneWire& get()
    {
        return sensor;
    }
};
