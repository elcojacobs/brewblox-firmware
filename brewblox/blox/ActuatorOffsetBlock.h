#pragma once

#include "ActuatorAnalogConstrained.h"
#include "ActuatorOffset.h"
#include "blox/Block.h"
#include "cbox/CboxPtr.h"

class ActuatorOffsetBlock : public Block<BrewBloxTypes_BlockType_ActuatorOffset> {
private:
    cbox::ObjectContainer& objectsRef; // remember object container reference to create constraints
    cbox::CboxPtr<SetpointSensorPair> target;
    cbox::CboxPtr<SetpointSensorPair> reference;
    ActuatorOffset offset;
    ActuatorAnalogConstrained constrained;

public:
    ActuatorOffsetBlock(cbox::ObjectContainer& objects)
        : objectsRef(objects)
        , target(objects)
        , reference(objects)
        , offset(target.lockFunctor(), reference.lockFunctor())
        , constrained(offset)
    {
    }

    virtual cbox::CboxError streamFrom(cbox::DataIn& dataIn) override final;

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final;

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final;

    virtual cbox::update_t update(const cbox::update_t& now) override final;

    virtual void* implements(const cbox::obj_type_t& iface) override final;

    ActuatorOffset& get()
    {
        return offset;
    }
};
