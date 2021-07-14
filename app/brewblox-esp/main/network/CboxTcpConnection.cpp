#include "CboxTcpConnection.hpp"

CboxTcpConnection::CboxTcpConnection(
    asio::ip::tcp::socket socket_,
    CboxConnectionManager& connection_manager_,
    cbox::Box& box_)
    : CboxConnection(connection_manager_, box_)
    , socket(std::move(socket_))
{
}

void CboxTcpConnection::stop()
{
    CboxConnection::stop();
    socket.close();
}

void CboxTcpConnection::do_read()
{
    auto self(shared_from_this());
    asio::async_read_until(
        socket,
        buffer_in, '\n',
        [this, self](std::error_code ec, std::size_t bytes_transferred) {
            handle_read(ec, bytes_transferred);
        });
}

void CboxTcpConnection::do_write()
{
    if (buffer_out.size()) {
        auto self(shared_from_this());
        asio::async_write(
            socket,
            buffer_out,
            [this, self](std::error_code ec, std::size_t bytes_transferred) {
                handle_write(ec, bytes_transferred);
            });
    }
}