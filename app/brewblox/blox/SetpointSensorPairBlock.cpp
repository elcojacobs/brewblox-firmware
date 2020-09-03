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

#include "SetpointSensorPairBlock.h"
#include "SetpointSensorPair.pb.h"
#include "blox/FieldTags.h"

cbox::CboxError
SetpointSensorPairBlock::streamFrom(cbox::DataIn& in)
{
    blox_SetpointSensorPair newData = blox_SetpointSensorPair_init_zero;
    cbox::CboxError res = streamProtoFrom(in, &newData, blox_SetpointSensorPair_fields, blox_SetpointSensorPair_size);
    /* if no errors occur, write new settings to wrapped object */
    if (res == cbox::CboxError::OK) {
        pair.setting(cnl::wrap<temp_t>(newData.storedSetting));
        pair.settingValid(newData.settingEnabled);
        pair.filterChoice(uint8_t(newData.filter));
        pair.filterThreshold(cnl::wrap<fp12_t>(newData.filterThreshold));

        if (newData.resetFilter || sensor.getId() != newData.sensorId) {
            sensor.setId(newData.sensorId);
            pair.resetFilter();
        }
        pair.update(); // force an update that bypasses the update interval
    }
    return res;
}

cbox::CboxError
SetpointSensorPairBlock::streamTo(cbox::DataOut& out) const
{
    blox_SetpointSensorPair message = blox_SetpointSensorPair_init_zero;
    FieldTags stripped;
    message.sensorId = sensor.getId();
    message.settingEnabled = pair.settingValid();
    message.storedSetting = cnl::unwrap(pair.setting());
    if (pair.valueValid()) {
        message.value = cnl::unwrap(pair.value());
    } else {
        stripped.add(blox_SetpointSensorPair_value_tag);
    }
    if (pair.settingValid()) {
        message.setting = cnl::unwrap(pair.setting());
    } else {
        stripped.add(blox_SetpointSensorPair_setting_tag);
    };
    if (pair.sensorValid()) {
        message.valueUnfiltered = cnl::unwrap(pair.valueUnfiltered());
    } else {
        stripped.add(blox_SetpointSensorPair_valueUnfiltered_tag);
    }

    message.filter = blox_FilterChoice(pair.filterChoice());
    message.filterThreshold = cnl::unwrap(pair.filterThreshold());

    stripped.copyToMessage(message.strippedFields, message.strippedFields_count, 3);

    return streamProtoTo(out, &message, blox_SetpointSensorPair_fields, blox_SetpointSensorPair_size);
}

cbox::CboxError
SetpointSensorPairBlock::streamPersistedTo(cbox::DataOut& out) const
{
    blox_SetpointSensorPair message = blox_SetpointSensorPair_init_zero;
    message.sensorId = sensor.getId();
    message.storedSetting = cnl::unwrap(pair.setting());
    message.settingEnabled = pair.settingValid();
    message.filter = blox_FilterChoice(pair.filterChoice());
    message.filterThreshold = cnl::unwrap(pair.filterThreshold());

    return streamProtoTo(out, &message, blox_SetpointSensorPair_fields, blox_SetpointSensorPair_size);
}

cbox::update_t
SetpointSensorPairBlock::update(const cbox::update_t& now)
{
    bool doUpdate = false;
    auto nextUpdate = m_intervalHelper.update(now, doUpdate);

    if (doUpdate) {
        pair.update();
    }
    return nextUpdate;
}

void*
SetpointSensorPairBlock::implements(const cbox::obj_type_t& iface)
{
    if (iface == BrewBloxTypes_BlockType_SetpointSensorPair) {
        return this; // me!
    }
    if (iface == cbox::interfaceId<ProcessValue<temp_t>>()) {
        // return the member that implements the interface in this case
        ProcessValue<temp_t>* ptr = &pair;
        return ptr;
    }
    if (iface == cbox::interfaceId<SetpointSensorPair>()) {
        SetpointSensorPair* ptr = &pair;
        return ptr;
    }
    return nullptr;
}
