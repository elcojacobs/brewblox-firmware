#pragma once

#include "ActuatorAnalogConstrained.h"
#include "ActuatorAnalogConstraintsProto.h"
#include "ActuatorDigitalConstrained.h"
#include "ActuatorPwm.h"
#include "blox/ActuatorPinBlock.h"
#include "blox/Block.h"
#include "blox/FieldTags.h"
#include "cbox/CboxPtr.h"
#include "proto/cpp/ActuatorPwm.pb.h"
#include "proto/cpp/AnalogConstraints.pb.h"

class ActuatorPwmBlock : public Block<BrewbloxOptions_BlockType_ActuatorPwm> {
private:
    cbox::ObjectContainer& objectsRef; // remember object container reference to create constraints
    cbox::CboxPtr<ActuatorDigitalConstrained> actuator;
    ActuatorPwm pwm;
    ActuatorAnalogConstrained constrained;

public:
    ActuatorPwmBlock(cbox::ObjectContainer& objects)
        : objectsRef(objects)
        , actuator(objects)
        // convert ActuatorDigitalConstrained to base ActuatorDigitalChangeLogged
        , pwm([this]() { return std::shared_ptr<ActuatorDigitalChangeLogged>(this->actuator.lock()); })
        , constrained(pwm)
    {
    }
    virtual ~ActuatorPwmBlock() = default;

    virtual cbox::CboxError streamFrom(cbox::DataIn& dataIn) override final
    {
        blox_ActuatorPwm newData = blox_ActuatorPwm_init_zero;
        cbox::CboxError result = streamProtoFrom(dataIn, &newData, blox_ActuatorPwm_fields, blox_ActuatorPwm_size);
        if (result == cbox::CboxError::OK) {
            actuator.setId(newData.actuatorId);
            if (newData.period < 1000) {
                if (actuator.lock_as<ActuatorPinBlock>() == nullptr) {
                    // for actuators that are not digital pins, set a mimimum period of 1000
                    // OneWire is too slow for fast PWM.
                    newData.period = 1000;
                }
            }
            pwm.period(newData.period);
            setAnalogConstraints(newData.constrainedBy, constrained, objectsRef);
            constrained.setting(cnl::wrap<ActuatorAnalog::value_t>(newData.setting));
        }
        return result;
    }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
    {
        blox_ActuatorPwm message = blox_ActuatorPwm_init_zero;
        FieldTags stripped;
        message.actuatorId = actuator.getId();

        message.period = pwm.period();

        if (constrained.valueValid()) {
            message.value = cnl::unwrap(constrained.value());
        } else {
            stripped.add(blox_ActuatorPwm_value_tag);
        }
        if (constrained.settingValid()) {
            message.setting = cnl::unwrap(constrained.setting());
            message.drivenActuatorId = message.actuatorId;
        } else {
            stripped.add(blox_ActuatorPwm_setting_tag);
        };

        getAnalogConstraints(message.constrainedBy, constrained);
        stripped.copyToMessage(message.strippedFields, message.strippedFields_count, 2);

        return streamProtoTo(out, &message, blox_ActuatorPwm_fields, blox_ActuatorPwm_size);
    }

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final
    {
        blox_ActuatorPwm persisted = blox_ActuatorPwm_init_zero;
        persisted.actuatorId = actuator.getId();
        persisted.period = pwm.period();
        persisted.setting = cnl::unwrap(constrained.setting());
        getAnalogConstraints(persisted.constrainedBy, constrained);

        return streamProtoTo(out, &persisted, blox_ActuatorPwm_fields, blox_ActuatorPwm_size);
    }

    virtual cbox::update_t update(const cbox::update_t& now) override final
    {
        constrained.update();
        return pwm.update(now);
    }

    virtual void* implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewbloxOptions_BlockType_ActuatorPwm) {
            return this; // me!
        }
        if (iface == cbox::interfaceId<ActuatorAnalogConstrained>()) {
            // return the member that implements the interface in this case
            ActuatorAnalogConstrained* ptr = &constrained;
            return ptr;
        }
        if (iface == cbox::interfaceId<ProcessValue<ActuatorAnalog::value_t>>()) {
            // return the member that implements the interface in this case
            ProcessValue<ActuatorAnalog::value_t>* ptr = &constrained;
            return ptr;
        }
        return nullptr;
    }

    const cbox::CboxPtr<ActuatorDigitalConstrained>& targetLookup() const
    {
        return actuator;
    }

    ActuatorPwm& getPwm()
    {
        return pwm;
    }

    const ActuatorPwm& getPwm() const
    {
        return pwm;
    }

    ActuatorAnalogConstrained& getConstrained()
    {
        return constrained;
    }

    const ActuatorAnalogConstrained& getConstrained() const
    {
        return constrained;
    }
};
