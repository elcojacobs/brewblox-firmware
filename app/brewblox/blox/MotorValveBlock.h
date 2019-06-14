#pragma once

#include "ActuatorDigitalConstrained.h"
#include "ActuatorDigitalConstraintsProto.h"
#include "DS2408Block.h"
#include "FieldTags.h"
#include "MotorValve.h"
#include "blox/Block.h"
#include "cbox/CboxPtr.h"
#include "proto/cpp/MotorValve.pb.h"
#include <cstdint>

class MotorValveBlock : public Block<BrewbloxOptions_BlockType_MotorValve> {
private:
    cbox::ObjectContainer& objectsRef; // remember object container reference to create constraints
    cbox::CboxPtr<DS2408> hwDevice;
    MotorValve valve;
    ActuatorDigitalConstrained constrained;

public:
    MotorValveBlock(cbox::ObjectContainer& objects)
        : objectsRef(objects)
        , hwDevice(objects)
        , valve(hwDevice.lockFunctor(), 0)
        , constrained(valve)
    {
    }
    virtual ~MotorValveBlock() = default;

    virtual cbox::CboxError streamFrom(cbox::DataIn& dataIn) override final
    {
        blox_MotorValve message = blox_MotorValve_init_zero;
        cbox::CboxError result = streamProtoFrom(dataIn, &message, blox_MotorValve_fields, blox_MotorValve_size);

        if (result == cbox::CboxError::OK) {
            if (hwDevice.getId() != message.hwDevice) {
                valve.startChannel(0); // unregister at old hwDevice
                hwDevice.setId(message.hwDevice);
            }
            valve.startChannel(message.startChannel);
            setDigitalConstraints(message.constrainedBy, constrained, objectsRef);
            constrained.state(ActuatorDigitalBase::State(message.state));
        }

        return result;
    }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
    {
        blox_MotorValve message = blox_MotorValve_init_zero;
        FieldTags stripped;

        auto state = valve.state();
        if (state == ActuatorDigitalBase::State::Unknown) {
            stripped.add(blox_MotorValve_state_tag);
        } else {
            message.state = blox_DigitalState(valve.state());
        }

        auto valveState = valve.valveState();
        if (valveState == MotorValve::ValveState::Unknown) {
            stripped.add(blox_MotorValve_valveState_tag);
        } else {
            message.valveState = blox_MotorValve_ValveState(valve.valveState());
        }

        message.hwDevice = hwDevice.getId();
        message.startChannel = valve.startChannel();

        getDigitalConstraints(message.constrainedBy, constrained);

        stripped.copyToMessage(message.strippedFields, message.strippedFields_count, 1);
        return streamProtoTo(out, &message, blox_MotorValve_fields, blox_MotorValve_size);
    }

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final
    {
        return streamTo(out);
    }

    virtual cbox::update_t update(const cbox::update_t& now) override final
    {
        valve.update();
        constrained.update(now);
        return now + 1000;
    }

    virtual void* implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewbloxOptions_BlockType_MotorValve) {
            return this; // me!
        }
        if (iface == cbox::interfaceId<ActuatorDigitalConstrained>()) {
            // return the member that implements the interface in this case
            ActuatorDigitalConstrained* ptr = &constrained;
            return ptr;
        }
        return nullptr;
    }

    ActuatorDigitalConstrained& getConstrained()
    {
        return constrained;
    }
};
