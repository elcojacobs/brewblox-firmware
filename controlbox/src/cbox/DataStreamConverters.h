/*
 * Copyright 2014-2015 Matthew McGowan.
 * Copyright 2018 BrewBlox / Elco Jacobs
 *
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

#include "DataStream.h"
#include "hex_utility.h"
#include <string>

namespace cbox {

/*
 * A DataIn filter - wraps a DataIn instance and provides also a DataIn interface.
 * Filters out non-significant text - comment markers, whitespace, unrecognized characters.
 * The stream automatically closes on newline and hasNext() returns false.
 * Once a character has been received, the underlying stream is continually polled for characters until
 * the stream is closed by the newline.
 *
 * The result of this is that lines are polled non-blocking while no data is available, and when data is available
 * the stream blocks for each character until the entire line is read.
 */

#if 0 // unused currently. Comments in input not supported.
class TextIn : public DataIn {
    DataIn* _in;
    uint8_t data;
    bool hasData;
    bool inLine;
    int8_t commentLevel; // -1 indicates end of stream

    void fetchNextData(bool optional);

public:
    TextIn(DataIn& in)
        : _in(&in)
        , data(0)
        , hasData(0)
        , inLine(false)
        , commentLevel(0)
    {
    }

    bool hasNext() override
    {
        fetchNextData(true);
        return hasData;
    }

    uint8_t next() override
    {
        fetchNextData(false);
        hasData = false;
        return data;
    }

    uint8_t peek() override
    {
        fetchNextData(true);
        return data;
    }

    stream_size_t available() override
    {
        return hasNext();
    }

    bool isClosed()
    {
        return commentLevel < 0;
    }
};
#endif

/*
 * Converts pairs of hex digit characters into the corresponding binary value.
 */
class HexTextToBinaryIn : public DataIn {
    DataIn& textIn;
    int nextByte = -1;

    bool peekEndline()
    {
        auto inByte = textIn.peek();
        return (inByte == '\r' || inByte == '\n');
    }

public:
    HexTextToBinaryIn(DataIn& _textIn)
        : textIn(_textIn)
    {
    }

    bool hasNext() override
    {
        return available() > 0;
    }

    void fetch()
    {
        if (nextByte < 0) {
            if (textIn.available() > 1) {
                nextByte = (h2d(textIn.next()) << 4) | h2d(textIn.next());
            }
        }
    }

    uint8_t peek() override
    {
        fetch();

        if (nextByte > 0) {
            return nextByte;
        }

        return 0;
    }

    uint8_t next() override
    {
        uint8_t r = peek();
        nextByte = -1;
        return r;
    }

    stream_size_t available() override
    {
        return textIn.available() / 2;
    }

    void unBlock()
    {
        while (peekEndline()) {
            textIn.next();
        }
    }

    virtual StreamType streamType() const override final
    {
        return textIn.streamType();
    }
};

} // end namespace cbox
