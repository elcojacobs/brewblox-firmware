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
};

class Section {
protected:
    using lookup_t = std::function<std::shared_ptr<ActuatorDigitalConstrained>()>;
    std::vector<lookup_t> lookups;

public:
    Section() = default;
    virtual ~Section() = default;

    void add(lookup_t&& act)
    {
        if (lookups.size() < 8) {
            lookups.push_back(act);
        }
    }

    void
    clear()
    {
        lookups.clear();
    }

    const auto& lookupsList()
    {
        return lookups;
    }

    virtual LogicOp op() const = 0;
    virtual State eval() const = 0;
};

class OR : public Section {
public:
    OR() = default;
    virtual ~OR() = default;

    virtual State eval() const override
    {
        for (auto& lookup : lookups) {
            if (lookup()->state() == State::Active) {
                return State::Active;
            }
        }
        return State::Inactive;
    }

    virtual LogicOp op() const override
    {
        return LogicOp::OR;
    }
};

class AND : public Section {
public:
    AND() = default;
    virtual ~AND() = default;

    virtual State eval() const override
    {
        if (lookups.size() == 0) {
            return State::Inactive;
        }
        for (auto& lookup : lookups) {
            if (lookup()->state() != State::Active) {
                return State::Inactive;
            }
        }
        return State::Active;
    }

    virtual LogicOp op() const override
    {
        return LogicOp::AND;
    }
};

class NOR : public OR {
public:
    NOR() = default;
    virtual ~NOR() = default;

    virtual State eval() const override final
    {
        return OR::eval() ? State::Inactive : State::Active;
    }

    virtual LogicOp op() const override final
    {
        return LogicOp::NOR;
    }
};

class NAND : public AND {
public:
    NAND() = default;
    virtual ~NAND() = default;

    virtual State eval() const override final
    {
        return AND::eval() ? State::Inactive : State::Active;
    }

    virtual LogicOp op() const override final
    {
        return LogicOp::NAND;
    }
};

class XOR : public Section {
public:
    XOR() = default;
    virtual ~XOR() = default;

    virtual State eval() const override final
    {
        uint16_t count = 0;
        for (auto& lookup : lookups) {
            if (lookup()->state() == State::Active) {
                ++count;
            }
        }
        return count == 1 ? State::Active : State::Inactive;
    }

    virtual LogicOp op() const override final
    {
        return LogicOp::XOR;
    }
};

class ActuatorLogic {
private:
    struct LogicSection {
        LogicOp combineOp;
        std::unique_ptr<Section> section;
    };
    std::vector<LogicSection> sections;
    std::function<std::shared_ptr<ActuatorDigitalConstrained>()> target;

public:
    ActuatorLogic(std::function<std::shared_ptr<ActuatorDigitalConstrained>()>&& target_)
        : target(target_)
    {
    }

    ActuatorLogic(const ActuatorLogic&) = delete;
    ActuatorLogic& operator=(const ActuatorLogic&) = delete;
    ActuatorLogic(ActuatorLogic&&) = default;

    virtual ~ActuatorLogic() = default;

    void addSection(LogicOp op, std::unique_ptr<Section>&& newSection)
    {
        if (sections.size() == 0) {
            op = LogicOp::OR;
        }
        if (sections.size() < 4) {
            sections.push_back({op, std::move(newSection)});
        }
    }

    void clear()
    {
        sections.clear();
    }

    State state() const
    {
        auto result = State::Inactive;
        for (auto& s : sections) {
            auto sectionResult = s.section->eval();
            switch (s.combineOp) {
            case LogicOp::OR:
                result = (result == State::Active || sectionResult == State::Active) ? State::Active : State::Inactive;
                break;
            case LogicOp::AND:
                result = (result == State::Active && sectionResult == State::Active) ? State::Active : State::Inactive;
                break;
            case LogicOp::NOR:
                result = (result == State::Active || sectionResult == State::Active) ? State::Inactive : State::Active;
                break;
            case LogicOp::NAND:
                result = (result == State::Active && sectionResult == State::Active) ? State::Inactive : State::Active;
                break;
            case LogicOp::XOR:
                result = (result == State::Active ^ sectionResult == State::Active) ? State::Active : State::Inactive;
                break;
            default:
                result = State::Inactive;
            }
        }
        return result;
    }

    void
    update()
    {
        if (auto actPtr = target()) {
            actPtr->desiredState(state());
        }
    }

    const std::vector<LogicSection>&
    sectionList() const
    {
        return sections;
    }
};

std::unique_ptr<Section>
makeSection(LogicOp op)
{
    switch (op) {
    case LogicOp::OR:
        return std::make_unique<ADLogic::OR>();
    case LogicOp::AND:
        return std::make_unique<ADLogic::OR>();
    case LogicOp::NOR:
        return std::make_unique<ADLogic::OR>();
    case LogicOp::NAND:
        return std::make_unique<ADLogic::OR>();
    case LogicOp::XOR:
        return std::make_unique<ADLogic::OR>();
    }
    return std::unique_ptr<Section>(); // nullptr if invalid op
}
} // end namespace ADLogic

using ActuatorLogic = ADLogic::ActuatorLogic;