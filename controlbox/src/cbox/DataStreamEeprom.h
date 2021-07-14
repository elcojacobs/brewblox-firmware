/*
 * Copyright 2014-2020 BrewPi B.V. / Elco Jacobs
 * based on earlier work of Matthew McGowan.
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

#include "DataStream.h"
#include "EepromAccess.h"

namespace cbox {

/**
 * Provides storage for a stream offset and length.
 */
template <typename Offset, typename Length>
class StreamRegion {
protected:
    Offset _offset = 0;
    Length _length = 0;

public:
    Offset offset() { return _offset; }
    Length length() { return _length; }

    void reset(Offset o, Length l)
    {
        _offset = o;
        _length = l;
    }
};

struct EepromStreamRegion : public StreamRegion<uint16_t, stream_size_t> {
};

/**
 * A datastream implementation that writes to a region of eeprom.
 * Eeprom is written via the EepromAccess helper.
 * The stream is only written up to the length specified - once that length has been filled
 * writes are silently failed.
 * @see EepromAccess
 */
class EepromDataOut final : public DataOut, public EepromStreamRegion {
private:
    EepromAccess& eepromAccess;

public:
    EepromDataOut(EepromAccess& ea)
        : eepromAccess(ea)
    {
    }

    virtual bool write(uint8_t value) override final
    {
        if (_length) {
            eepromAccess.writeByte(_offset++, value);
            _length--;
            return true;
        }
        return false; // LCOV_EXCL_LINE: doesn't happen if length is managed properly
    }
};

/**
 * A data input stream that reads from a region of eeprom.
 * @see EepromAccess
 */
class EepromDataIn : public DataIn, public EepromStreamRegion {
private:
    EepromAccess& eepromAccess;

public:
    EepromDataIn(EepromAccess& ea)
        : eepromAccess(ea)
    {
    }

    virtual int16_t peek() override final
    {
        return eepromAccess.readByte(_offset);
    }

    virtual int16_t read() override final
    {
        int16_t result = -1;
        if (_length) {
            result = eepromAccess.readByte(_offset);
        }
        if (result >= 0) {
            _length--;
            _offset++;
        }
        return result;
    }

    bool skip(stream_size_t skip_length)
    {
        auto skip = std::min(skip_length, _length);
        _offset += skip;
        _length -= skip;
        return skip == skip_length;
    }

    virtual StreamType streamType() const override final
    {
        return StreamType::Eeprom;
    }

    stream_size_t available()
    {
        return _length;
    }
};
}
