#include "DigitalActuatorBlock.h"
#include "ActuatorDigitalConstraintsProto.h"
#include "FieldTags.h"

cbox::CboxError
DigitalActuatorBlock::streamFrom(cbox::DataIn& dataIn)
{
    blox_DigitalActuator message = blox_DigitalActuator_init_zero;
    cbox::CboxError result = streamProtoFrom(dataIn, &message, blox_DigitalActuator_fields, blox_DigitalActuator_size);

    if (result == cbox::CboxError::OK) {
        if (hwDevice.getId() != message.hwDevice) {
            actuator.channel(0); // unregister at old hwDevice
            hwDevice.setId(message.hwDevice);
        }
        actuator.channel(message.channel);
        actuator.invert(message.invert);
        setDigitalConstraints(message.constrainedBy, constrained, objectsRef);
        constrained.desiredState(ActuatorDigitalBase::State(message.desiredState));
    }

    return result;
}

void
DigitalActuatorBlock::writePersistedStateToMessage(blox_DigitalActuator& message) const
{
    message.hwDevice = hwDevice.getId();
    message.channel = actuator.channel();
    message.invert = actuator.invert();
    message.desiredState = blox_DigitalState(constrained.desiredState());

    getDigitalConstraints(message.constrainedBy, constrained);
}

cbox::CboxError
DigitalActuatorBlock::streamTo(cbox::DataOut& out) const
{
    blox_DigitalActuator message = blox_DigitalActuator_init_zero;
    FieldTags stripped;

    writePersistedStateToMessage(message);
    auto state = actuator.state();
    if (state == ActuatorDigitalBase::State::Unknown) {
        stripped.add(blox_DigitalActuator_state_tag);
    } else {
        message.state = blox_DigitalState(actuator.state());
    }

    stripped.copyToMessage(message.strippedFields, message.strippedFields_count, 1);
    return streamProtoTo(out, &message, blox_DigitalActuator_fields, blox_DigitalActuator_size);
}

cbox::CboxError
DigitalActuatorBlock::streamPersistedTo(cbox::DataOut& out) const
{
    blox_DigitalActuator message = blox_DigitalActuator_init_zero;
    writePersistedStateToMessage(message);
    return streamProtoTo(out, &message, blox_DigitalActuator_fields, blox_DigitalActuator_size);
}

cbox::update_t
DigitalActuatorBlock::update(const cbox::update_t& now)
{
    actuator.update();
    return constrained.update(now);
}

void*
DigitalActuatorBlock::implements(const cbox::obj_type_t& iface)
{
    if (iface == BrewBloxTypes_BlockType_DigitalActuator) {
        return this; // me!
    }
    if (iface == cbox::interfaceId<ActuatorDigitalConstrained>()) {
        // return the member that implements the interface in this case
        ActuatorDigitalConstrained* ptr = &constrained;
        return ptr;
    }
    return nullptr;
}