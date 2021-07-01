#include "CboxConnection.hpp"
#include "CboxConnectionManager.hpp"
#include "brewblox.hpp"
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

namespace cbox {
void connectionStarted(DataOut& out)
{
    char header[] = "<!BREWBLOX,";
    const std::string& version = versionCsv();
    out.writeBuffer(header, strlen(header));
    out.writeBuffer(version.data(), version.size());
    out.write(',');
    cbox::EncodedDataOut hexOut(out);

#if PLATFORM_ID == 3
    int resetReason = 0;
#else
    auto resetReason = uint8_t(0);
#endif
    hexOut.write(resetReason);
    out.write(',');
#if PLATFORM_ID == 3
    int resetData = 0;
#else
    auto resetData = uint8_t(0);
#endif
    hexOut.write(resetData);
    out.write(',');

    uint8_t deviceId[12] = {0};
    hexOut.writeBuffer(deviceId, 12);
    out.write('>');
    out.write('\n');
}
}

void CboxTcpConnection::start()
{
    cbox::StreamBufDataOut out_cbox(buffer_out);
    cbox::connectionStarted(out_cbox);
    do_write();
    do_read();
}

void CboxTcpConnection::stop()
{
    socket.close();
}

void CboxTcpConnection::do_read()
{
    auto self(shared_from_this());
    asio::async_read_until(
        socket,
        buffer_in,
        '\n',
        [this, self](std::error_code ec, std::size_t bytes_transferred) {
            if (!ec) {
                cbox::StreamBufDataIn in_cbox(buffer_in);
                cbox::StreamBufDataOut out_cbox(buffer_out);
                box.handleCommand(in_cbox, out_cbox);
                do_write(); // send reply
                do_read();  // read next
            } else if (ec != asio::error::operation_aborted) {
                connection_manager.stop(shared_from_this());
            }
        });
}

void CboxTcpConnection::do_write()
{
    if (buffer_out.size()) {
        auto self(shared_from_this());
        asio::async_write(
            socket, buffer_out,
            [this, self](std::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    // success, leave socket open.

                    // // Initiate graceful connection closure.
                    // asio::error_code ignored_ec;
                    // ESP_LOGE("CbConn", "graceful close, %s", ec.message().c_str());
                    // socket.shutdown(asio::ip::tcp::socket::shutdown_both,
                    //                 ignored_ec);
                } else if (ec != asio::error::operation_aborted) {
                    connection_manager.stop(shared_from_this());
                }
            });
    }
}