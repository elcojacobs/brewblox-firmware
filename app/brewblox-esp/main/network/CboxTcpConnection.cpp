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

void CboxTcpConnection::async_read_impl(asio::streambuf& buffer_out, std::shared_ptr<CboxConnection> self)
{
    asio::async_read_until(
        socket,
        buffer_in,
        '\n',
        [self{std::move(self)}](std::error_code ec, std::size_t bytes_transferred) {
            self->finish_read(ec, bytes_transferred);
        });
}

void CboxTcpConnection::async_write_impl(asio::streambuf& buffer_out, std::shared_ptr<CboxConnection> self)
{
    asio::async_write(
        socket,
        buffer_out,
        [self{std::move(self)}](std::error_code ec, std::size_t bytes_transferred) {
            self->finish_write(ec, bytes_transferred);
        });
}