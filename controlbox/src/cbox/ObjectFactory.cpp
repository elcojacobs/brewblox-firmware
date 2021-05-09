
/*
 * Copyright 2018 Elco Jacobs / BrewBlox, based on earlier work of Matthew McGowan
 *
 * This file is part of ControlBox.
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

#include "ObjectFactory.h"
#include <algorithm>

namespace cbox {

std::tuple<CboxError, std::shared_ptr<Object>> ObjectFactory::make(const obj_type_t& t) const
{
    auto factoryEntry = std::find_if(objTypes.begin(), objTypes.end(), [&t](const ObjectFactoryEntry& entry) { return entry.typeId == t; });
    if (factoryEntry == objTypes.end()) {
        return std::make_tuple(CboxError::OBJECT_NOT_CREATABLE, std::shared_ptr<Object>());
    }
    auto obj = (*factoryEntry).createFn();
    if (!obj) {
        return std::make_tuple(CboxError::INSUFFICIENT_HEAP, std::shared_ptr<Object>());
    }

    return std::make_tuple(CboxError::OK, std::move(obj));
}

} // end namespace cbox
