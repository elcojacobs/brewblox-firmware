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

#include "CboxError.h"

#include <cstdint>
#include <string>
namespace cbox {

using stream_size_t = uint16_t;

/**
 * An output stream that supports writing data.
 * This is the base class for raw streams that do not encode their bytes as 2 hex characters
 */
class DataOut {
public:
    DataOut() = default;
    virtual ~DataOut() = default;

    /**
	 * Writes a byte to the stream.
	 * @return {@code true} if the byte was successfully written, false otherwise.
	 */
    virtual bool write(uint8_t data) = 0;

    template <typename T>
    bool put(const T& t)
    {
        auto p = reinterpret_cast<const uint8_t*>(std::addressof(t));
        return writeBuffer(p, sizeof(T));
    }

    /**
	 * Writes a number of bytes to the stream.
	 * @param data	The address of the data to write.
	 * @param len	The number of bytes to write.
	 * @return {@code true} if the byte was successfully written, false otherwise.
	 */
    virtual bool writeBuffer(const uint8_t* data, stream_size_t len);

    inline bool writeBuffer(const char* data, stream_size_t len)
    {
        return writeBuffer(reinterpret_cast<const uint8_t*>(data), len);
    }
};

/**
 * An output stream that supports encoded data. Each byte is encoded as hex.
 * Special characters are used for stream handling and injecting meta data:
 * input|output
 * <comment>
 * <!event>
 * , list separator
 * \n end of message
 */
class DataOutEncoded {
private:
    DataOut& out;

public:
    DataOutEncoded(DataOut& _out)
        : out(_out)
    {
    }
    virtual ~DataOutEncoded() = default;

    virtual void writeResponseSeparator() = 0;
    virtual void writeListSeparator() = 0;
    virtual void endMessage() = 0;

    void writeWithoutEncoding()
    {
    }
};

/**
 * An output stream that buffers data before writing.
 */
class BufferDataOut : public DataOut {
private:
    uint8_t* buffer;
    stream_size_t size;
    stream_size_t pos;

public:
    BufferDataOut(uint8_t* _buffer, stream_size_t _size)
        : buffer(_buffer)
        , size(_size)
        , pos(0)
    {
    }
    virtual ~BufferDataOut() = default;

    void reset()
    {
        pos = 0;
    }

    virtual bool write(uint8_t data) override
    {
        if (pos < size) {
            buffer[pos++] = data;
            return true;
        }
        return false;
    }

    stream_size_t bytesWritten() { return pos; }

    const uint8_t* data()
    {
        return buffer;
    }
};

/**
 * A DataOut implementation that discards all data.
 */
class BlackholeDataOut final : public DataOut {
public:
    BlackholeDataOut() = default;
    virtual ~BlackholeDataOut() = default;
    virtual bool write(uint8_t) override final { return true; }
};

/**
 * A DataOut implementation that discards all data, but counts each byte;
 */
class CountingBlackholeDataOut final : public DataOut {
private:
    stream_size_t counted;

public:
    CountingBlackholeDataOut()
        : counted(0)
    {
    }
    virtual ~CountingBlackholeDataOut() = default;
    virtual bool write(uint8_t) override final
    {
        ++counted;
        return true;
    }

    stream_size_t count()
    {
        return counted;
    }
};

enum class StreamType : uint8_t {
    Mock = 0,
    Usb = 1,
    Tcp = 2,
    Eeprom = 3,
};

/**
 * A data input stream. The stream contents may be determined asynchronously.
 * hasNext() returns true if the stream may eventually produce a new item, false if the stream is closed.
 * next() fetches the next item from the stream. return value is undefined if available()==0.
 * peek() retrieves the next data in the stream without removing it. Result is undefined if
 * available() returns 0.
 * available() the number of times read can be called to retrieve valid data.
 */
class DataIn {
public:
    virtual ~DataIn() = default;
    /*
	 * Determines if there is potentially more data in this stream.
	 * Note that this is not dependent upon time and asynchronous delivery of data, but if the stream is still open.
	 */
    virtual bool hasNext() = 0;

    /**
	 * Retrieves the next byte of data. The return value is only valid when `hasNext()` returns true.
	 */
    virtual uint8_t next() = 0;

    /**
	 * Retrieves the next byte of data without removing it from the stream. The result is only valid if `available`
	 * previously returned a non-zero value.
	 */
    virtual uint8_t peek() = 0;

    /**
	 * Determines how many bytes are available for reading from the stream without blocking.
	 */
    virtual stream_size_t available() = 0;

    /**
	 * Discards all data until no new data is available
	 */
    void spool()
    {
        while (hasNext()) {
            next();
        }
    }

    /**
	 * Unconditional read of {@code length} bytes.
	 */
    bool read(uint8_t* t, stream_size_t length);

    template <typename T>
    bool get(T& t)
    {
        return read(reinterpret_cast<uint8_t*>(&t), sizeof(T));
    }

    /**
     * Writes the contents of this stream to an output stream.
     * @param out
     * @param length
     * @return length was written
     */
    bool push(DataOut& out, stream_size_t length);

    /**
     * Writes the contents of this stream to an output stream, until input stream is empty
     * @param out
     */
    bool push(DataOut& out);

    virtual StreamType streamType() const = 0;
};

/**
 * A DataIn that provides no data.
 */
class EmptyDataIn : public DataIn {
public:
    EmptyDataIn() = default;
    virtual ~EmptyDataIn() = default;

