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
    virtual void do_read() override;
    virtual void do_write() override;

private:
    asio::posix::stream_descriptor in;
    asio::posix::stream_descriptor out;
};
