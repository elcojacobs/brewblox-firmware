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

class CboxConnection : public std::enable_shared_from_this<CboxConnection> {
public:
    CboxConnection(const CboxConnection&) = delete;
    CboxConnection& operator=(const CboxConnection&) = delete;

    explicit CboxConnection(
        CboxConnectionManager& connection_manager_,
        cbox::Box& box_);
    virtual ~CboxConnection() = default;

    virtual void start();
    virtual void stop();
    virtual void do_read() = 0;
    virtual void do_write() = 0;

    void handle_read(std::error_code ec, std::size_t bytes_transferred);
    void handle_write(std::error_code ec, std::size_t bytes_transferred);

protected:
    // asio::posix::stream_descriptor input;
    // asio::posix::stream_descriptor output;
    asio::streambuf buffer_in;
    asio::streambuf buffer_out;
    CboxConnectionManager& connection_manager;

    cbox::Box& box;
};

using CboxConnectionPtr = std::shared_ptr<CboxConnection>;