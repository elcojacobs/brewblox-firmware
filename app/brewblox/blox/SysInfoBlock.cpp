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
#include "Board.h"
#include "deviceid_hal.h"
#include "stringify.h"
#include <cstring>

#ifndef GIT_VERSION
#error GIT_VERSION not set
#endif

#if PLATFORM_ID != 3
#include "BrewPiTouch.h"
extern BrewPiTouch touch;
#endif

cbox::CboxError
SysInfoBlock::streamTo(cbox::DataOut& out) const
{
    blox_SysInfo message = blox_SysInfo_init_zero;

    HAL_device_ID(static_cast<uint8_t*>(&message.deviceId[0]), 12);

    strncpy(message.version, stringify(GIT_VERSION), 12);
    strncpy(message.protocolVersion, stringify(PROTO_VERSION), 12);
    strncpy(message.releaseDate, stringify(GIT_DATE), 12);
    strncpy(message.protocolDate, stringify(PROTO_DATE), 12);

    message.platform = blox_SysInfo_Platform(PLATFORM_ID);

    auto hw = blox_SysInfo_Hardware::blox_SysInfo_Hardware_unknown_hw;
    switch (getSparkVersion()) {
    case SparkVersion::V1:
        hw = blox_SysInfo_Hardware_Spark1;
        break;
    case SparkVersion::V2:
        hw = blox_SysInfo_Hardware_Spark2;
        break;
    case SparkVersion::V3:
        hw = blox_SysInfo_Hardware_Spark3;
        break;
    }
    message.hardware = hw;
#if PLATFORM_ID != 3
    message.voltage5 = touch.read5V();
    message.voltage12 = touch.read12V();
#endif
    return streamProtoTo(out, &message, blox_SysInfo_fields, blox_SysInfo_size);
}

cbox::CboxError
SysInfoBlock::streamFrom(cbox::DataIn& in)
{
    return cbox::CboxError::OK;
};

cbox::CboxError
SysInfoBlock::streamPersistedTo(cbox::DataOut& out) const
{
    return cbox::CboxError::OK;
}