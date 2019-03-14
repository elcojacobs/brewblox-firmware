#pragma once

#include "ActuatorAnalogConstrained.h"
#include "ActuatorAnalogConstraintsProto.h"
#include "ActuatorOffset.h"
#include "blox/Block.h"
#include "blox/FieldTags.h"
#include "cbox/CboxPtr.h"
#include "proto/cpp/ActuatorOffset.pb.h"
#include "proto/cpp/AnalogConstraints.pb.h"

class ActuatorOffsetBlock : public Block<BrewbloxOptions_BlockType_ActuatorOffset> {
private:
    cbox::ObjectContainer& objectsRef; // remember object container reference to create constraints
    cbox::CboxPtr<SetpointSensorPair> target;
    cbox::CboxPtr<SetpointSensorPair> reference;
    ActuatorOffset actuator;
    ActuatorAnalogConstrained constrained;

public:
    ActuatorOffsetBlock(cbox::ObjectContainer& objects)
        : objectsRef(objects)
        , target(objects)
        , reference(objects)
        , actuator(target.lockFunctor(), reference.lockFunctor())
        , constrained(actuator)
    {
    }

    virtual cbox::CboxError streamFrom(cbox::DataIn& dataIn) override final
    {
        blox_ActuatorOffset newData = blox_ActuatorOffset_init_zero;
        cbox::CboxError result = streamProtoFrom(dataIn, &newData, blox_ActuatorOffset_fields, blox_ActuatorOffset_size);
        if (result == cbox::CboxError::OK) {
            target.setId(newData.targetId);
            reference.setId(newData.referenceId);
            actuator.selectedReference(ActuatorOffset::SettingOrValue(newData.referenceSettingOrValue));
            setAnalogConstraints(newData.constrainedBy, constrained, objectsRef);
            constrained.setting(cnl::wrap<ActuatorAnalog::value_t>(newData.setting));
            actuator.enabled(newData.enabled);
        }
        return result;
    }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
    {
        blox_ActuatorOffset message = blox_ActuatorOffset_init_zero;

        FieldTags stripped;

        message.targetId = target.getId();
        message.referenceId = reference.getId();
        message.referenceSettingOrValue = blox_ActuatorOffset_SettingOrValue(actuator.selectedReference());
        message.enabled = actuator.enabled();

        if (constrained.valueValid()) {
            message.value = cnl::unwrap(constrained.value());
        } else {
            stripped.add(blox_ActuatorOffset_value_tag);
        }
        if (constrained.settingValid()) {
            message.setting = cnl::unwrap(constrained.setting());
            message.drivenTargetId = message.targetId;
        } else {
            stripped.add(blox_ActuatorOffset_setting_tag);
        };

        getAnalogConstraints(message.constrainedBy, constrained);

        stripped.copyToMessage(message.strippedFields, message.strippedFields_count, 2);

        return streamProtoTo(out, &message, blox_ActuatorOffset_fields, blox_ActuatorOffset_size);
    }

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final
    {
        blox_ActuatorOffset persisted = blox_ActuatorOffset_init_zero;

        persisted.targetId = target.getId();
        persisted.referenceId = reference.getId();
        persisted.referenceSettingOrValue = blox_ActuatorOffset_SettingOrValue(actuator.selectedReference());
        persisted.enabled = actuator.enabled();

        persisted.setting = cnl::unwrap(constrained.setting());
        getAnalogConstraints(persisted.constrainedBy, constrained);

        return streamProtoTo(out, &persisted, blox_ActuatorOffset_fields, blox_ActuatorOffset_size);
    }

    virtual cbox::update_t update(const cbox::update_t& now) override final
    {
        actuator.update();
        constrained.update();
        return now + 1000;
    }

    virtual void* implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewbloxOptions_BlockType_ActuatorOffset) {
            return this; // me!
        }
        if (iface == cbox::interfaceId<ActuatorAnalogConstrained>()) {
            // return the member that implements the interface in this case
            ActuatorAnalogConstrained* ptr = &constrained;
            return ptr;
        }
        return nullptr;
    }

    ActuatorOffset& get()
    {
        return actuator;
    }
};
