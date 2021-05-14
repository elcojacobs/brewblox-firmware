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
                ESP_LOGI("CbConn", "message received");
                char test[] = "Connected!\n";
                auto len = sizeof(test);
                std::ostream out(&buffer_out);
                out << "test\n";
                buffer_in.consume(bytes_transferred);

                do_write(); // send reply
                do_read();  // read next
            } else if (ec != asio::error::operation_aborted) {
                connection_manager.stop(shared_from_this());
            }
        });
}

void CboxTcpConnection::do_write()
{
    ESP_LOGI("CbConn", "Out buffer size %u", buffer_out.size());
    if (buffer_out.size()) {
        auto self(shared_from_this());
        asio::async_write(
            socket, make_view(buffer_out),
            [this, self](std::error_code ec, std::size_t bytes_transferred) {
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
}