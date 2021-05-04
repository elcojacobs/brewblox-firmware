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

#include "MockPinsBlock.h"
#include "IoArrayHelpers.h"
#include "proto/cpp/MockPins.pb.h"

cbox::CboxError
MockPinsBlock::streamTo(cbox::DataOut& out) const
{
    blox_MockPins message = blox_MockPins_init_zero;
    // looks a bit silly, but this way it is implemented the same as teh Spark2 and Spark3 blocks
    message.pins_count = 8;
    message.pins[0].which_Pin = blox_MockPinsIoPin_mock1_tag;
    readIo(mocks, 1, message.pins[0].Pin.mock1);
    message.pins[1].which_Pin = blox_MockPinsIoPin_mock2_tag;
    readIo(mocks, 2, message.pins[1].Pin.mock2);
    message.pins[2].which_Pin = blox_MockPinsIoPin_mock3_tag;
    readIo(mocks, 3, message.pins[2].Pin.mock3);
    message.pins[3].which_Pin = blox_MockPinsIoPin_mock4_tag;
    readIo(mocks, 4, message.pins[3].Pin.mock4);
    message.pins[4].which_Pin = blox_MockPinsIoPin_mock5_tag;
    readIo(mocks, 5, message.pins[4].Pin.mock5);
    message.pins[5].which_Pin = blox_MockPinsIoPin_mock6_tag;
    readIo(mocks, 6, message.pins[5].Pin.mock6);
    message.pins[6].which_Pin = blox_MockPinsIoPin_mock7_tag;
    readIo(mocks, 7, message.pins[6].Pin.mock7);
    message.pins[7].which_Pin = blox_MockPinsIoPin_mock8_tag;
    readIo(mocks, 8, message.pins[7].Pin.mock8);
    return streamProtoTo(out, &message, blox_MockPins_fields, blox_MockPins_size);
}

void*
MockPinsBlock::implements(const cbox::obj_type_t& iface)
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
