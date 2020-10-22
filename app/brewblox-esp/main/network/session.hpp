#pragma once

#include <asio.hpp>
#include <functional>
using namespace std::placeholders;

class session : public std::enable_shared_from_this<session> {
    using tcp = asio::ip::tcp;

public:
    session(tcp::socket&& socket)
        : socket(std::move(socket))
    {
    }

    void start()
    {
        // To start an echo session we should start to receive incoming data
        read();
    }

private:
    void read()
    {
        // Schedule asynchronous receiving of a data
        asio::async_read(socket, make_view(buffer), asio::transfer_at_least(1), std::bind(&session::on_read, shared_from_this(), _1, _2));
    }

    void on_read(asio::error_code error, std::size_t bytes_transferred)
    {
        // Check if an error has occurred or circular buffer is full
        if (!error && bytes_transferred) {
            // Check if the session isn't currently writing data
            if (!writing) {
                write();
            }

            // Read the next portion of the data
            read();
        } else {
            close();
        }
    }

    void write()
    {
        writing = true;
        // Schedule asynchronous sending of the data
        asio::async_write(socket, make_view(buffer), std::bind(&session::on_write, shared_from_this(), _1, _2));
    }

    void on_write(asio::error_code error, std::size_t bytes_transferred)
    {
        writing = false;

        if (!error) {
            // Check if there is something to send it back to the client
            if (buffer.size()) {
                write();
            }
        } else {
            close();
        }
    }

    void close()
    {
        asio::error_code error;
        socket.close(error);
    }

    tcp::socket socket;
    bool writing = false;
    circular_buffer<2048> buffer;
};