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

#include "OneWireBusBlock.h"
#include "OneWire.h"
#include "OneWireAddress.h"
#include <limits>

#include "nanopb_callbacks.h"

bool
streamAdresses(pb_ostream_t* stream, const pb_field_t* field, void* const* arg)
{
    OneWireAddress address;
    OneWire* busPtr = reinterpret_cast<OneWire*>(*arg);
    if (busPtr == nullptr) {
        return false;
    }
    while (busPtr->search(address)) {
        if (!pb_encode_tag_for_field(stream, field)) {
            return false;
        }
        uint64_t addr = uint64_t(address);
        if (!pb_encode_fixed64(stream, &addr)) {
            return false;
        }
    }
    return true;
}

OneWireBusBlock::OneWireBusBlock(OneWire& ow)
    : bus(ow)
    , command({NO_OP, 0})
{
    bus.init();
}

/**
     * Read value
     * - an ID encoded as a variable length ID chain (values > 0x80 mean there is more data)
     * - previous command data count - N
     * - previous command data - N bytes
     * - command rsult (variable length)
     * - cmd 00: no-op always 00 (success)
     * - cmd 01: reset bus (00 on success, FF on failure)
     * - cmd 02: search bus: a sequence of 0 or more 8-byte addresses, MSB first that were found on the bus
     */
cbox::CboxError
OneWireBusBlock::streamTo(cbox::DataOut& out) const
{
    blox_OneWireBus message = blox_OneWireBus_init_zero;
    message.command = command;
    message.address.funcs.encode = nullptr;
    message.address.arg = &bus;
    switch (command.opcode) {
    case NO_OP:
        break;
    case RESET:
        bus.reset();
        break;
    case SEARCH:
        bus.reset_search();
        if (command.data) {
            bus.target_search(command.data);
        }
        message.address.funcs.encode = &streamAdresses;
        break;
    }
    // commands are one-shot - once the command is done clear it.
    command.opcode = NO_OP;
    command.data = 0;
    return streamProtoTo(out, &message, blox_OneWireBus_fields, std::numeric_limits<size_t>::max());
}

/**
     * Set the command to be executed next form the input stream
     * - byte 0: command
     *   00: no-op
     *   01: reset bus
     *   02: search bus:
     *   03: search the bus, limiting to the given family code byte passed as data
     *   (later: search bus alarm state?)
     *   (later: set bus power? (off if next byte is 00, on if it's 01) )
     */
cbox::CboxError
OneWireBusBlock::streamFrom(cbox::DataIn& dataIn)
{
    blox_OneWireBus message = blox_OneWireBus_init_zero;

    cbox::CboxError res = streamProtoFrom(dataIn, &message, blox_OneWireBus_fields, std::numeric_limits<size_t>::max());
    /* if no errors occur, write new settings to wrapped object */
    if (res == cbox::CboxError::OK) {
        command = message.command;
    }
    return res;
}
