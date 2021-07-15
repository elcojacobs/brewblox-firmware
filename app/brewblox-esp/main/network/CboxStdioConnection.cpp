#include "CboxStdioConnection.hpp"
#include <unistd.h>

// note: not working on esp32, linking misses functions
// asio doesn't support serial port according to documentation

CboxStdioConnection::CboxStdioConnection(
    asio::io_context& io_context_,
    CboxConnectionManager& connection_manager_,
    cbox::Box& box_)
    : CboxConnection(connection_manager_, box_)
    , in(io_context_, ::dup(STDIN_FILENO))
    , out(io_context_, ::dup(STDOUT_FILENO))
{
}

void CboxStdioConnection::async_read_impl(asio::streambuf& buffer_out, std::shared_ptr<CboxConnection> self)
{
    asio::async_read_until(
        in,
        buffer_in,
        '\n',
        [self{std::move(self)}](std::error_code ec, std::size_t bytes_transferred) {
            self->finish_read(ec, bytes_transferred);
        });
}

void CboxStdioConnection::async_write_impl(asio::streambuf& buffer_out, std::shared_ptr<CboxConnection> self)
{
    asio::async_write(
        out,
        buffer_out,
        [self{std::move(self)}](std::error_code ec, std::size_t bytes_transferred) {
            self->finish_write(ec, bytes_transferred);
        });
}