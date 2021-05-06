#pragma once

#include "ActuatorAnalogConstrained.h"
#include "ActuatorDigitalConstrained.h"
#include "ActuatorPwm.h"
#include "DigitalActuatorBlock.h"
#include "blox/Block.h"
#include "cbox/CboxPtr.h"

class ActuatorPwmBlock : public Block<BrewBloxTypes_BlockType_ActuatorPwm> {
private:
    cbox::ObjectContainer& objectsRef; // remember object container reference to create constraints
    cbox::CboxPtr<ActuatorDigitalConstrained> actuator;
    ActuatorPwm pwm;
    ActuatorAnalogConstrained constrained;

    bool previousSettingValid = false;

public:
    ActuatorPwmBlock(cbox::ObjectContainer& objects)
        : objectsRef(objects)
        , actuator(objects)
        , pwm(actuator.lockFunctor())
        , constrained(pwm)
    {
    }
    virtual ~ActuatorPwmBlock() = default;

    virtual cbox::CboxError streamFrom(cbox::DataIn& dataIn) override final;

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final;

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final;

    virtual cbox::update_t update(const cbox::update_t& now) override final;

    virtual void* implements(const cbox::obj_type_t& iface) override final;

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
