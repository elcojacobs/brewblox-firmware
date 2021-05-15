/*
 * Copyright 2018 BrewPi B.V.
 *
 * This file is part of Controlbox
 *
 * Controlbox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Controlbox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SysInfoBlock.h"
#include "cbox/Tracing.h"
#include "stringify.h"
#include <cstring>

#ifndef GIT_VERSION
#error GIT_VERSION not set
#endif

cbox::CboxError
SysInfoBlock::streamTo(cbox::DataOut& out) const
{
    blox_SysInfo message = blox_SysInfo_init_zero;

    device_id_func(static_cast<uint8_t*>(&message.deviceId[0]), 12);

    strncpy(message.version, stringify(GIT_VERSION), 12);
    strncpy(message.protocolVersion, stringify(PROTO_VERSION), 12);
    strncpy(message.releaseDate, stringify(GIT_DATE), 12);
    strncpy(message.protocolDate, stringify(PROTO_DATE), 12);

    message.platform = blox_SysInfo_Platform(PLATFORM_ID);

    // if (command == Command::SYS_CMD_TRACE_READ || command == Command::READ_AND_SYS_CMD_TRACE_RESUME) {
    //     auto history = cbox::tracing::history();
    //     auto it = history.cbegin();
    //     auto end = history.cend();
    //     for (uint8_t i = 0; i < 10 && it < end; i++, it++) {
    //         message.trace[i].action = blox_Trace_Action(it->action);
    //         message.trace[i].id = it->id;
    //         message.trace[i].type = it->type;
    //     }
    //     message.trace_count = 10;
    // }
    // if (command == Command::SYS_CMD_TRACE_RESUME || command == Command::READ_AND_SYS_CMD_TRACE_RESUME) {
    //     cbox::tracing::unpause();
    // }

    command = Command::NONE;

    return streamProtoTo(out, &message, blox_SysInfo_fields, blox_SysInfo_size);
}

cbox::CboxError
SysInfoBlock::streamFrom(cbox::DataIn& in)
{
    blox_SysInfo message = blox_SysInfo_init_zero;
    auto res = streamProtoFrom(in, &message, blox_SysInfo_fields, blox_SysInfo_size);
    if (res == cbox::CboxError::OK) {
        command = Command(message.command);
    }
    return res;
}

cbox::CboxError
SysInfoBlock::streamPersistedTo(cbox::DataOut&) const
{
    return cbox::CboxError::PERSISTING_NOT_NEEDED;
}