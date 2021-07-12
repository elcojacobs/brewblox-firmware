#pragma once

#include "CboxConnection.hpp"
#include "cbox/DataStreamIo.h"
#include <asio.hpp>

class CboxTcpConnection : public CboxConnection {
public:
    explicit CboxTcpConnection(
        asio::ip::tcp::socket socket_,
        CboxConnectionManager& connection_manager_,
        cbox::Box& box_);
    virtual ~CboxTcpConnection() = default;

    // virtual void start();
    virtual void stop() override;
    virtual void do_read() override;
    virtual void do_write() override;

private:
    asio::ip::tcp::socket socket;
    // asio::posix::stream_descriptor input;
    // asio::posix::stream_descriptor output;
};
