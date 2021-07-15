#pragma once

#include "CboxConnection.hpp"
#include "cbox/DataStreamIo.h"
#include <asio.hpp>

class CboxStdioConnection : public CboxConnection {
public:
    explicit CboxStdioConnection(
        asio::io_context& io_context_,
        CboxConnectionManager& connection_manager_,
        cbox::Box& box_);
    virtual ~CboxStdioConnection() = default;

    // virtual void start();
    // virtual void stop();

    virtual void async_read_impl(asio::streambuf& buffer_out, std::shared_ptr<CboxConnection> self) override;
    virtual void async_write_impl(asio::streambuf& buffer_out, std::shared_ptr<CboxConnection> self) override;

private:
    asio::posix::stream_descriptor in;
    asio::posix::stream_descriptor out;
};