    virtual bool hasNext() override { return false; }
    virtual uint8_t next() override { return 0; }
    virtual uint8_t peek() override { return 0; }
    virtual stream_size_t available() override { return 0; }

    virtual StreamType streamType() const override final
    {
        return StreamType::Mock;
    }
};

/*
 * Reads data from a DataIn, and also writes the fetched bytes (if any) to a DataOut.
 */
class TeeDataIn : public DataIn {
    DataIn& in;
    DataOut& out;
    bool success;

public:
    TeeDataIn(DataIn& _in, DataOut& _out)
        : in(_in)
        , out(_out)
        , success(true)
    {
    }
    virtual ~TeeDataIn() = default;

    bool teeOk() { return success; }

    virtual uint8_t next() override
    {
        uint8_t val = in.next();
        bool result = out.write(val);
        success = success && result;
        return val;
    }

    virtual bool hasNext() override { return in.hasNext(); }
    virtual uint8_t peek() override { return in.peek(); }
    virtual stream_size_t available() override { return in.available(); }

    virtual StreamType streamType() const override final
    {
        return in.streamType();
    }
};

/*
 * A DataOut that writes to two other DataOut streams.
 */
class TeeDataOut final : public DataOut {
public:
    TeeDataOut(DataOut& _out1, DataOut& _out2)
        : out1(_out1)
        , out2(_out2)
    {
    }
    virtual ~TeeDataOut() = default;

    virtual bool write(uint8_t data) override
    {
        bool res1 = out1.write(data);
        bool res2 = out2.write(data);
        return res1 || res2;
    }

private:
    DataOut& out1;
    DataOut& out2;
};

/**
 * Provides a DataIn stream from a static buffer of data.
 */
class BufferDataIn : public DataIn {
    const uint8_t* data;
    stream_size_t size;
    stream_size_t pos;

public:
    BufferDataIn(const uint8_t* buf, stream_size_t len)
        : data(buf)
        , size(len)
        , pos(0)
    {
    }

    virtual uint8_t next() override { return data[pos++]; }
    virtual bool hasNext() override { return pos < size; }
    virtual uint8_t peek() override { return data[pos]; }
    virtual stream_size_t available() override { return size - pos; }
    void reset() { pos = 0; }
    stream_size_t bytes_read() { return pos; }

    virtual StreamType streamType() const override final
    {
        return StreamType::Mock;
    }
};

/**
 * Limits reading from the stream to the given number of bytes.
 */
class RegionDataIn final : public DataIn {
    DataIn& in;
    stream_size_t len;

public:
    RegionDataIn(DataIn& _in, stream_size_t _len)
        : in(_in)
        , len(_len)
    {
    }
    virtual ~RegionDataIn() = default;

    bool hasNext() override final
    {
        return len && in.hasNext();
    }

    uint8_t next() override final
    {
        return hasNext() ? --len, in.next() : 0;
    }

    uint8_t peek() override final
    {
        return in.peek();
    }

    stream_size_t available() override final
    {
        return std::min(len, in.available());
    }

    void reduceLength(stream_size_t newLen)
    {
        if (newLen < len) {
            len = newLen; // only allow making region smaller
        }
    }

    virtual StreamType streamType() const override final
    {
        return in.streamType();
    }
};

/**
 * Limits writing to the stream to the given number of bytes.
 */
class RegionDataOut final : public DataOut {
    DataOut* out; // use pointer to have assignment operator
    stream_size_t len;

public:
    RegionDataOut(DataOut& _out, stream_size_t _len)
        : out(&_out)
        , len(_len)
    {
    }
    virtual ~RegionDataOut() = default;

    virtual bool write(uint8_t data) override
    {
        if (len > 0) {
            --len;
            return out->write(data);
        }
        return false;
    }

    void setLength(stream_size_t len_)
    {
        len = len_;
    }
    stream_size_t availableForWrite()
    {
        return len;
    }
};

/**
 * CRC data out. Sends running CRC of data on request
 */
class CrcDataOut final : public DataOut {
    DataOut& out;
    uint8_t crcValue;

public:
    CrcDataOut(DataOut& _out, uint8_t initial = 0)
        : out(_out)
        , crcValue(initial)
    {
    }
    virtual ~CrcDataOut() = default;

    virtual bool write(uint8_t data) override final;

    bool writeCrc()
    {
        return out.write(crcValue);
    }

    void invalidateCrc()
    {
        crcValue += 1;
    }

    uint8_t crc()
    {
        return crcValue;
    }
};

/**
 * A DataOut decorator that converts from the 8-bit data bytes to ASCII Hex.
 */
class EncodedDataOut final : public DataOut {
private:
    uint8_t crcValue = 0;
    DataOut& out;

public:
    EncodedDataOut(DataOut& _out)
        : out(_out)
    {
    }

    void writeResponseSeparator()
    {
        // don't add CRC for the input, because it is already part of the input command
        crcValue = 0;
        out.write('|');
    }

    virtual void writeListSeparator()
    {
        write(crcValue);
        out.write(',');
    }

    /**
	 * Data is written as hex-encoded
	 */
    virtual bool write(uint8_t data) override final;

    uint8_t crc()
    {
        return crcValue;
    }

    void invalidateCrc()
    {
        crcValue += 1;
    }

    /**
	 * Rather than closing the global stream, write a newline to signify the end of this command.
	 */
    void endMessage();

    void writeAnnotation(std::string&& ann);

    void writeEvent(std::string&& ann);

    void writeError(CboxError error);
};

} // end namespace cbox
