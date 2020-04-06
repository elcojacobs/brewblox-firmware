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

#include "IoArrayHelpers.h"
#include "MockIoArray.h"
#include "blox/Block.h"
#include "proto/cpp/MockPins.pb.h"

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
    streamTo(cbox::DataOut& out) const override final
    {
        blox_MockPins message = blox_MockPins_init_zero;
        // looks a bit silly, but this way it is implemented the same as teh Spark2 and Spark3 blocks
        message.pins_count = 8;
        message.pins[0].which_Pin = blox_MockPins_IoPin_mock1_tag;
        readIo(mocks, 1, message.pins[0].Pin.mock1);
        message.pins[1].which_Pin = blox_MockPins_IoPin_mock2_tag;
        readIo(mocks, 2, message.pins[1].Pin.mock2);
        message.pins[2].which_Pin = blox_MockPins_IoPin_mock3_tag;
        readIo(mocks, 3, message.pins[2].Pin.mock3);
        message.pins[3].which_Pin = blox_MockPins_IoPin_mock4_tag;
        readIo(mocks, 4, message.pins[3].Pin.mock4);
        message.pins[4].which_Pin = blox_MockPins_IoPin_mock5_tag;
        readIo(mocks, 5, message.pins[4].Pin.mock5);
        message.pins[5].which_Pin = blox_MockPins_IoPin_mock6_tag;
        readIo(mocks, 6, message.pins[5].Pin.mock6);
        message.pins[6].which_Pin = blox_MockPins_IoPin_mock7_tag;
        readIo(mocks, 7, message.pins[6].Pin.mock7);
        message.pins[7].which_Pin = blox_MockPins_IoPin_mock8_tag;
        readIo(mocks, 8, message.pins[7].Pin.mock8);
        return streamProtoTo(out, &message, blox_MockPins_fields, blox_MockPins_size);
    }

    virtual cbox::CboxError
    streamPersistedTo(cbox::DataOut&) const override final
    {
        return cbox::CboxError::OK; // nothing to persist
    }

    virtual void*
    implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewBloxTypes_BlockType_MockPins) {
            return this; // me!
        }
        if (iface == cbox::interfaceId<IoArray>()) {
            // return the member that implements the interface in this case
            return &mocks;
        }
        return nullptr;
    }

    virtual cbox::update_t
    update(const cbox::update_t& now) override final
    {
        return update_never(now);
    }
};
