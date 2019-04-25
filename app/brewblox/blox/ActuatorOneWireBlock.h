#pragma once

#include "ActuatorDigitalConstrained.h"
#include "ActuatorDigitalConstraintsProto.h"
#include "ActuatorOneWire.h"
#include "FieldTags.h"
#include "blox/Block.h"
#include "blox/DS2413Block.h"
#include "cbox/CboxPtr.h"
#include "proto/cpp/ActuatorOneWire.pb.h"
#include <cstdint>

class ActuatorOneWireBlock : public Block<BrewbloxOptions_BlockType_ActuatorOneWire> {
private:
    cbox::ObjectContainer& objectsRef; // remember object container reference to create constraints
    cbox::CboxPtr<DS2413> hwDevice;
    ActuatorOneWire actuator;
    ActuatorDigitalConstrained constrained;

public:
    ActuatorOneWireBlock(cbox::ObjectContainer& objects)
        : objectsRef(objects)
        , hwDevice(objects)
        , actuator(hwDevice.lockFunctor())
        , constrained(actuator)
    {
    }
    virtual ~ActuatorOneWireBlock() = default;

    virtual cbox::CboxError streamFrom(cbox::DataIn& dataIn) override final
    {
        blox_ActuatorOneWire message = blox_ActuatorOneWire_init_zero;
        cbox::CboxError result = streamProtoFrom(dataIn, &message, blox_ActuatorOneWire_fields, blox_ActuatorOneWire_size);

        if (result == cbox::CboxError::OK) {
            hwDevice.setId(message.hwDevice);
            actuator.channel(DS2413::Pio(message.channel));
            actuator.invert(message.invert);
            setDigitalConstraints(message.constrainedBy, constrained, objectsRef);
            constrained.state(ActuatorDigital::State(message.state));
        }

        return result;
    }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
    {
        blox_ActuatorOneWire message = blox_ActuatorOneWire_init_zero;
        FieldTags stripped;

        auto state = actuator.state();
        if (state == ActuatorDigital::State::Unknown) {
            stripped.add(blox_ActuatorOneWire_state_tag);
        } else {
            message.state = blox_AD_State(actuator.state());
        }

        message.hwDevice = hwDevice.getId();
        message.channel = blox_ActuatorOneWire_Channel(actuator.channel());
        message.invert = actuator.invert();
        getDigitalConstraints(message.constrainedBy, constrained);

        stripped.copyToMessage(message.strippedFields, message.strippedFields_count, 1);
        return streamProtoTo(out, &message, blox_ActuatorOneWire_fields, blox_ActuatorOneWire_size);
    }

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final
    {
        return streamTo(out);
    }

    virtual cbox::update_t update(const cbox::update_t& now) override final
    {
        actuator.update();
        constrained.update(now);
        return now + 1000;
    }

    virtual void* implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewbloxOptions_BlockType_ActuatorOneWire) {
            return this; // me!
        }
        if (iface == cbox::interfaceId<ActuatorDigitalConstrained>()) {
            // return the member that implements the interface in this case
            ActuatorDigitalConstrained* ptr = &constrained;
            return ptr;
        }
        return nullptr;
    }

    ActuatorOneWire& getPin()
    {
        return actuator;
    }

    ActuatorDigitalConstrained& getConstrained()
    {
        return constrained;
    }
};
