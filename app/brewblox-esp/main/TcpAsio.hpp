#pragma once

#include "esp_log.h"
#include <asio.hpp>

namespace cbox {

using asio::ip::tcp;

class AsioTcpConnection public std::enable_shared_from_this<AsioTcpConnection>
{
public:
    AsioTcpConnection(tcp::socket socket)
        : socket_(std::move(socket))
    {
    }
    virtual ~AsioTcpConnection() = default;

    void start()
    {
        do_read();
    }

    void do_read()
    {
        auto self(shared_from_this());
        asio::async_read_until(socket_, incoming, '\n',
                               [this, self](asio::error_code error, std::size_t bytes_transferred) {
                                   if (error) {
                                       ESP_LOGE("tcp_read", error.message());
                                   } else {
                                       incoming.commit(bytes_transferred);
                                       ESP_LOGI("received:", streambuf_in_.str());
                                   }
                               });
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        asio::async_write_some(socket_, outgoing,
                               [this, self](asio::error_code error, std::size_t length) {
                                   if (error) {
                                       ESP_LOGE("tcp_write", error.message());
                                   }
                               });
    }

    tcp::socket socket_;
    enum { max_length = 1024 };
    asio::streambuf incoming(max_length);
    asio::streambuf outgoing(max_length);

    // virtual DataOut& getDataOut() = 0;
    // virtual DataIn& getDataIn() = 0;
    // virtual bool isConnected() = 0;
    // virtual void stop() = 0;
};

// * @par Examples
//  * Writing directly from an streambuf to a socket:
//  * @code
//  * asio::streambuf b;
//  * std::ostream os(&b);
//  * os << "Hello, World!\n";
//  *
//  * // try sending some data in input sequence
//  * size_t n = sock.send(b.data());
//  *
//  * b.consume(n); // sent data is removed from input sequence
//  * @endcode
//  *
//  * Reading from a socket directly into a streambuf:
//  * @code
//  * asio::streambuf b;
//  *
//  * // reserve 512 bytes in output sequence
//  * asio::streambuf::mutable_buffers_type bufs = b.prepare(512);
//  *
//  * size_t n = sock.receive(bufs);
//  *
//  * // received data is "committed" from output sequence to input sequence
//  * b.commit(n);
//  *
//  * std::istream is(&b);
//  * std::string s;
//  * is >> s;
//  * @endcode
}