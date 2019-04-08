#pragma once

#include "SetpointSensorPair.h"
#include "SetpointSensorPair.pb.h"
#include "blox/Block.h"
#include "blox/FieldTags.h"
#include "cbox/CboxPtr.h"

using std::placeholders::_1;

class SetpointSensorPairBlock : public Block<BrewbloxOptions_BlockType_SetpointSensorPair> {
private:
    cbox::CboxPtr<TempSensor> sensor;
    SetpointSensorPair pair;

public
    :
    SetpointSensorPairBlock(cbox::ObjectContainer& objects)
        : sensor(objects)
        , pair(sensor.lockFunctor())
    {
    }

    virtual ~SetpointSensorPairBlock() = default;

    virtual cbox::CboxError streamFrom(cbox::DataIn& in) override final
    {
        blox_SetpointSensorPair newData = blox_SetpointSensorPair_init_zero;
        cbox::CboxError res = streamProtoFrom(in, &newData, blox_SetpointSensorPair_fields, blox_SetpointSensorPair_size);
        /* if no errors occur, write new settings to wrapped object */
        if (res == cbox::CboxError::OK) {
            sensor.setId(newData.sensorId);
            pair.setting(cnl::wrap<temp_t>(newData.setting));
            pair.settingEnabled(newData.settingEnabled);
        }
        return res;
    }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
    {
        blox_SetpointSensorPair message = blox_SetpointSensorPair_init_zero;
        FieldTags stripped;
        message.sensorId = sensor.getId();
        message.settingEnabled = pair.settingEnabled();
        if (pair.valueValid()) {
            message.value = cnl::unwrap(pair.value());
        } else {
            stripped.add(blox_SetpointSensorPair_value_tag);
        }
        if (pair.settingValid()) {
            message.setting = cnl::unwrap(pair.setting());
        } else {
            stripped.add(blox_SetpointSensorPair_setting_tag);
        };

        stripped.copyToMessage(message.strippedFields, message.strippedFields_count, 2);

        return streamProtoTo(out, &message, blox_SetpointSensorPair_fields, blox_SetpointSensorPair_size);
    }

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final
    {
        blox_SetpointSensorPair message = blox_SetpointSensorPair_init_zero;
        message.sensorId = sensor.getId();
        message.setting = cnl::unwrap(pair.setting());
        message.settingEnabled = pair.settingEnabled();

        return streamProtoTo(out, &message, blox_SetpointSensorPair_fields, blox_SetpointSensorPair_size);
    }

    virtual cbox::update_t update(const cbox::update_t& now) override final
    {
        return update_never(now);
    }

    virtual void* implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewbloxOptions_BlockType_SetpointSensorPair) {
            return this; // me!
        }
        if (iface == cbox::interfaceId<ProcessValue<temp_t>>()) {
            // return the member that implements the interface in this case
            ProcessValue<temp_t>* ptr = &pair;
            return ptr;
        }
        if (iface == cbox::interfaceId<SetpointSensorPair>()) {
            SetpointSensorPair* ptr = &pair;
            return ptr;
        }
        return nullptr;
    }

    SetpointSensorPair& get()
    {
        return pair;
    }

    const SetpointSensorPair& get() const
    {
        return pair;
    }
};
