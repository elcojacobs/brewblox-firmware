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
enum class SectionOp : uint8_t {
    OR,
    AND,
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

class ActuatorSection;
class CompareSection;

class Section {
public:
    Section(SectionOp o, CombineOp c)
        : m_sectionOp(o)
        , m_combineOp(c)
    {
    }
    virtual ~Section() = default;

    SectionOp sectionOp() const
    {
        return m_sectionOp;
    }
    CombineOp combineOp() const
    {
        return m_combineOp;
    }
    virtual bool eval() const = 0;
    virtual ActuatorSection* asActuatorSection()
    {
        return nullptr;
    }
    virtual CompareSection* asCompareSection()
    {
        return nullptr;
    }

private:
    SectionOp m_sectionOp;
    CombineOp m_combineOp;
};

class ActuatorSection : public Section {
protected:
    using lookup_t = std::function<std::shared_ptr<const ActuatorDigitalConstrained>()>;
    std::vector<lookup_t> lookups;

public:
    ActuatorSection(SectionOp o, CombineOp c)
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
        switch (sectionOp()) {
        case SectionOp::OR:
            return evalOr();
        case SectionOp::AND:
            return evalAnd();
        case SectionOp::XOR:
            return evalXor();
        case SectionOp::GE:
            return false;
        case SectionOp::LE:
            return false;
        }
        return false;
    }

    virtual ActuatorSection* asActuatorSection() override final
    {
        return this;
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
    lookup_t m_source;
    bool m_useSetting; // when true use setting instead of value
    fp12_t m_threshold;

public:
    CompareSection(SectionOp o, CombineOp c, lookup_t&& l, bool u, fp12_t t)
        : Section(o, c)
        , m_source(l)
        , m_useSetting(u)
        , m_threshold(t)
    {
    }
    virtual ~CompareSection() = default;

    virtual bool eval() const override final
    {
        if (auto sourcePtr = m_source()) {
            fp12_t val = 0;
            if (m_useSetting) {
                if (!sourcePtr->settingValid()) {
                    return false;
                }
                val = sourcePtr->setting();
            } else {
                if (!sourcePtr->valueValid()) {
                    return false;
                }
                val = sourcePtr->value();
            }
            if (sectionOp() == SectionOp::GE) {
                return (val >= m_threshold);
            }
            if (sectionOp() == SectionOp::LE) {
                return (val <= m_threshold);
            }
        }
        return false;
    }

    bool useSetting() const
    {
        return m_useSetting;
    }

    fp12_t threshold() const
    {
        return m_threshold;
    }

    virtual CompareSection* asCompareSection() override final
    {
        return this;
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
            auto cop = s->get()->combineOp();
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