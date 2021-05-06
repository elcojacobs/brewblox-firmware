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

#pragma once

#include "ActuatorDigitalConstrained.h"
#include "DS2408.h"
#include "MotorValve.h"
#include "blox/Block.h"
#include "cbox/CboxPtr.h"
#include "proto/cpp/MotorValve.pb.h"

class MotorValveBlock : public Block<BrewBloxTypes_BlockType_MotorValve> {
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
    void writePersistedStateToMessage(blox_MotorValve& message) const;
};
