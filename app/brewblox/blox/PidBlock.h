#pragma once

#include "ActuatorAnalogConstrained.h"
#include "BrewBlox.h"
#include "IntervalHelper.h"
#include "Pid.h"
#include "ProcessValue.h"
#include "blox/Block.h"
#include "blox/FieldTags.h"
#include "cbox/CboxPtr.h"
#include "proto/cpp/Pid.pb.h"

class PidBlock : public Block<BrewBloxTypes_BlockType_Pid> {
private:
    cbox::CboxPtr<SetpointSensorPair> input;
    cbox::CboxPtr<ActuatorAnalogConstrained> output;

    Pid pid;
    IntervalHelper<1000> m_intervalHelper;
    bool previousActive = false;

public:
    PidBlock(cbox::ObjectContainer& objects)
        : input(objects)
        , output(objects)
        , pid(input.lockFunctor(), [this]() {
            // convert ActuatorConstrained to base ProcessValue
            return std::shared_ptr<ProcessValue<Pid::out_t>>(this->output.lock());
        })
    {
    }

    virtual ~PidBlock() = default;

    virtual cbox::CboxError streamFrom(cbox::DataIn& in) override final
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

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
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
        message.derivativeFilter = blox_Pid_FilterChoice(pid.derivativeFilterIdx() + 1);

        stripped.copyToMessage(message.strippedFields, message.strippedFields_count, 4);

        return streamProtoTo(out, &message, blox_Pid_fields, blox_Pid_size);
    }

    virtual cbox::CboxError
    streamPersistedTo(cbox::DataOut& out) const override final
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

    virtual cbox::update_t
    update(const cbox::update_t& now) override final
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

    virtual void*
    implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewBloxTypes_BlockType_Pid) {
            return this; // me!
        }
        return nullptr;
    }

    Pid&
    get()
    {
        return pid;
    }

    const Pid&
    get() const
    {
        return pid;
    }

    const auto&
    getInputLookup() const
    {
        return input;
    }

    const auto&
    getOutputLookup() const
    {
        return output;
    }
};
