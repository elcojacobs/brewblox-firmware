#pragma once

#include "ActuatorDigital.h"
#include "ActuatorDigitalConstrained.h"
#include "blox/Block.h"
#include "cbox/CboxPtr.h"
#include "proto/cpp/DigitalActuator.pb.h"

class IoArray;

class DigitalActuatorBlock : public Block<BrewBloxTypes_BlockType_DigitalActuator> {
private:
    cbox::ObjectContainer& objectsRef; // remember object container reference to create constraints
    cbox::CboxPtr<IoArray> hwDevice;
    ActuatorDigital actuator;
    ActuatorDigitalConstrained constrained;

public:
    DigitalActuatorBlock(cbox::ObjectContainer& objects);
    virtual ~DigitalActuatorBlock() = default;

    virtual cbox::CboxError streamFrom(cbox::DataIn& dataIn) override final;

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final;

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final;

    virtual cbox::update_t update(const cbox::update_t& now) override final;

    virtual void* implements(const cbox::obj_type_t& iface) override final;

    ActuatorDigitalConstrained& getConstrained()
    {
        return constrained;
    }

private:
    void writePersistedStateToMessage(blox_DigitalActuator& message) const;
};
