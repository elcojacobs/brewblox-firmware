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

    virtual void async_read_impl(asio::streambuf& buffer_out, std::shared_ptr<CboxConnection> self) override;
    virtual void async_write_impl(asio::streambuf& buffer_out, std::shared_ptr<CboxConnection> self) override;

private:
    asio::ip::tcp::socket socket;
};
