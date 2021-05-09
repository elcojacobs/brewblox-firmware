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
#include "DataStreamEeprom.h"
#include "EepromAccess.h"
#include "EepromLayout.h"
#include "ObjectStorage.h"

namespace cbox {

enum class BlockType : uint8_t {
    invalid, // ensures cleared eeprom reads as invalid block type
    object,
    disposed_block,
};

inline bool
operator==(const uint8_t& a, const BlockType& b)
{
    return a == static_cast<uint8_t>(b);
}

class EepromObjectStorage : public ObjectStorage {
public:
    EepromObjectStorage(EepromAccess& _eeprom);
    virtual ~EepromObjectStorage() = default;

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

    stream_size_t continuousFreeSpace();

    void defrag();

private:
    /**
     * The application supplied EEPROM storage class
     */
    EepromAccess& eeprom;

    /**
     * Stream wrappers for reading, writing and limiting region
     */
    EepromDataIn reader;
    EepromDataOut writer;

    inline uint8_t
    magicByte() const
    {
        return 0x69;
    }
    inline uint8_t
    storageVersion() const
    {
        return 0x01;
    }
    inline uint16_t
    referenceHeader() const
    {
        return magicByte() << 8 | storageVersion();
    }

    void
    resetReader()
    {
        reader.reset(EepromLocation(objects), EepromLocationSize(objects));
    }
    void
    resetWriter()
    {
        writer.reset(EepromLocation(objects), EepromLocationSize(objects));
    }

    static uint16_t
    blockHeaderLength()
    {
        return sizeof(BlockType) + sizeof(uint16_t);
    }

    static uint16_t
    objectHeaderLength()
    {
        // actual size + id
        return blockHeaderLength() + sizeof(uint16_t) + sizeof(storage_id_t);
    }

    RegionDataIn getBlockReader(const BlockType requestedType);
    RegionDataOut getBlockWriter(const BlockType requestedType, uint16_t minSize);
    RegionDataIn getObjectReader(const storage_id_t id, bool usedSize);
    RegionDataOut getObjectWriter(const storage_id_t id);
    RegionDataOut newObjectWriter(const storage_id_t id, uint16_t objectSize);

    void init();
    bool moveDisposedBackwards();
    bool mergeDisposedBlocks();
};

} // end namespace cbox
