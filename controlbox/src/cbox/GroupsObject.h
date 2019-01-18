/*
 * Copyright 2014-2015 Matthew McGowan.
 * Copyright 2018 BrewBlox / Elco Jacobs
 * This file is part of Controlbox.
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

#pragma once

#include "Box.h"

namespace cbox {
// the GroupsObject can added to a box, so the active group can be written as a system object and is also persisted
class GroupsObject : public ObjectBase<std::numeric_limits<uint16_t>::max() - 1> {
    Box* myBox;

public:
    GroupsObject(Box* box)
        : myBox(box)
    {
    }

    virtual cbox::CboxError streamFrom(cbox::DataIn& dataIn) override final
    {
        uint8_t newGroups;
        if (!dataIn.get(newGroups)) {
            return CboxError::INPUT_STREAM_READ_ERROR; // LCOV_EXCL_LINE
        }
        myBox->setActiveGroupsAndUpdateObjects(newGroups);
        return CboxError::OK;
    }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
    {
        uint8_t groups = myBox->getActiveGroups();
        if (!out.put(groups)) {
            return CboxError::OUTPUT_STREAM_WRITE_ERROR; // LCOV_EXCL_LINE
        }
        return CboxError::OK;
    }

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final
    {
        return streamTo(out);
    }

    virtual update_t update(const update_t& now) override final
    {
        return cbox::Object::update_never(now);
    }
};

} // end namespace cbox
