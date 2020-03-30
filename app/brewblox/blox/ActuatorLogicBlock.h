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
        : m_lookup(objects, cbox::obj_id_t(data.id))
        , m_op(data.op)
        , m_result(blox_ActuatorLogic_Result_FALSE)
        , m_rhs(ActuatorDigitalBase::State(data.rhs))
    {
    }

    ~DigitalCompare() = default;

    blox_ActuatorLogic_Result eval() const
    {
        if (auto actPtr = m_lookup.const_lock()) {
            switch (m_op) {
            case blox_ActuatorLogic_DigitalCompareOp_VALUE_IS:
                return blox_ActuatorLogic_Result(actPtr->state() == m_rhs);
            case blox_ActuatorLogic_DigitalCompareOp_VALUE_ISNOT:
                return blox_ActuatorLogic_Result(actPtr->desiredState() != m_rhs);
            case blox_ActuatorLogic_DigitalCompareOp_DESIRED_IS:
                return blox_ActuatorLogic_Result(actPtr->desiredState() == m_rhs);
            case blox_ActuatorLogic_DigitalCompareOp_DESIRED_ISNOT:
                return blox_ActuatorLogic_Result(actPtr->desiredState() != m_rhs);
            }
            return blox_ActuatorLogic_Result_INVALID_DIGITAL_OP;
        }
        return blox_ActuatorLogic_Result_BLOCK_NOT_FOUND;
    }

    void write(blox_ActuatorLogic_DigitalCompare& dest, bool includeNotPersisted) const
    {
        dest.id = m_lookup.getId();
        dest.op = m_op;
        dest.rhs = blox_DigitalState(m_rhs);
        if (includeNotPersisted) {
            dest.result = m_result;
        }
    }

    void update()
    {
        m_result = eval();
    }

    auto result() const
    {
        return m_result;
    }

private:
    cbox::CboxPtr<ActuatorDigitalConstrained> m_lookup;
    blox_ActuatorLogic_DigitalCompareOp m_op;
    blox_ActuatorLogic_Result m_result;
    ActuatorDigitalBase::State m_rhs;
};

class AnalogCompare {
public:
    AnalogCompare(const blox_ActuatorLogic_AnalogCompare& data, cbox::ObjectContainer& objects)
        : m_lookup(objects, cbox::obj_id_t(data.id))
        , m_op(data.op)
        , m_result(blox_ActuatorLogic_Result_FALSE)
        , m_rhs(cnl::wrap<fp12_t>(data.rhs))
    {
    }

    ~AnalogCompare() = default;

    blox_ActuatorLogic_Result eval() const
    {
        if (auto pvPtr = m_lookup.const_lock()) {
            switch (m_op) {
            case blox_ActuatorLogic_AnalogCompareOp_VALUE_LE:
                if (!pvPtr->valueValid()) {
                    return blox_ActuatorLogic_Result_FALSE;
                }
                return blox_ActuatorLogic_Result(pvPtr->value() <= m_rhs);
            case blox_ActuatorLogic_AnalogCompareOp_VALUE_GE:
                if (!pvPtr->valueValid()) {
                    return blox_ActuatorLogic_Result_FALSE;
                }
                return blox_ActuatorLogic_Result(pvPtr->value() >= m_rhs);
            case blox_ActuatorLogic_AnalogCompareOp_SETTING_LE:
                if (!pvPtr->settingValid()) {
                    return blox_ActuatorLogic_Result_FALSE;
                }
                return blox_ActuatorLogic_Result(pvPtr->setting() <= m_rhs);
            case blox_ActuatorLogic_AnalogCompareOp_SETTING_GE:
                if (!pvPtr->settingValid()) {
                    return blox_ActuatorLogic_Result_FALSE;
                }
                return blox_ActuatorLogic_Result(pvPtr->setting() >= m_rhs);
            }
            return blox_ActuatorLogic_Result_INVALID_ANALOG_OP;
        }
        return blox_ActuatorLogic_Result_BLOCK_NOT_FOUND;
    }

    void write(blox_ActuatorLogic_AnalogCompare& dest, bool includeNotPersisted) const
    {
        dest.id = m_lookup.getId();
        dest.op = m_op;
        dest.rhs = cnl::unwrap(m_rhs);
        if (includeNotPersisted) {
            dest.result = m_result;
        }
    }

