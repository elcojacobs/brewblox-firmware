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

#include "SetpointProfileBlock.h"
#include "blox/FieldTags.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "proto/cpp/SetpointProfile.pb.h"

bool
streamPointsOut(pb_ostream_t* stream, const pb_field_t* field, void* const* arg)
{
    const std::vector<SetpointProfileBlock::Point>* points = reinterpret_cast<std::vector<SetpointProfileBlock::Point>*>(*arg);
    for (const auto& p : *points) {
        auto submsg = blox_Point();
        submsg.time = p.time;
        submsg.temperature_oneof.temperature = cnl::unwrap(p.temp);
        submsg.which_temperature_oneof = blox_Point_temperature_tag;
        if (!pb_encode_tag_for_field(stream, field)) {
            return false;
        }
        if (!pb_encode_submessage(stream, blox_Point_fields, &submsg)) {
            return false;
        }
    }
    return true;
}

bool
streamPointsIn(pb_istream_t* stream, const pb_field_t*, void** arg)
{
    std::vector<SetpointProfileBlock::Point>* newPoints = reinterpret_cast<std::vector<SetpointProfileBlock::Point>*>(*arg);

    if (stream->bytes_left) {
        blox_Point submsg = blox_Point_init_zero;
        if (!pb_decode(stream, blox_Point_fields, &submsg)) {
            return false;
        }
        newPoints->push_back(SetpointProfileBlock::Point{submsg.time, cnl::wrap<decltype(SetpointProfileBlock::Point::temp)>(submsg.temperature_oneof.temperature)});
    }
    return true;
}

cbox::CboxError
SetpointProfileBlock::streamFrom(cbox::DataIn& in)
{
    blox_SetpointProfile newData = blox_SetpointProfile_init_zero;
    std::vector<Point> newPoints;
    newData.points.funcs.decode = &streamPointsIn;
    newData.points.arg = &newPoints;
    cbox::CboxError result = streamProtoFrom(in, &newData, blox_SetpointProfile_fields, std::numeric_limits<size_t>::max() - 1);
    if (result == cbox::CboxError::OK) {
        profile.points(std::move(newPoints));
        profile.enabled(newData.enabled);
        profile.startTime(newData.start);
        target.setId(newData.targetId);
    }
    return result;
}

cbox::CboxError
SetpointProfileBlock::streamTo(cbox::DataOut& out) const
{
    blox_SetpointProfile message = blox_SetpointProfile_init_zero;
    FieldTags stripped;
    message.points.funcs.encode = &streamPointsOut;
    message.points.arg = const_cast<std::vector<Point>*>(&profile.points());
    message.enabled = profile.enabled();
    message.start = profile.startTime();
    message.targetId = target.getId();
    if (profile.isDriving()) {
        message.drivenTargetId = target.getId();
    }

    cbox::CboxError result = streamProtoTo(out, &message, blox_SetpointProfile_fields, std::numeric_limits<size_t>::max() - 1);
    return result;
}

cbox::update_t
SetpointProfileBlock::update(const cbox::update_t& now)
{
    if (auto pTicks = ticksPtr.const_lock()) {
        auto time = pTicks->const_get().utc();
        profile.update(time);
    }
    return update_1s(now);
}

void*
SetpointProfileBlock::implements(const cbox::obj_type_t& iface)
{
    if (iface == BrewBloxTypes_BlockType_SetpointProfile) {
        return this; // me!
    }

    return nullptr;
}
