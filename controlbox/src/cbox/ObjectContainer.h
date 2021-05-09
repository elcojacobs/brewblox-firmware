/*
 * Copyright 2018 Elco Jacobs / BrewBlox
 *
 * This file is part of ControlBox
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

#include "ContainedObject.h"
#include "ObjectStorage.h"
#include <vector>

namespace cbox {

class ObjectContainer {
private:
    std::vector<ContainedObject> objects;
    obj_id_t startId = obj_id_t::start();
    ObjectStorage& storage;

public:
    using Iterator = decltype(objects)::iterator;
    using CIterator = decltype(objects)::const_iterator;

    ObjectContainer(ObjectStorage& storage_)
        : objects{}
        , storage(storage_)
    {
    }

    ObjectContainer(std::initializer_list<ContainedObject> systemObjects, ObjectStorage& storage_)
        : objects{systemObjects}
        , storage(storage_)
    {
    }

    virtual ~ObjectContainer() = default;

private:
    std::pair<Iterator, Iterator> findPosition(obj_id_t id);

    obj_id_t nextId() const;

public:
    ContainedObject* fetchContained(obj_id_t id);
    const std::weak_ptr<Object> fetch(obj_id_t id);

    /**
     * set start ID for user objects.
     * ID's smaller than the start ID are  assumed to be system objects and considered undeletable.
     **/
    void setObjectsStartId(obj_id_t id)
    {
        startId = id;
    }

    // create a new object and let box assign id
    obj_id_t add(std::shared_ptr<Object>&& obj, uint8_t active_in_groups)
    {
        return add(std::move(obj), active_in_groups, obj_id_t::invalid());
    }

    // create a new object with specific id, optionally replacing an existing object
    obj_id_t add(std::shared_ptr<Object>&& obj, uint8_t active_in_groups, obj_id_t id, bool replace = false);

    CboxError remove(obj_id_t id);

    // only const iterators are exposed. We don't want the caller to be able to modify the container
    CIterator cbegin()
    {
        return objects.cbegin();
    }

    CIterator cend()
    {
        return objects.cend();
    }

    CIterator userbegin()
    {
        return findPosition(startId).first;
    }

    // replace an object with an inactive object by const iterator
    void deactivate(const CIterator& cit);

    // replace an object with an inactive object by id
    void deactivate(obj_id_t id);

    // remove all non-system objects from the container
    void clear();

    // remove all objects from the container
    void clearAll();

    void update(update_t now);

    void forcedUpdate(update_t now);

    CboxError store(const obj_id_t& id);

    CboxError reloadStored(const obj_id_t& id);
};

} // end namespace cbox
