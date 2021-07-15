/*
 * Copyright 2021 Elco Jacobs / BrewBlox.
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

#include "FileObjectStorage.h"
#include "DataStreamIo.h"
#include <cstdio>
#include <dirent.h>
#include <fstream>

namespace cbox {

FileObjectStorage::FileObjectStorage(const std::string& root)
{
    rootLen = root.size();
    path.reserve(rootLen + 10);
    path = root;
    if (root.back() != '/') {
        path += '/';
        ++rootLen;
    }
}

/**
     * storeObject saves the data streamed by the handler under the given id.
     * it allocates a large enough block in EEPROM automatically and re-allocates if needed.
     * the handler should therefore stream the same data if it is called twice.
     * @param id: id to store the object with
     * @param handler: a callable that is provided with a DataOut to stream the new data to
     * @return CboxError
     */
CboxError FileObjectStorage::storeObject(
    const storage_id_t& id,
    const std::function<CboxError(DataOut&)>& handler)
{
    if (!id) {
        return CboxError::INVALID_OBJECT_ID;
    }

    // Do a test serialization
    BlackholeDataOut hole;
    CboxError res = handler(hole);

    if (res == CboxError::PERSISTING_NOT_NEEDED) {
        // exit for objects that don't need to exist in EEPROM. Not even their id/groups/existence
        return CboxError::OK;
    }

    if (res != CboxError::OK) {
        return res;
    };

    setPath(id);
    std::fstream fs(path, std::fstream::out | std::fstream::binary);
    if (!fs.is_open()) {
        return CboxError::PERSISTED_STORAGE_WRITE_ERROR;
    }
    OStreamDataOut outStream{fs};

    // Write to file overwriting old data
    auto writeWithCrc
        = [&id, &outStream, &handler]() -> CboxError {
        // write id to file as first 2 bytes
        CrcDataOut crcOut(outStream);
        crcOut.put(id);
        CboxError res = handler(crcOut);

        if (res == CboxError::OK) {
            // write CRC after object data so we can check integrity
            if (crcOut.writeCrc()) {
                return CboxError::OK;
            }
        }
        return CboxError::PERSISTED_STORAGE_WRITE_ERROR;
    };

    res = writeWithCrc();
    fs.flush();
    fs.close();

    if (res != CboxError::OK) {
        remove(path.c_str());
    }

    return res;
}

/**
     * Retrieve a single object from storage
     * @param id: id of object to retrieve
     * @param handler: a callable with the following prototype: (DataIn &) -> CboxError.
     * DataIn will contain the object's data followed by a CRC.
     * @return CboxError
     */
CboxError FileObjectStorage::retrieveObject(
    const storage_id_t& id,
    const std::function<CboxError(RegionDataIn&)>& handler)
{
    setPath(id);
    std::fstream fs(path, std::fstream::in | std::fstream::binary);
    if (!fs.is_open()) {
        return cbox::CboxError::PERSISTED_OBJECT_NOT_FOUND;
    }

    IStreamDataIn inStream{fs};
    RegionDataIn objectData(inStream, UINT16_MAX);
    // check that the first 2 bytes match the ID
    storage_id_t stored_id{0};
    if (objectData.get(stored_id) && stored_id == id) {
        return handler(objectData);
    }
    return cbox::CboxError::PERSISTED_OBJECT_NOT_FOUND;
}

/**
     * Retreive all objects from storage
     * @param handler: a callable with the following prototype: (const storage_id_t&, DataOut &) -> CboxError.
     * The handler will be called for each object and the object, with the DataIn stream containing the object's data.
     * @return
     */
CboxError FileObjectStorage::retrieveObjects(
    const std::function<CboxError(const storage_id_t& id, RegionDataIn&)>& handler)
{
    path.resize(rootLen);
    if (auto* dir = opendir(path.c_str())) {
        while (auto* entry = readdir(dir)) {
            if ((entry->d_type & DT_REG) == DT_REG) {
                path.resize(rootLen);
                path += entry->d_name;
                std::fstream fs(path, std::fstream::in | std::fstream::binary);
                IStreamDataIn inStream{fs};
                RegionDataIn objectData(inStream, UINT16_MAX);
                // check that the first 2 bytes match the ID
                storage_id_t stored_id{0};
                if (objectData.get(stored_id) && stored_id == atoi(entry->d_name)) {
                    handler(stored_id, objectData);
                }
            }
        }
        closedir(dir);
    }
    return CboxError::OK;
}

bool FileObjectStorage::disposeObject(const storage_id_t& id, bool mergeDisposed)
{
    setPath(id);
    return remove(path.c_str()) == 0;
}

void FileObjectStorage::clear()
{
    path.resize(rootLen);
    if (auto* dir = opendir(path.c_str())) {
        while (auto* entry = readdir(dir)) {
            if ((entry->d_type & DT_REG) == DT_REG) {
                path.resize(rootLen);
                path += entry->d_name;
                remove(path.c_str());
            }
        }
        closedir(dir);
    }
}

} // end namespace cbox
