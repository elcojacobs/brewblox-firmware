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

#include "blox/Block.h"
#include "proto/cpp/OneWireBus.pb.h"

class OneWire;

class OneWireBusBlock : public Block<BrewBloxTypes_BlockType_OneWireBus> {
private:
    OneWire& bus;

    mutable _blox_OneWireBusCommand command; // declared mutable so const streamTo functions can reset it

    static const uint8_t NO_OP = 0;
    static const uint8_t RESET = 1;
    static const uint8_t SEARCH = 2; // pass family as data, 00 for all

public:
    OneWireBusBlock(OneWire& ow);
    virtual ~OneWireBusBlock() = default;

    OneWire& oneWire() { return bus; }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final;
    virtual cbox::CboxError streamFrom(cbox::DataIn& dataIn) override final;

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut&) const override final
    {
        return cbox::CboxError::OK;
    }

    virtual cbox::update_t update(const cbox::update_t& now) override final
    {
        // No updates for now. Alternatively, a periodic bus scan for new devices?
        return update_never(now);
    }
};
