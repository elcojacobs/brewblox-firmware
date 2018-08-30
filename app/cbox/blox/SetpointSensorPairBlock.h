#pragma once

#include "Block.h"
#include "CboxPtr.h"
#include "ResolveType.h"
#include "SetpointSensorPair.h"
#include "SetpointSensorPair.pb.h"

class SetpointSensorPairBlock : public Block<SetpointSensorPairBlock> {
private:
    cbox::CboxPtr<TempSensor> sensor;
    cbox::CboxPtr<SetPoint> setpoint;
    SetpointSensorPair pair;

public:
    SetpointSensorPairBlock(cbox::ObjectContainer& objects)
        : sensor(objects)
        , setpoint(objects)
        , pair(sensor, setpoint)
    {
    }

    virtual cbox::CboxError streamFrom(cbox::DataIn& in) override final
    {
        blox_SetpointSensorPair newData;
        cbox::CboxError res = streamProtoFrom(in, &newData, blox_SetpointSensorPair_fields, blox_SetpointSensorPair_size);
        /* if no errors occur, write new settings to wrapped object */
        if (res == cbox::CboxError::OK) {
            sensor.setId(newData.sensorId);
            setpoint.setId(newData.setpointId);
        }
        return res;
    }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
    {
        blox_SetpointSensorPair message;
        message.sensorId = sensor.getId();
        message.setpointId = setpoint.getId();
        message.sensor = pair.value().getRaw();
        message.setpoint = pair.setting().getRaw();
        message.sensorValid = sensor.valid();
        message.setpointValid = setpoint.valid();

        return streamProtoTo(out, &message, blox_SetpointSensorPair_fields, blox_SetpointSensorPair_size);
    }

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final
    {
        blox_SetpointSensorPair message;
        message.sensor = sensor.getId();
        message.setpoint = setpoint.getId();

        return streamProtoTo(out, &message, blox_SetpointSensorPair_fields, blox_SetpointSensorPair_size);
    }

    virtual cbox::update_t update(const cbox::update_t& now) override final
    {
        return update_never(now);
    }

    virtual void* implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == cbox::resolveTypeId(this)) {
            return this; // me!
        }
        if (iface == cbox::resolveTypeId<ProcessValue>()) {
            // return the member that implements the interface in this case
            ProcessValue* ptr = &pair;
            return ptr;
        }
        return nullptr;
    }

    SetpointSensorPair& get()
    {
        return pair;
    }
};
