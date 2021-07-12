#pragma once

#include "CircularBufferView.hpp"
#include "cbox/DataStreamIo.h"
#include <asio.hpp>

namespace cbox {

class StreamBufDataIn : public DataIn {
    std::streambuf& in;

public:
    StreamBufDataIn(std::streambuf& in_)
        : in(in_)
    {
    }

    virtual int16_t read() override
    {
        return in.sbumpc();
    }

    virtual int16_t peek() override
    {
        return in.sgetc();
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

class CboxConnectionManager;
namespace cbox {
class Box;
}

class CboxTcpConnection : public std::enable_shared_from_this<CboxTcpConnection> {
public:
    CboxTcpConnection(const CboxTcpConnection&) = delete;
    CboxTcpConnection& operator=(const CboxTcpConnection&) = delete;

    explicit CboxTcpConnection(
        asio::ip::tcp::socket socket_,
        CboxConnectionManager& connection_manager_,
        cbox::Box& box_);
    ~CboxTcpConnection() = default;

    void start();
    void stop();
    void do_read();
    void do_write();

private:
    asio::ip::tcp::socket socket;
    asio::streambuf buffer_in;
    asio::streambuf buffer_out;
    CboxConnectionManager& connection_manager;

    cbox::Box& box;
};

using CboxConnectionPtr = std::shared_ptr<CboxTcpConnection>;