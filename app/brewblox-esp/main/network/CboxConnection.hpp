#pragma once

#include "BufferedConnection.hpp"
#include "cbox/Connections.h"
#include "cbox/DataStreamIo.h"

namespace cbox {

class StreamBufDataIn : public DataIn {
    std::streambuf& in;

public:
    StreamBufDataIn(std::streambuf& in_)
        : in(in_)
    {
    }

    virtual bool hasNext() override
    {
        return available() > 0;
    }

    virtual uint8_t next() override
    {
        if (hasNext()) {
            return in.sbumpc();
        }
        return 0;
    }

    virtual uint8_t peek() override
    {
        if (hasNext()) {
            return in.sgetc();
        }
        return 0;
    }

    virtual stream_size_t available() override
    {
        return in.in_avail();
    }

    virtual StreamType streamType() const override final
    {
        return StreamType::Tcp;
    }
};

/**
 * Provides a DataOut stream by wrapping a std::ostream.
 */
class StreamBufDataOut final : public DataOut {
    std::streambuf& out;

public:
    StreamBufDataOut(std::streambuf& out_)
        : out(out_)
    {
    }

    virtual bool write(uint8_t data) override final
    {
        out.sputc(data);
        if (data == '\n') {
            out.pubsync();
        }
        return true;
    }
};

} // end namespace cbox

class CboxTcpConnection : public cbox::Connection {
public:
    explicit CboxTcpConnection(asio::ip::tcp::socket&& socket)
        : conn(std::make_shared<BufferedConnection>(std::move(socket)))
        , in_buf(conn->buffer_in)
        , out_buf(conn->buffer_out)
        , in_cbox(in_buf)
        , out_cbox(out_buf)
    {
        conn->start();
        std::ostream(&out_buf) << "welcome" << std::endl;
    }
    ~CboxTcpConnection() = default;

    virtual cbox::DataOut& getDataOut() override final
    {
        return out_cbox;
    };
    virtual cbox::DataIn& getDataIn() override final
    {
        return in_cbox;
    };
    virtual bool isConnected() override final
    {
        return conn->is_connected();
    };
    virtual void stop() override final
    {
        conn->stop();
    }

private:
    std::shared_ptr<BufferedConnection> conn;
    std::streambuf& in_buf;
    std::streambuf& out_buf;
    cbox::StreamBufDataIn in_cbox;
    cbox::StreamBufDataOut out_cbox;
};