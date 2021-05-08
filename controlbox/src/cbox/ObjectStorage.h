/*
 * Copyright 2018 Elco Jacobs / BrewBlox.
 *
 * This file is part of BrewBlox.
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

#include <cstdint>
#include <functional>

namespace cbox {

using storage_id_t = uint16_t;

class ObjectStorage {
public:
    ObjectStorage() = default;
    virtual ~ObjectStorage() = default;

    virtual CboxError retrieveObject(
        const storage_id_t& id,
        const std::function<CboxError(RegionDataIn&)>& handler)
        = 0;
    virtual CboxError storeObject(
        const storage_id_t& id,
        const std::function<CboxError(DataOut&)>& handler)
        = 0;
    virtual CboxError retrieveObjects(
        const std::function<CboxError(const storage_id_t& id, RegionDataIn&)>& handler)
        = 0;
    virtual bool disposeObject(const storage_id_t& id, bool mergeDisposed = true) = 0;

    virtual void clear() = 0;
};

class ObjectStorageStub : public ObjectStorage {
public:
    ObjectStorageStub() = default;
    virtual ~ObjectStorageStub() = default;

    virtual CboxError retrieveObject(
        const storage_id_t& /*id*/,
        const std::function<CboxError(RegionDataIn&)>& /*handler*/) override final
    {
        return CboxError::PERSISTED_OBJECT_NOT_FOUND;
    }

    virtual CboxError storeObject(
        const storage_id_t& /*id*/,
        const std::function<CboxError(DataOut&)>& /*handler*/) override final
    {
        return CboxError::PERSISTED_STORAGE_WRITE_ERROR;
    }

    virtual CboxError retrieveObjects(
        const std::function<CboxError(const storage_id_t& /*id*/, RegionDataIn&)>& /*handler*/) override final
    {
        return CboxError::PERSISTED_OBJECT_NOT_FOUND;
    }

    virtual bool disposeObject(const storage_id_t& /*id*/, bool /*mergeDisposed = true*/) override final
    {
        return false;
    }

    virtual void clear() override final
    {
    }
};

} // end namespace cbox
