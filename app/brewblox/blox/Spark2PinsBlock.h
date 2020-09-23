/*
 * Copyright 2019 BrewPi B.V.
 *
 * This file is part of BrewPi.
 *
 * BrewPi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BrewPi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BrewPi.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "blox/Block.h"
#include "blox/SparkIoBase.h"

class Spark2PinsBlock : public SparkIoBase, public Block<BrewBloxTypes_BlockType_Spark2Pins> {
private:
    static const uint8_t numPins = 4;
    virtual pin_t channelToPin(uint8_t channel) const override final;

public:
    Spark2PinsBlock();

    virtual cbox::CboxError streamFrom(cbox::DataIn& in) override final;

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final;
    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final;

    virtual void* implements(const cbox::obj_type_t& iface) override final;

    virtual cbox::update_t
    update(const cbox::update_t& now) override final
    {
        return update_never(now);
    }
};
