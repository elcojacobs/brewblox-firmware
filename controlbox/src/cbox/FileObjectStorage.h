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

#include "CboxError.h"
#include "DataStream.h"
#include "ObjectStorage.h"
#include <filesystem>
#include <string>

namespace cbox {
class FileObjectStorage : public ObjectStorage {
public:
    FileObjectStorage(const std::filesystem::path& root_);
    FileObjectStorage(std::filesystem::path&& root_);

    virtual ~FileObjectStorage() = default;

    virtual CboxError storeObject(
        const storage_id_t& id,
        const std::function<CboxError(DataOut&)>& handler) override final;

    virtual CboxError retrieveObject(
        const storage_id_t& id,
        const std::function<CboxError(RegionDataIn&)>& handler) override final;

    virtual CboxError retrieveObjects(
        const std::function<CboxError(const storage_id_t& id, RegionDataIn&)>& handler) override final;

    virtual bool disposeObject(const storage_id_t& id, bool mergeDisposed = true) override final;

    virtual void clear() override final;

    stream_size_t freeSpace();

private:
    std::filesystem::path root;

    inline uint8_t
    storageVersion() const
    {
        return 0x01;
    }

    std::filesystem::path getPath(const storage_id_t& id)
    {
        return root / std::to_string(uint16_t(id));
    }
};

} // end namespace cbox
