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

#include "PidBlock.h"
#include "ProcessValue.h"
#include "blox/FieldTags.h"
#include "brewblox_particle.hpp" // TODO refactor to avoid needing global box instance to store output
#include "proto/cpp/Pid.pb.h"

PidBlock::PidBlock(cbox::ObjectContainer& objects)
    : input(objects)
    , output(objects)
    , pid(input.lockFunctor(), [this]() {
        // convert ActuatorConstrained to base ProcessValue
        return std::shared_ptr<ProcessValue<Pid::out_t>>(this->output.lock());
    })
{
}

cbox::CboxError
PidBlock::streamFrom(cbox::DataIn& in)
{
    blox_Pid newData = blox_Pid_init_zero;
    cbox::CboxError res = streamProtoFrom(in, &newData, blox_Pid_fields, blox_Pid_size);
    /* if no errors occur, write new settings to wrapped object */
    if (res == cbox::CboxError::OK) {
        pid.enabled(newData.enabled);
        input.setId(newData.inputId);
        output.setId(newData.outputId);
        pid.kp(cnl::wrap<Pid::in_t>(newData.kp));
        pid.ti(newData.ti);
        pid.td(newData.td);
        if (newData.integralReset != 0) {
            pid.setIntegral(cnl::wrap<Pid::out_t>(newData.integralReset));
        }
        pid.boilPointAdjust(cnl::wrap<Pid::in_t>(newData.boilPointAdjust));
        pid.boilMinOutput(cnl::wrap<Pid::out_t>(newData.boilMinOutput));
        pid.update(); // force an update that bypasses the update interval
    }
    return res;
}

cbox::CboxError
PidBlock::streamTo(cbox::DataOut& out) const
{
    FieldTags stripped;
    blox_Pid message = blox_Pid_init_zero;
    message.inputId = input.getId();
    message.outputId = output.getId();

    if (auto ptr = input.const_lock()) {
        if (ptr->valueValid()) {
            message.inputValue = cnl::unwrap(ptr->value());
        } else {
            stripped.add(blox_Pid_inputValue_tag);
        }
        if (ptr->settingValid()) {
            message.inputSetting = cnl::unwrap(ptr->setting());
        } else {
            stripped.add(blox_Pid_inputSetting_tag);
        }
    } else {
        stripped.add(blox_Pid_inputSetting_tag);
        stripped.add(blox_Pid_inputValue_tag);
    }

    if (auto ptr = output.const_lock()) {
        if (ptr->valueValid()) {
            message.outputValue = cnl::unwrap(ptr->value());
        } else {
            stripped.add(blox_Pid_outputValue_tag);
        }
        if (ptr->settingValid()) {
            message.outputSetting = cnl::unwrap(ptr->setting());
        } else {
            stripped.add(blox_Pid_outputSetting_tag);
        }

    } else {
        stripped.add(blox_Pid_outputSetting_tag);
        stripped.add(blox_Pid_outputValue_tag);
    }
    if (pid.active()) {
        message.drivenOutputId = message.outputId;
    }

    message.enabled = pid.enabled();
    message.active = pid.active();
    message.kp = cnl::unwrap(pid.kp());
    message.ti = pid.ti();
    message.td = pid.td();
    message.p = cnl::unwrap(pid.p());
    message.i = cnl::unwrap(pid.i());
    message.d = cnl::unwrap(pid.d());
    message.error = cnl::unwrap(pid.error());
    message.integral = cnl::unwrap(pid.integral());
    message.derivative = cnl::unwrap(pid.derivative());
    message.boilPointAdjust = cnl::unwrap(pid.boilPointAdjust());
    message.boilMinOutput = cnl::unwrap(pid.boilMinOutput());
    message.boilModeActive = pid.boilModeActive();
    message.derivativeFilter = blox_FilterChoice(pid.derivativeFilterNr());

    stripped.copyToMessage(message.strippedFields, message.strippedFields_count, 4);

    return streamProtoTo(out, &message, blox_Pid_fields, blox_Pid_size);
}

cbox::CboxError
PidBlock::streamPersistedTo(cbox::DataOut& out) const
{
    blox_Pid message = blox_Pid_init_zero;
    message.inputId = input.getId();
    message.outputId = output.getId();
    message.enabled = pid.enabled();
    message.kp = cnl::unwrap(pid.kp());
    message.ti = pid.ti();
    message.td = pid.td();
    message.boilPointAdjust = cnl::unwrap(pid.boilPointAdjust());
    message.boilMinOutput = cnl::unwrap(pid.boilMinOutput());

    return streamProtoTo(out, &message, blox_Pid_fields, blox_Pid_size);
}

cbox::update_t
PidBlock::update(const cbox::update_t& now)
{
    bool doUpdate = false;
    auto nextUpdate = m_intervalHelper.update(now, doUpdate);

    if (doUpdate) {
        pid.update();
        auto pidActive = pid.active();
        if (previousActive != pidActive) {
            // When the pid changes whether it is active
            // ensure that the output object setting in EEPROM is zero
            // to prevent loading older EEPROM data for it on reboot
            // in which the output is still active
            if (auto ptr = output.lock()) {
                ptr->setting(0);
                brewbloxBox().storeUpdatedObject(output.getId());
                previousActive = pidActive;
            }
            return now;
        }
    }
    return nextUpdate;
}

void* PidBlock::implements(const cbox::obj_type_t& iface)
{
    if (iface == BrewBloxTypes_BlockType_Pid) {
        return this; // me!
    }
    return nullptr;
}
