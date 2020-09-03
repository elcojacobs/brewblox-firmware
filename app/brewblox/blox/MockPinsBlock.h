/*
 * Copyright 2019 BrewPi B.V.
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

#include "MockIoArray.h"
#include "blox/Block.h"

class MockPinsBlock : public Block<BrewBloxTypes_BlockType_MockPins> {
private:
    MockIoArray mocks;

public:
    MockPinsBlock() = default;
    virtual ~MockPinsBlock() = default;

    virtual cbox::CboxError streamFrom(cbox::DataIn&) override final
    {
        return cbox::CboxError::OK;
    }

    virtual cbox::CboxError
    streamTo(cbox::DataOut& out) const override final;

    virtual cbox::CboxError
    streamPersistedTo(cbox::DataOut&) const override final
    {
        return cbox::CboxError::OK; // nothing to persist
    }

    virtual void*
    implements(const cbox::obj_type_t& iface) override final;

    virtual cbox::update_t
    update(const cbox::update_t& now) override final
    {
        return update_never(now);
    }
};
