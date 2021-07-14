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

void CboxStdioConnection::do_read()
{
    auto self(shared_from_this());
    asio::async_read_until(
        in,
        buffer_in,
        '\n',
        [this, self](std::error_code ec, std::size_t bytes_transferred) {
            handle_read(ec, bytes_transferred);
        });
}

void CboxStdioConnection::do_write()
{
    if (buffer_out.size()) {
        auto self(shared_from_this());
        asio::async_write(
            out, buffer_out,
            [this, self](std::error_code ec, std::size_t bytes_transferred) {
                handle_write(ec, bytes_transferred);
            });
    }
}