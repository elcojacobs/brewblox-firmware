/*
 * Copyright 2020 BrewPi B.V.
 *
 * This file is part of BrewBlox.
 *
 * BrewBlox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BrewBlox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BrewBlox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MotorValveBlock.h"
#include "ActuatorDigitalConstraintsProto.h"
#include "FieldTags.h"

cbox::CboxError
MotorValveBlock::streamFrom(cbox::DataIn& dataIn)
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
        constrained.desiredState(ActuatorDigitalBase::State(message.desiredState));
    }

    return result;
}
void
MotorValveBlock::writePersistedStateToMessage(blox_MotorValve& message) const
{
    message.desiredState = blox_DigitalState(constrained.desiredState());
    message.hwDevice = hwDevice.getId();
    message.startChannel = valve.startChannel();
    getDigitalConstraints(message.constrainedBy, constrained);
}

cbox::CboxError
MotorValveBlock::streamTo(cbox::DataOut& out) const
{
    blox_MotorValve message = blox_MotorValve_init_zero;
    FieldTags stripped;

    writePersistedStateToMessage(message);

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

    stripped.copyToMessage(message.strippedFields, message.strippedFields_count, 1);
    return streamProtoTo(out, &message, blox_MotorValve_fields, blox_MotorValve_size);
}

cbox::CboxError
MotorValveBlock::streamPersistedTo(cbox::DataOut& out) const
{

    blox_MotorValve message = blox_MotorValve_init_zero;
    writePersistedStateToMessage(message);
    return streamProtoTo(out, &message, blox_MotorValve_fields, blox_MotorValve_size);
}

cbox::update_t
MotorValveBlock::update(const cbox::update_t& now)
{
    valve.update();
    return constrained.update(now);
}

void*
MotorValveBlock::implements(const cbox::obj_type_t& iface)
{
    if (iface == BrewBloxTypes_BlockType_MotorValve) {
        return this; // me!
    }
    if (iface == cbox::interfaceId<ActuatorDigitalConstrained>()) {
        // return the member that implements the interface in this case
        ActuatorDigitalConstrained* ptr = &constrained;
        return ptr;
    }
    return nullptr;
}
