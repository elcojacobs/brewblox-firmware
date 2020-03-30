#pragma once

#include "ActuatorDigitalConstrained.h"
#include "BrewBlox.h"
#include "ProcessValue.h"
#include "blox/Block.h"
#include "cbox/CboxPtr.h"
#include "proto/cpp/ActuatorLogic.pb.h"

class DigitalCompare {
public:
    DigitalCompare(const blox_ActuatorLogic_DigitalCompare& data, cbox::ObjectContainer& objects)
        : lookup(objects, cbox::obj_id_t(data.id))
        , op(data.op)
        , result(blox_ActuatorLogic_Result_FALSE)
        , rhs(ActuatorDigitalBase::State(data.rhs))
    {
    }

    ~DigitalCompare() = default;

    blox_ActuatorLogic_Result eval() const
    {
        if (auto actPtr = lookup.const_lock()) {
            switch (op) {
            case blox_ActuatorLogic_DigitalCompareOp_VALUE_IS:
                return blox_ActuatorLogic_Result(actPtr->state() == rhs);
            case blox_ActuatorLogic_DigitalCompareOp_VALUE_ISNOT:
                return blox_ActuatorLogic_Result(actPtr->desiredState() != rhs);
            case blox_ActuatorLogic_DigitalCompareOp_DESIRED_IS:
                return blox_ActuatorLogic_Result(actPtr->desiredState() == rhs);
            case blox_ActuatorLogic_DigitalCompareOp_DESIRED_ISNOT:
                return blox_ActuatorLogic_Result(actPtr->desiredState() != rhs);
            }
            return blox_ActuatorLogic_Result_INVALID_DIGITAL_OP;
        }
        return blox_ActuatorLogic_Result_BLOCK_NOT_FOUND;
    }

    void write(blox_ActuatorLogic_DigitalCompare& dest, bool includeNotPersisted) const
    {
        dest.id = lookup.getId();
        dest.op = op;
        dest.rhs = blox_DigitalState(rhs);
        if (includeNotPersisted) {
            dest.result = result;
        }
    }

    void update()
    {
        result = eval();
    }

private:
    cbox::CboxPtr<ActuatorDigitalConstrained> lookup;
    blox_ActuatorLogic_DigitalCompareOp op;
    blox_ActuatorLogic_Result result;
    ActuatorDigitalBase::State rhs;
};

class AnalogCompare {
public:
    AnalogCompare(const blox_ActuatorLogic_AnalogCompare& data, cbox::ObjectContainer& objects)
        : lookup(objects, cbox::obj_id_t(data.id))
        , op(data.op)
        , result(blox_ActuatorLogic_Result_FALSE)
        , rhs(cnl::wrap<fp12_t>(data.rhs))
    {
    }

    ~AnalogCompare() = default;

    blox_ActuatorLogic_Result eval() const
    {
        if (auto pvPtr = lookup.const_lock()) {
            switch (op) {
            case blox_ActuatorLogic_AnalogCompareOp_VALUE_LE:
                if (!pvPtr->valueValid()) {
                    return blox_ActuatorLogic_Result_FALSE;
                }
                return blox_ActuatorLogic_Result(pvPtr->value() <= rhs);
            case blox_ActuatorLogic_AnalogCompareOp_VALUE_GE:
                if (!pvPtr->valueValid()) {
                    return blox_ActuatorLogic_Result_FALSE;
                }
                return blox_ActuatorLogic_Result(pvPtr->value() >= rhs);
            case blox_ActuatorLogic_AnalogCompareOp_SETTING_LE:
                if (!pvPtr->settingValid()) {
                    return blox_ActuatorLogic_Result_FALSE;
                }
                return blox_ActuatorLogic_Result(pvPtr->setting() <= rhs);
            case blox_ActuatorLogic_AnalogCompareOp_SETTING_GE:
                if (!pvPtr->settingValid()) {
                    return blox_ActuatorLogic_Result_FALSE;
                }
                return blox_ActuatorLogic_Result(pvPtr->setting() >= rhs);
            }
            return blox_ActuatorLogic_Result_INVALID_ANALOG_OP;
        }
        return blox_ActuatorLogic_Result_BLOCK_NOT_FOUND;
    }