    void update()
    {
        m_result = eval();
    }

    auto result() const
    {
        return m_result;
    }

private:
    cbox::CboxPtr<ProcessValue<fp12_t>> m_lookup;
    blox_ActuatorLogic_AnalogCompareOp m_op;
    blox_ActuatorLogic_Result m_result;
    fp12_t m_rhs;
};

class ActuatorLogicBlock : public Block<BrewBloxTypes_BlockType_ActuatorLogic> {
private:
    cbox::ObjectContainer& objectsRef; // remember object container reference to create constraints
    cbox::CboxPtr<ActuatorDigitalConstrained> target;
    bool enabled = false;
    std::vector<DigitalCompare> digitals;
    std::vector<AnalogCompare> analogs;
    std::string expression;
    blox_ActuatorLogic_Result m_result = blox_ActuatorLogic_Result_FALSE;

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

            expression = std::string(newData.expression);
        }
        return result;
    }

    void
    writeMessage(blox_ActuatorLogic& message, bool includeNotPersisted) const
    {
        message.targetId = target.getId();
        message.enabled = enabled;
        if (includeNotPersisted) {
            message.result = m_result;
        }

        for (pb_size_t i = 0; i < digitals.size() && i < 16; i++) {
            digitals[i].write(message.digital[i], includeNotPersisted);
        }
        message.digital_count = digitals.size();
        for (pb_size_t i = 0; i < analogs.size() && i < 16; i++) {
            analogs[i].write(message.analog[i], includeNotPersisted);
        }
        message.analog_count = analogs.size();

        expression.copy(message.expression, 64);
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
        m_result = evaluate();
        if (enabled) {
            if (auto targetPtr = target.lock()) {
                if (m_result == blox_ActuatorLogic_Result_TRUE) {
                    targetPtr->desiredState(ActuatorDigitalBase::State::Active);
                } else {
                    targetPtr->desiredState(ActuatorDigitalBase::State::Inactive);
                }
            }
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

    blox_ActuatorLogic_Result evaluate()
    {
        for (auto& d : digitals) {
            d.update();
        }

        for (auto& a : analogs) {
            a.update();
        }

        auto it = expression.cbegin();
        auto result = eval(it);
        return result;
    }

private:
    blox_ActuatorLogic_Result eval(std::string::const_iterator& it) const
    {
        blox_ActuatorLogic_Result res = blox_ActuatorLogic_Result_UNEXPECTED_END;
        while (it < expression.cend()) {
            auto c = *it;
            if (c == ')') {
                return blox_ActuatorLogic_Result_UNEXPECTED_CLOSING_BRACKET;
            }
            ++it;
            if ('a' <= c && c <= 'z') {
                auto compare = digitals.cbegin() + (c - 'a');
                if (compare >= digitals.cend()) {
                    return blox_ActuatorLogic_Result_INVALID_DIG_COMPARE_IDX;
                }
                res = compare->result();
            } else if ('A' <= c && c <= 'Z') {
                auto compare = analogs.cbegin() + (c - 'A');
                if (compare >= analogs.cend()) {
                    return blox_ActuatorLogic_Result_INVALID_ANA_COMPARE_IDX;
                }
                res = compare->result();
            } else if (c == '|') {
                if (res == blox_ActuatorLogic_Result_TRUE) {
                    return res;
                }
                return eval(it);
            } else if (c == '&') {
                if (res == blox_ActuatorLogic_Result_TRUE) {
                    return eval(it);
                }
            } else if (c == '^') {
                auto rhs = eval(it);
                if (rhs != blox_ActuatorLogic_Result_TRUE && rhs != blox_ActuatorLogic_Result_FALSE) {
                    return rhs; // error
                }
                if (rhs != res) {
                    return blox_ActuatorLogic_Result_TRUE;
                }
                return blox_ActuatorLogic_Result_FALSE;
            } else if (c == '(') {
                return bracket_open(it);
            } else {
                return blox_ActuatorLogic_Result_UNEXPECTED_CHARACTER;
            }
        }
        return res;
    }

    blox_ActuatorLogic_Result bracket_open(std::string::const_iterator& it) const
    {
        auto res = eval(it);
        if (it < expression.cend()) {
            if (*it == ')') {
                ++it;
                return res;
            }
        }
        return blox_ActuatorLogic_Result_MISSING_CLOSING_BRACKET;
    }
};
