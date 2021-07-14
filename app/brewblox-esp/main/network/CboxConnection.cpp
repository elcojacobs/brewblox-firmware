#include "CboxConnection.hpp"
#include "CboxConnectionManager.hpp"
#include "brewblox.hpp"
#include "cbox/Box.h"

CboxConnection::CboxConnection(
    CboxConnectionManager& connection_manager_,
    cbox::Box& box_)
    : buffer_in(8192)
    , buffer_out(8192)
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

void CboxConnection::start()
{
    cbox::StreamBufDataOut out_cbox(buffer_out);
    cbox::connectionStarted(out_cbox);
    do_write();
    do_read();
}

void CboxConnection::stop()
{
}

void CboxConnection::handle_read(std::error_code ec, std::size_t bytes_transferred)
{
    if (!ec) {
        cbox::StreamBufDataIn in_cbox(buffer_in);
        cbox::StreamBufDataOut out_cbox(buffer_out);
        box.handleCommand(in_cbox, out_cbox);
        do_write(); // send reply
        do_read();  // read next
    } else if (ec != asio::error::operation_aborted) {
        connection_manager.stop(shared_from_this());
    }
}

void CboxConnection::handle_write(std::error_code ec, std::size_t bytes_transferred)
{
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
}
