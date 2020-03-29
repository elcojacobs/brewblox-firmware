/*
 * Copyright 2020 BrewPi B.V.
 *
 * This file is part of the BrewBlox Control Library.
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
 * along with BrewBlox. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "ActuatorDigital.h"
#include "ActuatorDigitalConstrained.h"
#include "ProcessValue.h"
#include <functional>
#include <memory>
#include <vector>

namespace ADLogic {
using State = ActuatorDigital::State;
enum class LogicOp : uint8_t {
    OR,
    AND,
    NOR,
    NAND,
    XOR,
    LE,
    GE,
};

enum class CombineOp : uint8_t {
    OR,
    AND,
    OR_NOT,
    AND_NOT
};

class Section {
public:
    Section(LogicOp o, CombineOp c)
        : m_op(o)
        , m_combineOp(c)
    {
    }
    virtual ~Section() = default;

    LogicOp op() const
    {
        return m_op;
    }
    CombineOp combineOp() const
    {
        return m_combineOp;
    }
    virtual bool eval() const = 0;

private:
    LogicOp m_op;
    CombineOp m_combineOp;
};

class ActuatorSection : public Section {
protected:
    using lookup_t = std::function<std::shared_ptr<const ActuatorDigitalConstrained>()>;
    std::vector<lookup_t> lookups;

public:
    ActuatorSection(LogicOp o, CombineOp c)
        : Section(o, c)
    {
    }

    virtual ~ActuatorSection() = default;

    void add(lookup_t&& act)
    {
        if (lookups.size() < 8) {
            lookups.push_back(std::move(act));
        }
    }

    void
    clear()
    {
        lookups.clear();
    }

    const auto& lookupsList() const
    {
        return lookups;
    }
    virtual bool eval() const override final
    {
        if (lookups.size() == 0) {
            return false;
        }
        switch (op()) {
        case LogicOp::OR:
            return evalOr();
        case LogicOp::AND:
            return evalAnd();
        case LogicOp::NOR:
            return !evalOr();
        case LogicOp::NAND:
            return !evalAnd();
        case LogicOp::XOR:
            return evalXor();
        case LogicOp::GE:
            return false;
        case LogicOp::LE:
            return false;
        }
        return false;
    }

private:
    bool evalOr() const
    {
        for (auto& lookup : lookups) {
            if (auto actPtr = lookup()) {
                if (actPtr->state() == State::Active) {
                    return true;
                }
            }
        }
        return false;
    }

    bool evalAnd() const
    {
        for (auto& lookup : lookups) {
            if (auto actPtr = lookup()) {
                if (actPtr->state() != State::Active) {
                    return false;
                }
            } else {
                return false;
            }
        }
        return true;
    }

    bool evalXor() const
    {
        uint16_t count = 0;
        for (auto& lookup : lookups) {
            if (auto actPtr = lookup()) {
                if (actPtr->state() == State::Active) {
                    ++count;
                }
            }
        }
        return count == 1;
    }
};

class CompareSection : public Section {
protected:
    using lookup_t = std::function<std::shared_ptr<const ProcessValue<fp12_t>>()>;
    lookup_t target;
    bool useSetting; // when true use setting instead of value
    fp12_t threshold;

public:
    CompareSection(LogicOp o, CombineOp c, lookup_t&& l, bool u, fp12_t t)
        : Section(o, c)
        , target(l)
        , useSetting(u)
        , threshold(t)
    {
    }
    virtual ~CompareSection() = default;

    virtual bool eval() const override final
    {
        if (auto targetPtr = target()) {
            fp12_t val = 0;
            if (useSetting) {
                if (!targetPtr->settingValid()) {
                    return false;
                }
                val = targetPtr->setting();
            } else {
                if (!targetPtr->valueValid()) {
                    return false;
                }
                val = targetPtr->value();
            }
            if (op() == LogicOp::GE) {
                return (val >= threshold);
            }
            if (op() == LogicOp::LE) {
                return (val <= threshold);
            }
        }
        return false;
    }
};

class ActuatorLogic {
private:
    std::vector<std::unique_ptr<Section>> m_sections;
    std::function<std::shared_ptr<ActuatorDigitalConstrained>()> m_target;
    bool m_result = false;

public:
    ActuatorLogic(std::function<std::shared_ptr<ActuatorDigitalConstrained>()>&& target_)
        : m_target(std::move(target_))
    {
    }

    ActuatorLogic(const ActuatorLogic&) = delete;
    ActuatorLogic& operator=(const ActuatorLogic&) = delete;
    ActuatorLogic(ActuatorLogic&&) = default;

    virtual ~ActuatorLogic() = default;

    void addSection(std::unique_ptr<Section>&& newSection)
    {
        if (m_sections.size() < 4) {
            m_sections.push_back(std::move(newSection));
        }
    }

    void
    clear()
    {
        m_sections.clear();
    }

    bool
    evaluate() const
    {
        bool result = false;
        for (auto s = m_sections.cbegin(); s < m_sections.cend(); s++) {
            bool sectionResult = s->get()->eval();
            // always use OR for first element
            auto cop = (s == m_sections.cbegin()) ? CombineOp::OR : s->get()->combineOp();
            switch (cop) {
            case CombineOp::OR:
                result = result || sectionResult;
                break;
            case CombineOp::AND:
                result = result && sectionResult;
                break;
            case CombineOp::OR_NOT:
                result = result || !sectionResult;
                break;
            case CombineOp::AND_NOT:
                result = result && !sectionResult;
                break;
            default:
                result = false;
            }
        }
        return result;
    }

    bool
    result() const
    {
        return m_result;
    }

    void
    update()
    {
        m_result = evaluate();
        if (auto actPtr = m_target()) {
            actPtr->desiredState(m_result ? State::Active : State::Inactive);
        }
    }

    const auto&
    sectionsList() const
    {
        return m_sections;
    }
};

} // end namespace ADLogic

using ActuatorLogic = ADLogic::ActuatorLogic;