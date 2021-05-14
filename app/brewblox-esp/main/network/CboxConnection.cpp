#include "CboxConnection.hpp"
#include "CboxConnectionManager.hpp"
#include "cbox/Box.h"

CboxTcpConnection::CboxTcpConnection(
    asio::ip::tcp::socket socket_,
    CboxConnectionManager& connection_manager_,
    cbox::Box& box_)
    : socket(std::move(socket_))
    , connection_manager(connection_manager_)
    , box(box_)
{
}

void CboxTcpConnection::do_read()
{
    auto self(shared_from_this());
    asio::async_read_until(
        socket,
        make_view(buffer_in),
        '\n',
        [this, self](std::error_code ec, std::size_t bytes_transferred) {
            if (!ec) {
                cbox::StreamBufDataIn in_cbox(buffer_in);
                cbox::StreamBufDataOut out_cbox(buffer_out);
                box.parseMessage(in_cbox, out_cbox);
                do_write();
            } else if (ec != asio::error::operation_aborted) {
                connection_manager.stop(shared_from_this());
            }
        });
}

void CboxTcpConnection::do_write()
{
    auto self(shared_from_this());
    asio::async_write(
        socket, make_view(buffer_out),
        [this, self](std::error_code ec, std::size_t) {
            if (!ec) {
                // Initiate graceful connection closure.
                asio::error_code ignored_ec;
                socket.shutdown(asio::ip::tcp::socket::shutdown_both,
                                ignored_ec);
            }

            if (ec != asio::error::operation_aborted) {
                connection_manager.stop(shared_from_this());
            }
        });
}