#pragma once

#include "ActuatorAnalogConstrained.h"
#include "ActuatorAnalogMock.h"
#include "blox/Block.h"

namespace cbox {
class ObjectContainer;
}

class ActuatorAnalogMockBlock : public Block<BrewBloxTypes_BlockType_ActuatorAnalogMock> {
private:
    cbox::ObjectContainer& objectsRef; // remember object container reference to create constraints
    ActuatorAnalogMock actuator;
    ActuatorAnalogConstrained constrained;

public:
    ActuatorAnalogMockBlock(cbox::ObjectContainer& objects)
        : objectsRef(objects)
        , actuator(0, 0, 100)
        , constrained(actuator)
    {
    }

    virtual cbox::CboxError streamFrom(cbox::DataIn& dataIn) override final;

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final;

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final;

    virtual cbox::update_t update(const cbox::update_t& now) override final
    {
        return update_never(now);
    }

    virtual void* implements(const cbox::obj_type_t& iface) override final;

    ActuatorAnalogMock& get()
    {
        return actuator;
    }
};
