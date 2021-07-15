#pragma once
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
    // virtual dispatch because only derived class knows stream type to pass to templated async read/write
    virtual void async_write_impl(asio::streambuf& buffer_out, std::shared_ptr<CboxConnection> self) = 0;
    virtual void async_read_impl(asio::streambuf& buffer_in, std::shared_ptr<CboxConnection> self) = 0;

    void start_read();
    void start_write();
    void finish_write(std::error_code ec, std::size_t bytes_transferred);
    void finish_read(std::error_code ec, std::size_t bytes_transferred);

protected:
    void handle_read(std::error_code ec, std::size_t bytes_transferred);
    void handle_write(std::error_code ec, std::size_t bytes_transferred);

    asio::streambuf buffer_in;
    asio::streambuf buffer_out;
    CboxConnectionManager& connection_manager;
    cbox::Box& box;
    bool writing = false;
};

using CboxConnectionPtr = std::shared_ptr<CboxConnection>;