    void write(blox_ActuatorLogic_AnalogCompare& dest, bool includeNotPersisted) const
    {
        dest.id = lookup.getId();
        dest.op = op;
        dest.rhs = cnl::unwrap(rhs);
        if (includeNotPersisted) {
            dest.result = result;
        }
    }

    void update()
    {
        result = eval();
    }

private:
    cbox::CboxPtr<ProcessValue<fp12_t>> lookup;
    blox_ActuatorLogic_AnalogCompareOp op;
    blox_ActuatorLogic_Result result;
    fp12_t rhs;
};

class ActuatorLogicBlock : public Block<BrewBloxTypes_BlockType_ActuatorLogic> {
private:
    cbox::ObjectContainer& objectsRef; // remember object container reference to create constraints
    cbox::CboxPtr<ActuatorDigitalConstrained> target;
    bool enabled = false;
    std::vector<DigitalCompare> digitals;
    std::vector<AnalogCompare> analogs;
    std::vector<blox_ActuatorLogic_SyntaxSymbol> symbols;

public:
    ActuatorLogicBlock(cbox::ObjectContainer& objects)
        : objectsRef(objects)
        , target(objects)
    {
    }
    virtual ~ActuatorLogicBlock() = default;

    virtual cbox::CboxError streamFrom(cbox::DataIn& dataIn) override final
    {
        blox_ActuatorLogic newData = blox_ActuatorLogic_init_zero;
        cbox::CboxError result = streamProtoFrom(dataIn, &newData, blox_ActuatorLogic_fields, blox_ActuatorLogic_size);
        if (result == cbox::CboxError::OK) {
            target.setId(newData.targetId);
            enabled = newData.enabled;
            digitals.clear();
            analogs.clear();

            for (pb_size_t i = 0; i < newData.digital_count; i++) {
                digitals.emplace_back(newData.digital[i], objectsRef);
            }
            for (pb_size_t i = 0; i < newData.analog_count; i++) {
                analogs.emplace_back(newData.analog[i], objectsRef);
            }

            for (pb_size_t i = 0; i < newData.symbols_count; i++) {
                symbols.emplace_back(symbols[i]);
            }
        }
        return result;
    }

    void
    writeMessage(blox_ActuatorLogic& message, bool includeNotPersisted) const
    {
        message.targetId = target.getId();
        message.enabled = enabled;
        // message.result = blox_DigitalState(logic.result());

        for (pb_size_t i = 0; i < digitals.size() && i < 16; i++) {
            digitals[0].write(message.digital[i], includeNotPersisted);
        }
        message.digital_count = digitals.size();
        for (pb_size_t i = 0; i < analogs.size() && i < 16; i++) {
            analogs[0].write(message.analog[i], includeNotPersisted);
        }
        message.analog_count = analogs.size();
        for (pb_size_t i = 0; i < digitals.size() && i < 16; i++) {
            message.symbols[i] = symbols[i];
        }
        message.symbols_count = symbols.size();
    }

    virtual cbox::CboxError
    streamTo(cbox::DataOut& out) const override final
    {
        blox_ActuatorLogic message = blox_ActuatorLogic_init_zero;
        writeMessage(message, true);

        return streamProtoTo(out, &message, blox_ActuatorLogic_fields, blox_ActuatorLogic_size);
    }

    virtual cbox::CboxError
    streamPersistedTo(cbox::DataOut& out) const override final
    {
        blox_ActuatorLogic message = blox_ActuatorLogic_init_zero;
        writeMessage(message, false);

        return streamProtoTo(out, &message, blox_ActuatorLogic_fields, blox_ActuatorLogic_size);
    }

    virtual cbox::update_t
    update(const cbox::update_t& now) override final
    {
        if (enabled) {
            // logic.update();
        }
        return now + 100; // update every 100ms
    }

    virtual void*
    implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewBloxTypes_BlockType_ActuatorLogic) {
            return this; // me!
        }
        return nullptr;
    }
};
